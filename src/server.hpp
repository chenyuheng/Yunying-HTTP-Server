#ifndef YY_SERVER
#define YY_SERVER

#include <netinet/in.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdint>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

#include "cache.hpp"
#include "conf.hpp"
#include "connection.hpp"
#include "http.hpp"
#include "origin.hpp"

namespace yunying {
class Server {
 public:
  Server();
  ~Server();
  void start();

 private:
  Origin* origin_;
  Cache* cache_;
  uint16_t port_;
  int listen_socket_;
  std::vector<int> epoll_fds_;
  std::thread listen_thread_;
  std::vector<std::thread> request_threads_;
  HttpResponse bad_request_response_;
  int startListen();
  int startEpoll();
  int handleListen();
  int handleRequest(int worker_id);
};
}  // namespace yunying

#endif  // YY_SERVER