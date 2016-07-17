#include "spp_proto_async_client.h"

SppProtoAsyncClient::~SppProtoAsyncClient()
{
  if (!m_exit) {
    Thread *thread = Stop();
    if (thread) {
      thread->Join();
    }
  }
  while(!m_task_queue.empty())
  {
    Task * task = m_task_queue.front();
    m_task_queue.pop();
    task->terminate(false);
  }
  if(m_epoll_fd >= 0)
  {
    close(m_epoll_fd);
    m_epoll_fd = -1;
  }
  if(m_return_events != NULL)
  {
    delete []m_return_events;
    m_return_events = NULL;
  }
  if(m_task_notification_pipe_fds[0] >= 0)
  {
    close(m_task_notification_pipe_fds[0]);
    m_task_notification_pipe_fds[0] = -1;
  }
  if(m_task_notification_pipe_fds[1] >= 0)
  {
    close(m_task_notification_pipe_fds[1]);
    m_task_notification_pipe_fds[1] = -1;
  }
  for(map<string, AddrInfo>::iterator iter=m_addr_info.begin();
      iter != m_addr_info.end();
      iter++)
  {
    for( size_t i = 0; i < iter->second.m_fds.size(); ++i)
    {
      close(iter->second.m_fds[i]);
    }
  }
  while(!m_task_pool.empty())
  {
    Task *task = m_task_pool.top();
    delete task;
    m_task_pool.pop();
  }
  while (!m_monitor_server_close_handlers.empty()) 
  {
    delete m_monitor_server_close_handlers.top();
    m_monitor_server_close_handlers.pop();
  }
}
int SppProtoAsyncClient::init()
{
  pipe(m_task_notification_pipe_fds);
  int flags = fcntl(m_task_notification_pipe_fds[0], F_GETFL, 0);
  fcntl(m_task_notification_pipe_fds[0], F_SETFL, flags|O_NONBLOCK);
  flags = fcntl(m_task_notification_pipe_fds[1], F_GETFL, 0);
  fcntl(m_task_notification_pipe_fds[1], F_SETFL, flags|O_NONBLOCK);

  m_epoll_fd = epoll_create(1024);
  if(m_epoll_fd < 0)
  {
    return -1;
  }
  m_notification_handler.set_fd(m_task_notification_pipe_fds[0]);
  add_listen(&m_notification_handler, EPOLLIN);
  m_return_events = new epoll_event[1024];
  return 0;


}

Thread *SppProtoAsyncClient::Start()
{
  if (init() != 0) 
  {
    return NULL;
  }
  m_thread.reset(new Thread(NewCallback(this, &SppProtoAsyncClient::run)));
  m_thread->Start();
  return m_thread.get();
}


Thread *SppProtoAsyncClient::Stop()
{
  m_exit = true;
  //这里只是简单把线程从epoll_wait激活
  if (notify_new_task() == 0) {
    return m_thread.get();
  }
  else {
    return NULL;
  }
}

