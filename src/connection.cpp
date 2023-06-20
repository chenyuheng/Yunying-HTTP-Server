    
#include "connection.hpp" 

namespace yunying {
    Connection::Connection(int fd) {
        fd_ = fd;
        send_offset_ = 0;
        recv_done_ = false;
        send_started_ = false;
        send_done_ = false;
        fd_closed_ = false;
    }

    Connection::~Connection() {
        if (fd_ != -1) {
            close(fd_);
        }
    }

    void Connection::recv() {
        auto start_time = std::chrono::system_clock::now();
        char buf[4096];
        if (recv_done_) {
            recv_done_ = false;
            received_raw_ = "";
        }
        int n = read(fd_, buf, 4096);
        if (n <= 0) {
            recv_done_ = true;
            fd_closed_ = true;
            return;
        }
        received_raw_ += std::string(buf, n);
        if (received_raw_.find("\r\n\r\n") != std::string::npos) {
            std::string header = received_raw_.substr(0, received_raw_.find("\r\n\r\n"));
            // convert header to uppercase
            for (int i = 0; i < header.size(); i++) {
                if (header[i] >= 'a' && header[i] <= 'z') {
                    header[i] -= 32;
                }
            }
            int content_length = 0;
            if (header.find("CONTENT-LENGTH:") != std::string::npos) {
                std::string content_length_str = header.substr(header.find("CONTENT-LENGTH:") + 15);
                content_length_str = content_length_str.substr(0, content_length_str.find("\r\n")); 
                content_length = std::stoi(content_length_str);
            }
            if (received_raw_.size() >= header.size() + 4 + content_length) {
                recv_done_ = true;
            }
        }
        auto end_time = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end_time - start_time;
        Metrics::getInstance().count("recv_time", elapsed_seconds.count());
        Metrics::getInstance().count("recv_count", 1);
    }

    void Connection::send() {
        if (send_done_) {
            send_done_ = false;
            send_offset_ = 0;
        }
        int n = ::send(fd_, send_raw_.c_str() + send_offset_, send_raw_.size() - send_offset_, MSG_NOSIGNAL);
        if (n <= 0) {
            send_done_ = true;
            fd_closed_ = true;
            return;
        }
        send_offset_ += n;
        if (send_offset_ == send_raw_.size()) {
            send_done_ = true;
            send_started_ = false;
        }
    }
}
