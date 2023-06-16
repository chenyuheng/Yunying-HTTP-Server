#include "server.hpp"

#include "http.hpp"
#include <unistd.h>

namespace yunying {
    Server::Server() {
        conf_ = Conf();
        cache_ = Cache(conf_.get_cache_size_bytes());
        port_ = conf_.get_port();
    }

    Server::~Server() {
        // TODO
    }

    int Server::startListen() {
        int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (listenSocket < 0) {
            throw std::runtime_error("Failed to create socket");
        }
        struct sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(port_);
        if (bind(listenSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
            throw std::runtime_error("Failed to bind socket");
        }
        if (listen(listenSocket, 5) < 0) {
            throw std::runtime_error("Failed to listen socket");
        }
        return listenSocket;
    }

    int Server::startEpoll() {
        int epollFd = epoll_create1(0);
        if (epollFd < 0) {
            throw std::runtime_error("Failed to create epoll");
        }
        return epollFd;
    }

    int Server::handleListen() {
        while (true) {
            struct sockaddr_in clientAddress;
            socklen_t clientAddressLength = sizeof(clientAddress);
            int clientSocket = accept(listen_socket_, (struct sockaddr *) &clientAddress, &clientAddressLength);
            if (clientSocket < 0) {
                throw std::runtime_error("Failed to accept client socket");
            }
            struct epoll_event event;
            event.events = EPOLLIN;
            event.data.fd = clientSocket;
            if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, clientSocket, &event) < 0) {
                throw std::runtime_error("Failed to add client socket to epoll");
            }
        }
    }

    int Server::handleRequest() {
        printf("[IN]working thread started\n");
        struct epoll_event events[1];
        while (true) {
            epoll_mutex_.lock();
            int n = epoll_wait(epollFd_, events, 1, -1);
            epoll_mutex_.unlock();
            if (n == 0 || events[0].data.fd == listen_socket_) {
                continue;
            }
            int fd = events[0].data.fd;
            HttpRequest request = HttpRequest(fd);
            HttpResponse response = cache_.get(request);
            response.send(fd);
            epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, NULL);
            close(fd);
        }
    }

    void Server::start() {
        printf("Server started\n");
        listen_socket_ = startListen();
        printf("Listening on port %d\n", port_);
        epollFd_ = startEpoll();
        printf("Epoll started\n");
        listen_thread_ = std::thread(&Server::handleListen, this);
        printf("Listening thread started\n");
        for (int i = 0; i < conf_.get_working_threads(); i++) {
            printf("Starting working thread %d\n", i);
            request_threads_.push_back(std::thread(&Server::handleRequest, this));
        }
        printf("All working threads started\n");
    }
}