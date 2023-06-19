#include "server.hpp"

namespace yunying {
    Server::Server() {
        origin_ = new StaticFileOrigin(Conf::getInstance().get_root_dir());
        cache_ = new Cache(origin_, Conf::getInstance().get_cache_size_bytes());
        port_ = Conf::getInstance().get_port();
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
        int current_epoll_id = 0;
        int working_threads_num = Conf::getInstance().get_working_threads_num();
        while (true) {
            struct sockaddr_in clientAddress;
            socklen_t clientAddressLength = sizeof(clientAddress);
            int client_fd = accept(listen_socket_, (struct sockaddr *) &clientAddress, &clientAddressLength);
            if (client_fd < 0) {
                throw std::runtime_error("Failed to accept client socket");
            }
            struct epoll_event event;
            event.events = EPOLLIN;
            event.data.ptr = (void*) new Connection(client_fd);
            if (epoll_ctl(epoll_fds_[current_epoll_id++], EPOLL_CTL_ADD, client_fd, &event) < 0) {
                perror("epoll_ctl");
                throw std::runtime_error("Failed to add client socket to epoll");
            }
            current_epoll_id %= working_threads_num;
        }
    }

    int Server::handleRequest(int worker_id) {
        printf("[IN]working thread started\n");
        struct epoll_event events[10];
        while (true) {
            int n = epoll_wait(epoll_fds_[worker_id], events, 10, -1);
            for (int i = 0; i < n; i++) {
                Connection* conn = reinterpret_cast<Connection *>(events[i].data.ptr);
                int fd = conn->getFd();
                if (events[i].events & EPOLLIN) {
                    conn->recv();
                    if (conn->getFdClosed()) {
                        epoll_ctl(epoll_fds_[worker_id], EPOLL_CTL_DEL, fd, NULL);
                        delete conn;
                    } else if (conn->getRecvDone()) {
                        HttpRequest req = HttpRequest(conn->getReceivedRaw());
                        HttpResponse* resp = cache_->get(req);
                        conn->setResponse(resp);
                        events[i].events = EPOLLOUT;
                        epoll_ctl(epoll_fds_[worker_id], EPOLL_CTL_MOD, fd, &events[i]);
                    }
                } else if (events[i].events & EPOLLOUT) {
                    if (!conn->getSendStarted()) {
                        conn->setSendRaw(conn->getResponse()->to_string());
                    }
                    conn->send();
                    if (conn->getSendDone()) {
                        events[i].events = EPOLLIN;
                        epoll_ctl(epoll_fds_[worker_id], EPOLL_CTL_MOD, fd, &events[i]);
                    }
                } else {
                    close(fd);
                    epoll_ctl(epoll_fds_[worker_id], EPOLL_CTL_DEL, fd, NULL);
                    delete conn;
                }
            }
        }
    }

    void Server::start() {
        signal(SIGPIPE, SIG_IGN);
        printf("Server started\n");
        listen_socket_ = startListen();
        printf("Listening on port %d\n", port_);
        for (int i = 0; i < Conf::getInstance().get_working_threads_num(); i++) {
            epoll_fds_.push_back(startEpoll());
        }
        printf("Epoll started\n");
        listen_thread_ = std::thread(&Server::handleListen, this);
        printf("Listening thread started\n");
        for (int i = 0; i < Conf::getInstance().get_working_threads_num(); i++) {
            printf("Starting working thread %d\n", i);
            request_threads_.push_back(std::thread(&Server::handleRequest, this, i));
        }
        printf("All working threads started\n");
    }
}