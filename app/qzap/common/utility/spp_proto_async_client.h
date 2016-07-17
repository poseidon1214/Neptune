#ifndef APP_QZAP_COMMON_UTILITY_SPP_PROTO_ASYNC_CLIENT_H_
#define APP_QZAP_COMMON_UTILITY_SPP_PROTO_ASYNC_CLIENT_H_
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <stack>
#include <queue>
#include <vector>
#include "app/qzap/common/base/scoped_ptr.h"
#include "app/qzap/common/base/thread.h"
#include "app/qzap/common/thread/mutex.h"
#include "app/qzap/common/utility/spp_endpoint.pb.h"
#include "base_class_old/include/qzone_protocol.h"
#include "app/qzap/common/base/walltime.h"
#include "thirdparty/qos_client/qos_client.h"
#include "thirdparty/glog/logging.h"

namespace google {
  namespace protobuf {
    class Message;
  }
}

enum TaskState
{
  TASK_STATE_INIT,
  TASK_STATE_CONNECT,
  TASK_STATE_SEND,
  TASK_STATE_RECV
};

class SppProtoAsyncClient
{
  public:
    class EventHandler
    {
      public:
        EventHandler():m_fd(-1)
        {
        }
        void set_fd(int fd)
        {
          m_fd = fd;
        }
        int get_fd()
        {
          return m_fd;
        }
        virtual int handle_event(uint32_t event) = 0; 
        virtual ~EventHandler() 
        {
          if (m_fd >= 0) 
          {
            close(m_fd);
            m_fd = -1;
          }
        }
      protected:
        int m_fd;
    };
    struct Task;
    class SppProtoAsyncClientCallback
    {
      public:
        virtual ~SppProtoAsyncClientCallback()
        {
        }
        virtual void Process(bool succ, uint32_t cmd, int version, const ::google::protobuf::Message *request, ::google::protobuf::Message *response, const SppEndpoint &spp_endpoint, bool *endup)
        {
        }
    };
    struct NotificationHandler:public EventHandler
    {
      ~NotificationHandler()
      {
      }
      virtual int handle_event(uint32_t);
    };
    struct MonitorServerCloseHandler:public EventHandler
    {
      ~MonitorServerCloseHandler()
      {
      }
      virtual int handle_event(uint32_t);
      SppProtoAsyncClient *m_async_client;
    };
    struct Task:public EventHandler
    {
      Task(SppProtoAsyncClient *async_client):
        m_send_buffer(NULL),
        m_send_offset(0),
        m_send_size(0),
        m_recv_offset(0),
        m_recv_size(0),
        m_callback(NULL),
        m_state(TASK_STATE_INIT),
        m_async_client(async_client),
        m_abs_expire_time(0),
        m_queue_index(-1),
        m_version(-1)
      {
      }
      ~Task()
      {
      }
      void Process(bool succ, bool *endup)
      {
        m_callback->Process(succ, m_cmd, m_version, m_request, m_response, m_spp_endpoint, endup);
        if (m_spp_endpoint.l5_mod_id() != 0) {
          //L5 update
          int32_t sig_time= static_cast<int32_t>((WallTime_Now() - m_init_time)) * 1000000;
          QOSREQUEST l5_req;
          l5_req._host_ip = m_spp_endpoint.host();
          l5_req._host_port = m_spp_endpoint.port();
          l5_req._modid = m_spp_endpoint.l5_mod_id();
          l5_req._cmd = m_spp_endpoint.l5_cmd_id();
          std::string error_msg;
          int32_t l5_ret = ApiRouteResultUpdate(
              l5_req, succ ? 0 : -1, sig_time, error_msg);

          if (l5_ret) {
            LOG(ERROR) << "SppProtoSendAndRecv::SendAndRecv l5_ret: " << l5_ret;
          }
        }
      }
      void reset();
      int CheckRecvPacketHead();
      bool InitSendBuffer();
      virtual int handle_event(uint32_t events);
      int terminate(bool normal);
      const char *GetStateDesp()
      {
        switch(m_state)
        {
          case TASK_STATE_INIT:
            return "init";
          case TASK_STATE_CONNECT:
            return "connecting";
          case TASK_STATE_SEND:
            return "sending";
          case TASK_STATE_RECV:
            return "recving";
          default:
            return "unexpected";
        }
      }

      char *m_send_buffer;
      int32_t m_send_offset;
      int32_t m_send_size;
      std::vector<char> m_recv_buffer;
      int32_t m_recv_offset;
      int32_t m_recv_size;
      SppEndpoint m_spp_endpoint;
      SppProtoAsyncClientCallback *m_callback;
      TaskState m_state;
      SppProtoAsyncClient *m_async_client;
      int64_t m_abs_expire_time;
      int32_t m_queue_index;
      uint32_t m_cmd;
      const ::google::protobuf::Message *m_request;
      ::google::protobuf::Message *m_response;
      scoped_ptr<QzoneProtocolPacket> m_qzone_request_packet;
      uint32_t m_sn;
      int m_version;
      double m_init_time;
    };