bool SppProtoAsyncClient::Call(uint32_t cmd, int version, const ::google::protobuf::Message *request,
    ::google::protobuf::Message *response, const SppEndpoint &spp_endpoint, SppProtoAsyncClient::SppProtoAsyncClientCallback *callback) 
{

  Task * task = alloc_task();
  if (task == NULL)
  {
    return false;
  }
  task->m_spp_endpoint = spp_endpoint;
  if (!InitHostPort(task->m_spp_endpoint))
  {
    return false;
  }
  task->m_callback = callback;
  task->m_async_client = this;
  task->m_cmd = cmd;
  task->m_request = request;
  task->m_response = response;
  task->m_sn = m_sn++;
  task->m_version = version;
  task->InitSendBuffer();


  add_task(task);
  return true;
}
SppProtoAsyncClient::Task* SppProtoAsyncClient::alloc_task()
{
  Task *task = NULL;
  m_mutex.Lock();
  if(!m_task_pool.empty())
  {
    task = m_task_pool.top();
    m_task_pool.pop();
  }
  m_mutex.Unlock();
  if(task == NULL)
  {
    task = new (std::nothrow)Task(this);
  }
  return task;
}
void SppProtoAsyncClient::return_task(SppProtoAsyncClient::Task *task)
{
  MutexLock lock(&m_mutex);
  if((int)m_task_pool.size() >= m_task_pool_size_limit)
  {
    delete task;
  }
  else
  {
    m_task_pool.push(task);
  }
}
void SppProtoAsyncClient::add_task(SppProtoAsyncClient::Task *task)
{
  m_mutex.Lock();
  m_task_queue.push(task);
  m_mutex.Unlock();
  notify_new_task();
}
SppProtoAsyncClient::Task *SppProtoAsyncClient::get_task()
{
  Task *task = NULL;
  MutexLock lock(&m_mutex);
  if (m_expiration_queue.size() >= m_max_concurrent_task)
  {
    return task;
  }
  if(!m_task_queue.empty())
  {
    task = m_task_queue.front();
    m_task_queue.pop();
  }
  return task;
}
int SppProtoAsyncClient::notify_new_task()
{
  char ch=0;
  if(write(m_task_notification_pipe_fds[1], &ch, 1) == 1)
  {
    return 0;
  }
  else
  {
    return -1;
  }
}
int SppProtoAsyncClient::expire_time_change(SppProtoAsyncClient::Task *task)
{
  m_expiration_queue.updown(task);
  return 0;
}
bool SppProtoAsyncClient::InitHostPort(SppEndpoint &endpoint) 
{
  if (!endpoint.host().empty()) 
  {
    return true;
  }
  QOSREQUEST req;
  req._modid = endpoint.l5_mod_id();
  req._cmd = endpoint.l5_cmd_id();
  string err_msg;
  float tm_out = 0.2;
  int ret = ApiGetRoute(req, tm_out, err_msg);
  if (ret < 0) {
    if (ret == QOS_RTN_OVERLOAD) {
      LOG(ERROR) << "Service overload, modid: " << req._modid
        << " cmd: " << req._cmd;
      return false;
    }
    LOG(ERROR) << "ApiGetRoute failed, ret: " << ret
      << " err: " << err_msg;
    return false;
  }
  endpoint.set_host(req._host_ip);
  endpoint.set_port(req._host_port);
  return true;
}
int SppProtoAsyncClient::init_task(SppProtoAsyncClient::Task *task)
{
  task->m_init_time = WallTime_Now();
  std::string host = task->m_spp_endpoint.host();
  int port = task->m_spp_endpoint.port();
  int send_timeout = task->m_spp_endpoint.send_timeout();
  int conn_timeout = task->m_spp_endpoint.conn_timeout();
  int sock = get_fd(host, port);
  if(sock == -1)
  {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags|O_NONBLOCK);
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, host.c_str(), &(addr.sin_addr));
    addr.sin_port = htons(port);
    int connect_ret = ::connect(sock, (sockaddr*)&addr, sizeof(addr));
    if(connect_ret == 0)
    {
      task->set_fd(sock);
      task->m_state = TASK_STATE_SEND;
      add_listen(task, EPOLLIN|EPOLLOUT);
      task->m_abs_expire_time = static_cast<int64_t>(WallTime_Now())*1000+send_timeout;
      m_expiration_queue.insert(task);
    }
    else if(errno == EINPROGRESS)
    {
      task->set_fd(sock);
      task->m_state = TASK_STATE_CONNECT;
      add_listen(task, EPOLLIN|EPOLLOUT);
      task->m_abs_expire_time = static_cast<int64_t>(WallTime_Now())*1000+conn_timeout;
      m_expiration_queue.insert(task);
    }
    else
    {
      LOG(WARNING) << "connect to spp endpoint fail, endpoint:" << task->m_spp_endpoint.DebugString() 
        << " errno:" << errno;
      close(sock);
      task->Process(false, NULL);
      task->terminate(false);
      return -1;
    }
  }
  else
  {
    task->set_fd(sock);
    task->m_state = TASK_STATE_SEND;
    add_listen(task, EPOLLIN|EPOLLOUT);
    task->m_abs_expire_time = static_cast<int64_t>(WallTime_Now())*1000+send_timeout;
    m_expiration_queue.insert(task);
  }
  return 0;
}
int SppProtoAsyncClient::add_listen(SppProtoAsyncClient::EventHandler *handler, uint32_t events)
{
  epoll_event event;
  event.data.ptr = handler;
  event.events = events;
  return epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, handler->get_fd(), &event);
}
int SppProtoAsyncClient::remove_from_expireation_queue(SppProtoAsyncClient::Task *task)
{
  m_expiration_queue.erase(task);
  return 0;
}
int SppProtoAsyncClient::cancel_listen(SppProtoAsyncClient::EventHandler *task)
{
  epoll_event event;
  return epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, task->get_fd(), &event);
}
int SppProtoAsyncClient::modify_listen(SppProtoAsyncClient::EventHandler *task, uint32_t events)
{
  epoll_event event;
  event.data.ptr = task;
  event.events = events;
  return epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, task->get_fd(), &event);
}


