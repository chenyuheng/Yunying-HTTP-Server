#include "server.hpp"

namespace yunying {
    Server::Server(Conf conf) {
        conf_ = conf;
        cache_ = Cache(conf_.get_cache_size_bytes());
        port_ = conf_.get_port();
    }

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
        int current_epoll_id = 0;
        int working_threads_num = conf_.get_working_threads_num();
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
                    HttpRequest request = HttpRequest(fd);
                    if (request.failed()) {
                        epoll_ctl(epoll_fds_[worker_id], EPOLL_CTL_DEL, fd, NULL);
                        continue;
                    }
                    HttpResponse* response = cache_.get(request);
                    conn->setResponse(response);
                    struct epoll_event event;
                    event.events = EPOLLOUT;
                    event.data.ptr = (void *) conn;
                    epoll_ctl(epoll_fds_[worker_id], EPOLL_CTL_MOD, fd, &event);
                } else if (events[i].events & EPOLLOUT) {
                    HttpResponse *response = conn->getResponse();
                    response->send(fd);
                    struct epoll_event event;
                    event.events = EPOLLIN;
                    event.data.ptr = (void *) conn;
                    epoll_ctl(epoll_fds_[worker_id], EPOLL_CTL_MOD, fd, &event);
                } else if (events[i].events & EPOLLRDHUP ) {
                    printf("Client closed connection\n");
                    close(fd);
                    epoll_ctl(epoll_fds_[worker_id], EPOLL_CTL_DEL, fd, NULL);
                } else {
                    printf("Unknown event\n");
                    close(fd);
                    epoll_ctl(epoll_fds_[worker_id], EPOLL_CTL_DEL, fd, NULL);
                }
            }
        }
    }

    void Server::start() {
        signal(SIGPIPE, SIG_IGN);
        printf("Server started\n");
        listen_socket_ = startListen();
        printf("Listening on port %d\n", port_);
        for (int i = 0; i < conf_.get_working_threads_num(); i++) {
            epoll_fds_.push_back(startEpoll());
        }
        printf("Epoll started\n");
        listen_thread_ = std::thread(&Server::handleListen, this);
        printf("Listening thread started\n");
        for (int i = 0; i < conf_.get_working_threads_num(); i++) {
            printf("Starting working thread %d\n", i);
            request_threads_.push_back(std::thread(&Server::handleRequest, this, i));
        }
        printf("All working threads started\n");
    }

    Connection::Connection() {
        fd_ = -1;
    }

    Connection::Connection(int fd) {
        fd_ = fd;
    }

    Connection::~Connection() {
        if (fd_ != -1) {
            close(fd_);
        }
    }


}