#ifndef YY_HTTP
#define YY_HTTP

#include "metrics.hpp"
#include "util.hpp"

#include <algorithm>
#include <chrono>
#include <ctime>  
#include <mutex>
#include <string>
#include <unordered_map>
#include <iostream>
#include <unistd.h>
#include <vector>

namespace yunying {
    enum class HttpMethod {
        GET,
        POST,
        HEAD,
        PUT,
        DELETE,
        CONNECT,
        OPTIONS,
        TRACE,
        PATCH,
        PURGE,
        OTHER
    };

    enum class HttpStatus {
        OTHER = -1,
        OK = 200,
        PARTIAL_CONTENT = 206,
        MULTIPLE_CHOICES = 300,
        MOVED_PERMANENTLY = 301,
        FOUND = 302,
        NOT_MODIFIED = 304,        
        BAD_REQUEST = 400,
        NOT_FOUND = 404,
        METHOD_NOT_ALLOWED = 405,
        INTERNAL_SERVER_ERROR = 500,
        NOT_IMPLEMENTED = 501,
        HTTP_VERSION_NOT_SUPPORTED = 505
    };

    enum class HttpVersion {
        HTTP_1_0,
        HTTP_1_1,
        HTTP_2_0,
        HTTP_3_0,
        OTHER
    };

    static std::unordered_map<HttpMethod, std::string> MethodString = {
        {HttpMethod::GET, "GET"},
        {HttpMethod::POST, "POST"},
        {HttpMethod::HEAD, "HEAD"},
        {HttpMethod::PUT, "PUT"},
        {HttpMethod::DELETE, "DELETE"},
        {HttpMethod::CONNECT, "CONNECT"},
        {HttpMethod::OPTIONS, "OPTIONS"},
        {HttpMethod::TRACE, "TRACE"},
        {HttpMethod::PATCH, "PATCH"},
        {HttpMethod::PURGE, "PURGE"}
    };

    static std::unordered_map<std::string, HttpMethod> MethodDict = {
        {"GET", HttpMethod::GET},
        {"POST", HttpMethod::POST},
        {"HEAD", HttpMethod::HEAD},
        {"PUT", HttpMethod::PUT},
        {"DELETE", HttpMethod::DELETE},
        {"CONNECT", HttpMethod::CONNECT},
        {"OPTIONS", HttpMethod::OPTIONS},
        {"TRACE", HttpMethod::TRACE},
        {"PATCH", HttpMethod::PATCH},
        {"PURGE", HttpMethod::PURGE}
    };

    static std::unordered_map<HttpStatus, std::string> StatusString = {
        {HttpStatus::OK, "OK"},
        {HttpStatus::PARTIAL_CONTENT, "Partial Content"},
        {HttpStatus::MULTIPLE_CHOICES, "Multiple Choices"},
        {HttpStatus::MOVED_PERMANENTLY, "Moved Permanently"},
        {HttpStatus::FOUND, "Found"},
        {HttpStatus::NOT_MODIFIED, "Not Modified"},
        {HttpStatus::BAD_REQUEST, "Bad Request"},
        {HttpStatus::NOT_FOUND, "Not Found"},
        {HttpStatus::METHOD_NOT_ALLOWED, "Method Not Allowed"},
        {HttpStatus::INTERNAL_SERVER_ERROR, "Internal Server Error"},
        {HttpStatus::NOT_IMPLEMENTED, "Not Implemented"},
        {HttpStatus::HTTP_VERSION_NOT_SUPPORTED, "HTTP Version Not Supported"}
    };

    static std::unordered_map<HttpVersion, std::string> HttpVersionString = {
        {HttpVersion::HTTP_1_0, "HTTP/1.0"},
        {HttpVersion::HTTP_1_1, "HTTP/1.1"},
        {HttpVersion::HTTP_2_0, "HTTP/2.0"},
        {HttpVersion::HTTP_3_0, "HTTP/3.0"}
    };

    static std::unordered_map<std::string, HttpVersion> HttpVersionDict = {
        {"HTTP/1.0", HttpVersion::HTTP_1_0},
        {"HTTP/1.1", HttpVersion::HTTP_1_1},
        {"HTTP/2.0", HttpVersion::HTTP_2_0},
        {"HTTP/3.0", HttpVersion::HTTP_3_0}
    };

    class HttpRequest {
    private:
        std::unordered_map<std::string, std::string> headers_;
        HttpMethod method_;
        std::string path_;
        HttpVersion http_version_;
        std::string host_;
        std::string body_;
        bool failed_;
    public:
        HttpRequest();
        HttpRequest(std::string request_raw);
        ~HttpRequest();
        std::string get_header(std::string key) {
            std::string upper_key = key;
            for (auto &c : upper_key) c = toupper(c);
            if (headers_.find(upper_key) == headers_.end()) {
                return "";
            }
            return headers_[upper_key];
        }
        HttpMethod get_method() { return method_; }
        std::string get_path() { return path_; }
        HttpVersion get_http_version() { return http_version_; }
        std::string get_host() { return host_; }
        std::string get_body() { return body_; }
        bool failed() { return failed_; }
        void set_header(const std::string key, const std::string value);
        void set_method(HttpMethod method);
        void set_path(const std::string path);
        void set_host(const std::string host);
        void set_body(const std::string body);

        std::string to_string();
    };

    class HttpResponse {
    private:
        std::unordered_map<std::string, std::string> headers_;
        HttpVersion http_version_;
        HttpStatus status_;
        std::string body_;
        bool failed_;
    public:
        HttpResponse();
        HttpResponse(std::string response_raw);
        ~HttpResponse();
        std::string get_header(std::string key) {
            std::string upper_key = key;
            for (auto &c : upper_key) c = toupper(c);
            if (headers_.find(upper_key) == headers_.end()) {
                return "";
            }
            return headers_[upper_key];
        }
        HttpVersion get_http_version() { return http_version_; }
        HttpStatus get_status() { return status_; }
        std::string get_body() { return body_; }
        bool failed() { return failed_; }
        void set_header(const std::string key, const std::string value);
        void set_status(HttpStatus status);
        void set_body(const std::string body);

        std::string to_string();
    };
} // namespace yunying

#endif // YY_HTTP