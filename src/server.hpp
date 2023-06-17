#ifndef YY_SERVER
#define YY_SERVER

#include "conf.hpp"
#include "http.hpp"
#include "cache.hpp"

#include <cstdint>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <thread>
#include <mutex>
#include <vector>
#include <unistd.h>
#include <signal.h>

namespace yunying {
    class Server {
    public:
        Server();
        Server(Conf conf);
        ~Server();
        void start();
    private:
        Conf conf_;
        Cache cache_;
        uint16_t port_;
        int listen_socket_;
        std::vector<int> epoll_fds_;
        std::thread listen_thread_;
        std::vector<std::thread> request_threads_;
        int startListen();
        int startEpoll();
        int handleListen();
        int handleRequest(int worker_id);
    };

    class Connection {
    private:
        int fd_;
        HttpRequest* request_;
        HttpResponse* response_;
    public:
        Connection();
        Connection(int fd);
        ~Connection();
        void setResponse(HttpResponse* response) { response_ = response; }
        HttpResponse* getResponse() { return response_; }
        int getFd() { return fd_; }
    };
} // namespace yunying

#endif // YY_SERVER