int SppProtoAsyncClient::get_epoll_wait_time()
{
  int wait = 0;
  if(!m_expiration_queue.empty())
  {
    Task *task = m_expiration_queue.front();
    int64_t cur_time = static_cast<int64_t>(WallTime_Now())*1000;
    wait = task->m_abs_expire_time-cur_time;
    if(wait < 0)
    {
      wait = 0;
    }
    else if(wait > 1000)
    {
      wait = 1000;
    }
    return wait;
  }
  else
  {
    return 1000;
  }
}
int SppProtoAsyncClient::check_timeout()
{
  int64_t cur_time = static_cast<int64_t>(WallTime_Now())*1000;
  Task *task = m_expiration_queue.front();
  while(task != NULL)
  {
    if(task->m_abs_expire_time <= cur_time)
    {
      LOG(WARNING) << "task expired, state:" << task->GetStateDesp() << " endpoint:" << task->m_spp_endpoint.DebugString(); 
      task->Process(false, NULL);
      task->terminate(false);
      task = m_expiration_queue.front();
    }
    else
    {
      break;
    }
  }
  return 0;
}

void SppProtoAsyncClient::run()
{
  Task *task = NULL;
  while(true)
  {
    int fd_num = epoll_wait(m_epoll_fd, m_return_events, 1024, get_epoll_wait_time());
    if (m_exit) {
      return;
    }
    if(fd_num > 0)
    {
      for(int i=0; i<fd_num; i++)
      {
        EventHandler *handler = static_cast<EventHandler*>(m_return_events[i].data.ptr);
        handler->handle_event(m_return_events[i].events);
      }
    }
    check_timeout();
    while((task=get_task()) != NULL)
    {
      init_task(task);
    }
  }
}

int SppProtoAsyncClient::get_fd(const string &host, int port)
{
  int fd = -1;
  string key(host);
  key.append((char*)&port, sizeof(port));
  map<string, AddrInfo>::iterator iter = m_addr_info.find(key);
  if(iter != m_addr_info.end() && !iter->second.m_fds.empty())
  {
    fd = iter->second.m_fds.back();
    iter->second.m_fds.pop_back();
    cancel_listen(m_fd_info[fd].second);
    return_monitor_server_close_handler(m_fd_info[fd].second);
    m_fd_info.erase(fd);
  }
  return fd;
}

void SppProtoAsyncClient::return_monitor_server_close_handler(SppProtoAsyncClient::MonitorServerCloseHandler *handler)
{
  m_monitor_server_close_handlers.push(handler);
}
int SppProtoAsyncClient::return_fd(const string &host, int port, int fd)
{
  string key(host);
  key.append((char*)&port, sizeof(port));
  map<string, AddrInfo>::iterator iter = m_addr_info.find(key);
  if(iter != m_addr_info.end())
  {
    if((int)iter->second.m_fds.size() < iter->second.m_connection_limit)
    {
      iter->second.m_fds.push_back(fd);
      m_fd_info[fd] = std::make_pair(key, monitor_server_close(fd));
    }
    else
    {
      close(fd);
    }
  }
  else
  {
    AddrInfo info(m_max_concurrent_task);
    info.m_fds.push_back(fd);
    m_addr_info[key] = info;
    m_fd_info[fd] = std::make_pair(key, monitor_server_close(fd));
  }
  return 0;
}
SppProtoAsyncClient::MonitorServerCloseHandler * SppProtoAsyncClient::monitor_server_close(int fd)
{
  MonitorServerCloseHandler *handler = NULL;
  if (!m_monitor_server_close_handlers.empty()) 
  {
    handler = m_monitor_server_close_handlers.top();
    m_monitor_server_close_handlers.pop();
  }
  else
  {
    handler = new MonitorServerCloseHandler();
    handler->m_async_client = this;
  }
  handler->set_fd(fd);
  add_listen(handler, EPOLLIN);
  return handler;
}

