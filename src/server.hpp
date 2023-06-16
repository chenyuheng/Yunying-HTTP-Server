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

namespace yunying {
    class Server {
    public:
        Server();
        ~Server();
        void start();
    private:
        Conf conf_;
        Cache cache_;
        uint16_t port_;
        int listen_socket_;
        int epollFd_;
        std::mutex epoll_mutex_;
        std::thread listen_thread_;
        std::vector<std::thread> request_threads_;
        int startListen();
        int startEpoll();
        int handleListen();
        int handleRequest();
    };
} // namespace yunying

#endif // YY_SERVER