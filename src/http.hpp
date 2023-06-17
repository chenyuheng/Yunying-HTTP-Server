#ifndef YY_HTTP
#define YY_HTTP

#include <string>
#include <unordered_map>
#include <iostream>
#include <unistd.h>

namespace yunying {
    enum class HttpMethod {
        GET,
        HEAD,
        OPTIONS
    };

    enum class HttpStatus {
        OK = 200,
        PARTIAL_CONTENT = 206,
        NOT_FOUND = 404,
        NOT_IMPLEMENTED = 501
    };

    static std::unordered_map<HttpStatus, std::string> StatusString = {
        {HttpStatus::OK, "OK"},
        {HttpStatus::PARTIAL_CONTENT, "Partial Content"},
        {HttpStatus::NOT_FOUND, "Not Found"},
        {HttpStatus::NOT_IMPLEMENTED, "Not Implemented"}
    };

    class HttpRequest {
    private:
        std::unordered_map<std::string, std::string> headers_;
        HttpMethod method_;
        std::string path_;
        std::string host_;
        bool failed_;
    public:
        HttpRequest();
        HttpRequest(int fd);
        ~HttpRequest();
        std::unordered_map<std::string, std::string> get_headers() { return headers_;}
        HttpMethod get_method() { return method_; }
        std::string get_path() { return path_; }
        std::string get_host() { return host_; }
        bool failed() { return failed_; }
        void set_header(const std::string key, const std::string value);
        void set_method(HttpMethod method);
        void set_path(const std::string path);
        void set_host(const std::string host);
        void send(int fd);
    };

    class HttpResponse {
    private:
        std::unordered_map<std::string, std::string> headers_;
        HttpStatus status_;
        std::string body_;
    public:
        HttpResponse();
        HttpResponse(int fd);
        ~HttpResponse();
        std::unordered_map<std::string, std::string> get_headers() { return headers_; }
        HttpStatus get_status() { return status_; }
        std::string get_body() { return body_; }
        void set_header(const std::string key, const std::string value);
        void set_status(HttpStatus status);
        void set_body(const std::string body);
        void send(int fd);
    };
} // namespace yunying



#endif // YY_HTTP