#ifndef YY_CACHE
#define YY_CACHE

#include <queue>
#include <set>
#include <string>
#include <thread>
#include <unordered_map>

#include "http.hpp"
#include "origin.hpp"

namespace yunying {
class ExpiresCompare {
 public:
  bool operator()(std::pair<int, HttpResponse*> below,
                  std::pair<int, HttpResponse*> above);
};

class Cache {
 public:
  Cache() = delete;
  Cache(Origin* origin);
  ~Cache();

  HttpResponse* get(const HttpRequest request);

 private:
  size_t max_size_bytes_;
  size_t size_bytes_;
  int clean_interval_;
  Origin* origin_;
  std::unordered_map<std::string, HttpResponse*> cache_;
  std::unordered_map<HttpResponse*, std::string> reverse_cache_;
  std::priority_queue<std::pair<int, HttpResponse*>,
                      std::vector<std::pair<int, HttpResponse*>>,
                      ExpiresCompare>
      expires_;
  std::vector<HttpResponse*> to_delete_;
  std::thread* clean_thread_;
  std::set<std::string> originating_keys_;
  std::mutex read_mutex_;
  std::mutex originating_mutex_;

  void cleanThread();
  HttpResponse* safeCacheGet(std::string key);
};
}  // namespace yunying

#endif  // YY_CACHE