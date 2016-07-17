#include "data_collector/common/curl_downloader.h"
#include <unistd.h>
#include <fstream>

#include "thirdparty/glog/logging.h"
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/utility/time_utility.h"
#include "app/qzap/common/utility/file_utility.h"

using std::string;

DEFINE_int32(download_timeout_seconds, 1000, "max seconds to wait for downloading");

namespace gdt {

CurlDownloader::CurlDownloader() : curl_(NULL) {
}

CurlDownloader::~CurlDownloader() {
}

bool CurlDownloader::Init() {
  curl_ = curl_easy_init();
  if (curl_ == NULL) {
    LOG(ERROR) << "curl_easy_init failed";
    return false;
  }

  curl_easy_setopt(curl_, CURLOPT_CONNECTTIMEOUT, FLAGS_download_timeout_seconds);
  curl_easy_setopt(curl_, CURLOPT_TIMEOUT, FLAGS_download_timeout_seconds);
  curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, 0);
  curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYHOST, 0);
  curl_easy_setopt(curl_, CURLOPT_ENCODING, "gzip, deflate");
  curl_easy_setopt(curl_, CURLOPT_USERAGENT,
      "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322; .NET CLR 2.0.50727)");
  curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1);
  curl_easy_setopt(curl_, CURLOPT_MAXREDIRS, 6);
  curl_easy_setopt(curl_, CURLOPT_NOSIGNAL, 1);
  return true;
}

void CurlDownloader::UnInit() {
  if (curl_ != NULL) {
    curl_easy_cleanup(curl_);
  }
}

size_t CallbackWriteFunc(void *ptr, size_t size, size_t nmemb, string *data) {
  CHECK_NOTNULL(data);
  size_t len = size * nmemb;
  data->append((char*)ptr, len);
  return len;
}

size_t CallbackWriteFileFunc(const void *ptr, size_t size, size_t nmemb, FILE *data) {
  CHECK_NOTNULL(data);
  size_t written=0;
  written= fwrite(ptr, size, nmemb, data);
  return written;
}

bool CurlDownloader::DoDownload(const string &url, string *data) {
  if (curl_ == NULL) {
    LOG(ERROR) << "curl_ is NULL";
    return false;
  }
  if (url.empty()) {
    LOG(WARNING) << "url is empty";
    return false;
  }
  if (data == NULL) {
    LOG(ERROR) << "string* data is NULL";
    return false;
  }

  curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, CallbackWriteFunc);
  curl_easy_setopt(curl_, CURLOPT_WRITEDATA, reinterpret_cast<void *>(data));

  CURLcode res = curl_easy_perform(curl_);
  long http_code = 0;
  curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &http_code);
  if (http_code != 200) {
    LOG(WARNING) << "curl perform http error, http_code=[" << http_code << "]";
    return false;
  } else if (res != CURLE_OK) {
    LOG(WARNING) << "curl perform failed, error=[" << curl_easy_strerror(res) << "]";
    return false;
  }
  return true;
}

bool CurlDownloader::DoDownloadFile(const string &url,
    const string& path) {
  if (curl_ == NULL) {
    LOG(ERROR) << "curl_ is NULL";
    return false;
  }
  if (url.empty()) {
    LOG(WARNING) << "url is empty";
    return false;
  }
  if (path.empty()) {
    LOG(ERROR) << "filepath is empty";
    return false;
  }

  FILE *fp= fopen(path.c_str(), "wb");
  if (fp == NULL) {
    LOG(WARNING) << "File "<<path<<" open failed";
    return false;
  }
  curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl_, CURLOPT_WRITEDATA, fp);
  curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, CallbackWriteFileFunc);
  CURLcode res = curl_easy_perform(curl_);
  fclose(fp);
  long http_code = 0;
  curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &http_code);
  if (http_code != 200) {
    LOG(WARNING) << "curl perform http error, http_code=[" << http_code << "]";
    return false;
  } else if (res != CURLE_OK) {
    LOG(WARNING) << "curl perform failed, error=[" << curl_easy_strerror(res) << "]";
    return false;
  }
  return true;
}

}  // namespace gdt