  private:
    class TaskExpirationQueue
    {
      public:
        TaskExpirationQueue()
        {
        }
        Task *front();
        bool empty()
        {
          return m_tasks.empty();
        }
        Task *pop_front();
        void insert(Task *task)
        {
          task->m_queue_index = m_tasks.size();
          m_tasks.push_back(task);
          upword(task);
        }
        void erase(Task *task)
        {
          int32_t index = task->m_queue_index;
          m_tasks[index] = m_tasks.back();
          m_tasks[index]->m_queue_index = index;
          m_tasks.resize(m_tasks.size()-1);
          updown(m_tasks[index]);
        }
        void updown(Task *task)
        {
          int32_t index = task->m_queue_index;
          if(index > 0 && task->m_abs_expire_time < m_tasks[(index-1)/2]->m_abs_expire_time)
          {
            upword(task);
          }
          else
          {
            downward(task);
          }
        }
        void upword(Task *task);
        void downward(Task *task);
        void swap(int32_t i, int32_t j)
        {
          Task *tmp = m_tasks[i];
          m_tasks[i] = m_tasks[j];
          m_tasks[j] = tmp;
          m_tasks[i]->m_queue_index = i;
          m_tasks[j]->m_queue_index = j;

        }
        int32_t size()
        {
          return static_cast<int32_t>(m_tasks.size());
        }
      private:
        vector<Task*> m_tasks;
    };
    struct AddrInfo
    {
      AddrInfo():m_connection_limit(SppProtoAsyncClient::kDefaultMaxConcurrentTask)
      {
      }
      AddrInfo(int32_t connection_limit):m_connection_limit(connection_limit)
      {
      }
      int32_t m_connection_limit;
      std::vector<int> m_fds;
    };

  public:
    SppProtoAsyncClient(int32_t max_concurrent_task=kDefaultMaxConcurrentTask):m_task_pool_size_limit(DEFAULT_TASK_POOL_SIZE_LIMIT),
      m_max_concurrent_task(max_concurrent_task),
      m_epoll_fd(-1),m_return_events(NULL),m_exit(false)
    {
      m_task_notification_pipe_fds[0] = m_task_notification_pipe_fds[1] = -1;
    }
    ~SppProtoAsyncClient();
    int init();
    bool Call(uint32_t cmd, int version, const ::google::protobuf::Message *request,
        ::google::protobuf::Message *response, const SppEndpoint &spp_endpoint, SppProtoAsyncClientCallback *callback); 
    Task* alloc_task();
    void return_task(Task *task);
    void return_monitor_server_close_handler(SppProtoAsyncClient::MonitorServerCloseHandler *handler);
    int remove_from_expireation_queue(SppProtoAsyncClient::Task *task);
    int close_fd(int fd);
    MonitorServerCloseHandler* monitor_server_close(int fd);
    void add_task(Task *task);
    Task *get_task();
    int notify_new_task();
    int expire_time_change(Task *task);
    bool InitHostPort(SppEndpoint &endpoint);
    int init_task(Task *task);
    int add_listen(EventHandler *handler, uint32_t events);
    int cancel_listen(EventHandler *task);
    int modify_listen(EventHandler *task, uint32_t events);
    int get_epoll_wait_time();
    int check_timeout();
    Thread *Start();
    Thread *Stop();

    void run();
    int get_fd(const string &host, int port);
    int return_fd(const string &host, int port, int fd);
    static const int32_t DEFAULT_TASK_POOL_SIZE_LIMIT = 1024;
    static const int32_t kDefaultMaxConcurrentTask = 64;
  private:
    Mutex m_mutex;
    queue<Task*> m_task_queue;
    stack<Task*> m_task_pool;
    int32_t m_task_pool_size_limit;
    int32_t m_max_concurrent_task;
    int m_task_notification_pipe_fds[2];
    int m_epoll_fd;
    epoll_event *m_return_events;
    TaskExpirationQueue m_expiration_queue;
    map<string, AddrInfo> m_addr_info;
    map<int, std::pair<std::string, MonitorServerCloseHandler*> > m_fd_info;
    uint64_t m_sn;
    scoped_ptr<Thread> m_thread;
    bool m_exit;
    NotificationHandler m_notification_handler;
    stack<MonitorServerCloseHandler*> m_monitor_server_close_handlers; 
};
#endif
