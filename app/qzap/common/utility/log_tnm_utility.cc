#include "thirdparty/glog/logging.h"
#include "app/qzap/common/base/shared_ptr.h"
#include "app/qzap/common/tnm/Attr_API.h"
#include "app/qzap/common/utility/log_tnm_utility.h"
DEFINE_int32(tnm_log_level, ::google::GLOG_WARNING, "the tnm log level,0:INFO,1:WARNING,2:ERROR,3:FATAL");
class TMNLogSink : public ::google::LogSink {
 public:
   void send(
     ::google::LogSeverity severity,
     const char *full_filename,
     const char *base_filename,
     int line,
     const struct ::tm *tm_time,
     const char *message,
     size_t message_len) {
     if (static_cast<int>(severity) < FLAGS_tnm_log_level) {
       return;
     }
     bool use_base_name = FLAGS_log_strip_source_path;
     std::string s = ToString(severity,
                              use_base_name ? base_filename : full_filename,
                              line, tm_time, message, message_len);
     adv_attr_set(attr_id, s.size(), &s[0]);
   }
   int attr_id;
};

static TMNLogSink log_sink;
void InitTNMLogSink(int attr_id) {
  log_sink.attr_id = attr_id;
  ::google::AddLogSink(&log_sink);
}

static std::map<std::string, shared_ptr<TMNLogSink> >* GetGloalTNMLogSinkPool() {
  static  std::map<std::string, shared_ptr<TMNLogSink> > s_tnm_log_sink_pool;
  return &s_tnm_log_sink_pool;
}

void InitTNMLogSinkPool(const std::map<std::string, int>& key_to_attr_id) {
  for (std::map<std::string, int>::const_iterator it = key_to_attr_id.begin();
    it != key_to_attr_id.end();
    ++it) {
    shared_ptr<TMNLogSink> sink(new TMNLogSink);
    sink->attr_id = it->second;
    GetGloalTNMLogSinkPool()->insert(std::make_pair(it->first, sink));
  }
}
::google::LogSink* TNMSink(const std::string& key) {
  std::map<std::string, shared_ptr<TMNLogSink> >::const_iterator it =
    GetGloalTNMLogSinkPool()->find(key);
    if (it ==  GetGloalTNMLogSinkPool()->end()) {
      return NULL;
    }
  return  it->second.get();
}
