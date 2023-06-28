#ifndef YY_ORIGIN
#define YY_ORIGIN

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include "conf.hpp"
#include "connection.hpp"
#include "http.hpp"
#include "util.hpp"

namespace yunying {
class Origin {
 public:
  virtual std::string getKey(HttpRequest request) = 0;
  virtual HttpResponse* get(HttpRequest request, int* max_age) = 0;
};

class StaticFileOrigin : public Origin {
 private:
  std::string root_dir_;
  std::string handleDefault(std::string path);

 public:
  StaticFileOrigin();
  ~StaticFileOrigin();
  std::string getKey(HttpRequest request);
  HttpResponse* get(HttpRequest request, int* max_age);
};

class UpstreamOrigin : public Origin {
 public:
  UpstreamOrigin();
  ~UpstreamOrigin();
  std::string getKey(HttpRequest request);
  HttpResponse* get(HttpRequest request, int* max_age);
};
}  // namespace yunying

#endif