int SppProtoAsyncClient::close_fd(int fd)
{
  map<int, std::pair<std::string, MonitorServerCloseHandler*> >::iterator iter = m_fd_info.find(fd);
  if (iter == m_fd_info.end()) {
    return -1;
  }
  map<string, AddrInfo>::iterator addr_iter = m_addr_info.find(iter->second.first);
  if (addr_iter == m_addr_info.end()) {
    return_monitor_server_close_handler(iter->second.second);
    m_fd_info.erase(iter);
    LOG(ERROR) << "fd and addr info inconsident";
    return -1;
  }
  for (std::vector<int>::iterator fd_iter=addr_iter->second.m_fds.begin(); 
      fd_iter != addr_iter->second.m_fds.end();
      fd_iter++) {
    if (*fd_iter == fd) {
      addr_iter->second.m_fds.erase(fd_iter);
      return_monitor_server_close_handler(iter->second.second);
      m_fd_info.erase(iter);
      close(fd);
      return 0;
    }
  }
  return_monitor_server_close_handler(iter->second.second);
  m_fd_info.erase(iter);
  LOG(ERROR) << "fd and addr info inconsident";
  return -1;
}



SppProtoAsyncClient::Task * SppProtoAsyncClient::TaskExpirationQueue::front()
{
  if(!m_tasks.empty())
  {
    return m_tasks[0];
  }
  else
  {
    return NULL;
  }

}

SppProtoAsyncClient::Task *SppProtoAsyncClient::TaskExpirationQueue::pop_front()
{
  Task *task = NULL;
  if(m_tasks.empty())
  {
    return task;
  }
  task = m_tasks[0];
  m_tasks[0] = m_tasks.back();
  m_tasks[0]->m_queue_index = 0;
  m_tasks.resize(m_tasks.size()-1);
  downward(m_tasks[0]);
  return task;
}

void SppProtoAsyncClient::TaskExpirationQueue::upword(SppProtoAsyncClient::Task *task)
{
  int32_t index = task->m_queue_index;
  int32_t parent_index;
  while(index >0)
  {
    parent_index = (index-1)/2;
    if(m_tasks[parent_index]->m_abs_expire_time > task->m_abs_expire_time)
    {
      swap(index, parent_index);
      index = parent_index;
    }
    else
    {
      break;
    }
  }
}
void SppProtoAsyncClient::TaskExpirationQueue::downward(SppProtoAsyncClient::Task *task)
{
  int32_t index = task->m_queue_index;
  int32_t l_child_index, r_child_index;
  int32_t min_index;
  while(index < (int)m_tasks.size()/2)
  {
    l_child_index = index*2+1;
    r_child_index = index*2+2;
    min_index = l_child_index;
    if(r_child_index < (int)m_tasks.size() && m_tasks[r_child_index]->m_abs_expire_time < m_tasks[min_index]->m_abs_expire_time)
    {
      min_index = r_child_index;
    }
    if(m_tasks[min_index]->m_abs_expire_time < m_tasks[index]->m_abs_expire_time)
    {
      swap(index, min_index);
      index = min_index;
    }
    else
    {
      break;
    }
  }
}


void SppProtoAsyncClient::Task::reset()
{
  m_send_offset = 0;
  m_send_size = 0;
  m_recv_offset = 0;
  m_recv_size = 0;
  m_callback = NULL;
  m_state = TASK_STATE_INIT;
  m_spp_endpoint.Clear();
  m_abs_expire_time = 0;
  m_queue_index = -1;
  m_version = -1;
}

bool SppProtoAsyncClient::Task::InitSendBuffer()
{
  int ret;
  std::string request_buffer;
  if (!m_request->AppendToString(&request_buffer)) {
    LOG(ERROR) << "Fail to serialize request: " << m_request->DebugString();
    return false;
  }
  m_qzone_request_packet.reset(new QzoneProtocolPacket());
  const int packet_length = request_buffer.size() + sizeof(*m_qzone_request_packet);
  VLOG(2) << "packet_length: " << packet_length << " request_buffer size: " << request_buffer.size();
  ret = m_qzone_request_packet->CreatePacket(packet_length);
  if (ret != 0) {
    LOG(ERROR) << "Fail to CreatePacket, packet_length:" << packet_length;
    return false;
  }

  m_qzone_request_packet->SetHead(
      static_cast<uint8_t>(m_version),
      static_cast<uint32_t>(m_cmd));
  m_qzone_request_packet->SetSerialNo(m_sn);
  m_qzone_request_packet->SetServerResponse(QzoneServerSucc, 0);
  m_qzone_request_packet->AppendData(request_buffer.c_str(), request_buffer.size());
  ret = m_qzone_request_packet->Output(m_send_buffer, m_send_size);
  if (ret != 0) {
    LOG(ERROR) << "SppProtoSendAndRecv::SendAndRecv packet output fail, ret: " << ret;
    return false;
  }

  VLOG(2) << "Write to socket, length: " << m_send_size;
  return true;
}

