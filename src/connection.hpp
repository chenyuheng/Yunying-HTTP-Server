#ifndef YY_CONNECTION
#define YY_CONNECTION

#include "http.hpp"
#include "metrics.hpp"

#include <sys/socket.h>

namespace yunying {
    class Connection {
    private:
        int fd_;
        std::string received_raw_;
        std::string received_buffer_;
        std::string send_raw_;
        size_t send_offset_;
        bool recv_done_;
        bool send_started_;
        bool send_done_;
        bool fd_closed_;
        HttpRequest* request_;
        HttpResponse* response_;
    public:
        Connection(int fd);
        ~Connection();

        void setResponse(HttpResponse* response) { response_ = response; }
        void setRequest(HttpRequest* request) { request_ = request; }
        void setSendRaw(std::string send_raw) { send_raw_ = send_raw; send_started_ = true; send_offset_ = 0; }

        HttpResponse* getResponse() { return response_; }
        HttpRequest* getRequest() { return request_; }
        int getFd() { return fd_; }
        std::string getReceivedRaw() { return received_raw_; }
        bool getRecvDone() { return recv_done_; }
        bool getSendStarted() { return send_started_; }
        bool getSendDone() { return send_done_; }
        bool getFdClosed() { return fd_closed_; }

        void recv();
        void send();
    };
} // namespace yunying

#endif // YY_CONNECTION