int SppProtoAsyncClient::Task::terminate(bool normal)
{
  if(m_fd >= 0)
  {
    m_async_client->remove_from_expireation_queue(this);
    m_async_client->cancel_listen(this);
    if(normal)
    {
      m_async_client->return_fd(m_spp_endpoint.host(), m_spp_endpoint.port(), m_fd);
    }
    else
    {
      ::close(m_fd);
    }
    m_fd = -1;
  }
  reset();
  m_async_client->return_task(this);
  return 0;
}



int SppProtoAsyncClient::Task::handle_event(uint32_t events)
{
  int32_t left, sent, fetch, got;
  switch(m_state)
  {
    case TASK_STATE_CONNECT:
      {
        int error = 0;
        socklen_t len = sizeof(error);
        if((events&EPOLLOUT) && getsockopt(m_fd, SOL_SOCKET, SO_ERROR, &error, &len) >= 0 && error == 0)
        {
          m_state = TASK_STATE_SEND;
          m_abs_expire_time = static_cast<int64_t>(WallTime_Now())*1000+m_spp_endpoint.send_timeout();
          m_async_client->expire_time_change(this);
        }
        else
        {
          char error_msg[1024];
          LOG(WARNING) << "connect fail, errno:" << errno << " error_msg:" << 
            strerror_r(errno, error_msg, sizeof(error_msg)) << " spp_endpoint:" <<
            m_spp_endpoint.DebugString(); 
          goto ERR_RETURN;
        }
        break;
      }
    case TASK_STATE_SEND:
      {
        left = m_send_size-m_send_offset;
        sent = send(m_fd, m_send_buffer+m_send_offset, left, MSG_NOSIGNAL);
        if(sent <= 0)
        {
          if(errno == EAGAIN || errno == EWOULDBLOCK)
          {
            return 0;
          }
          char error_msg[1024];
          LOG(WARNING) << "send fail, errno:" << errno << " error_msg:" << 
            strerror_r(errno, error_msg, sizeof(error_msg)) << " spp_endpoint:" <<
            m_spp_endpoint.DebugString(); 
          goto ERR_RETURN;
        }
        m_send_offset += sent;
        if(m_send_offset == m_send_size)
        {
          m_state = TASK_STATE_RECV;
          m_async_client->modify_listen(this, EPOLLIN);
          m_abs_expire_time = static_cast<int64_t>(WallTime_Now())*1000+m_spp_endpoint.recv_timeout();
          m_async_client->expire_time_change(this);
        }
        break;
      }
    case TASK_STATE_RECV:
      {
        QzoneProtocolPacket recv_packet;
        int head_size = recv_packet.headSize();
        if(m_recv_offset < head_size)
        {
          m_recv_buffer.resize(head_size);
          fetch = head_size-m_recv_offset;
          got = recv(m_fd, &m_recv_buffer[0]+m_recv_offset, fetch, 0);
          if(got >0)
          {
            m_recv_offset += got;
            if (m_recv_offset < head_size)
            {
              return 0;
            }
            if (CheckRecvPacketHead() != 0)
            {
              LOG(WARNING) << "CheckRecvPacketHead fail, spp_endpoint:" <<
                m_spp_endpoint.DebugString(); 
              goto ERR_RETURN;
            }
          }
          else if(got < 0)
          {
            if(errno == EAGAIN || errno == EWOULDBLOCK)
            {
              return 0;
            }
            else
            {
              char error_msg[1024];
              LOG(WARNING) << "recv fail, errno:" << errno << " error_msg:" << 
                strerror_r(errno, error_msg, sizeof(error_msg)) << " spp_endpoint:" <<
                m_spp_endpoint.DebugString(); 
              goto ERR_RETURN;
            }
          }
          else
          {
            char error_msg[1024];
            LOG(WARNING) << "recv fail, errno:" << errno << " error_msg:" << 
              strerror_r(errno, error_msg, sizeof(error_msg)) << " spp_endpoint:" <<
              m_spp_endpoint.DebugString(); 
            goto ERR_RETURN;
          }
        }
        fetch = m_recv_size-m_recv_offset;
        got = recv(m_fd, &m_recv_buffer[0]+m_recv_offset, fetch, 0);
        if(got == 0)
        {
          char error_msg[1024];
          LOG(WARNING) << "recv fail, errno:" << errno << " error_msg:" << 
            strerror_r(errno, error_msg, sizeof(error_msg)) << " spp_endpoint:" <<
            m_spp_endpoint.DebugString(); 
          goto ERR_RETURN;
        }
        else if(got < 0)
        {
          if(errno == EAGAIN || errno == EWOULDBLOCK)
          {
            return 0;
          }
          else
          {
            char error_msg[1024];
            LOG(WARNING) << "recv fail, errno:" << errno << " error_msg:" << 
              strerror_r(errno, error_msg, sizeof(error_msg)) << " spp_endpoint:" <<
              m_spp_endpoint.DebugString(); 
            goto ERR_RETURN;
          }
        }
        else
        {
          m_recv_offset += got;
        }
        if(m_recv_offset == m_recv_size)
        {
          recv_packet.MapPacketHeadBuffer(&m_recv_buffer[0], head_size);
          int response_size = recv_packet.bodySize(recv_packet.mappedLen());
          if (!m_response->ParseFromArray(&m_recv_buffer[head_size], response_size)) {
            LOG(ERROR) << "SppProtoSendAndRecv::SendAndRecv response ParseFromArray fail, response_size = " << response_size;
            goto ERR_RETURN;
          }
          else
          {
            bool endup = true;
            Process(true, &endup);
            if (endup) {
              terminate(true);
            }
            else {
              m_recv_offset = 0;
            }
            return 0;
          }
        }
        break;
      }
    default:
      return -1;
  }
  return 0;
ERR_RETURN:
  Process(false, NULL);
  terminate(false);
  return -1;
}
int SppProtoAsyncClient::Task::CheckRecvPacketHead()
{
  QzoneProtocolPacket recv_packet;
  int head_size = recv_packet.headSize();
  int ret = recv_packet.MapPacketHeadBuffer(&m_recv_buffer[0], head_size);
  if (0 != ret) {
    LOG(ERROR) << "SppProtoSendAndRecv::SendAndRecv MapPacketHeadBuffer fail, ret: " << ret;
    return -1;
  }
  if (recv_packet.mappedSerialNo() != m_sn)
  {
    LOG(ERROR) << "recv serialno = " << recv_packet.mappedSerialNo() << ", send sn = "<< m_sn << ", respFlag = " << (int)recv_packet.m_pPacket->head.serverResponseFlag;
  }
  if (0 != recv_packet.CheckPacketBuffer(&m_recv_buffer[0])) {
    LOG(ERROR) << "SppProtoSendAndRecv::SendAndRecv CheckPacketBuffer fail";
    return -1;
  }
  if (0 != recv_packet.mappedServerResponseFlag()) {
    LOG(ERROR) << "SppProtoSendAndRecv::SendAndRecv recv_packet.mappedServerResponseFlag fail, ret: "
      << recv_packet.mappedServerResponseFlag();
    return -1;
  }
  if (0 != recv_packet.mappedServerResponseInfo()) {
    LOG(ERROR) << "SppProtoSendAndRecv::SendAndRecv recv_packet.mappedServerResponseInfo fail, ret: "
      << recv_packet.mappedServerResponseInfo();
    return -1;
  }
  m_recv_size = recv_packet.mappedLen();
  m_recv_buffer.resize(m_recv_size);
  return 0;
}
int SppProtoAsyncClient::NotificationHandler::handle_event(uint32_t event) 
{
  char buffer[1024];
  int read_ret = 0;
  while((read_ret=read(m_fd, buffer, sizeof(buffer))) == (int)sizeof(buffer))
  {
  }
  return 0;
}
int SppProtoAsyncClient::MonitorServerCloseHandler::handle_event(uint32_t event)
{
  m_async_client->cancel_listen(this);
  m_async_client->close_fd(m_fd);
  LOG(INFO) << "server close connecion";
  return 0;
}

