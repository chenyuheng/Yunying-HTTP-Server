#ifndef YY_HTTP
#define YY_HTTP

#include <string>
#include <unordered_map>

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

    class HttpRequest {
    public:
        HttpRequest();
        HttpRequest(int fd);
        ~HttpRequest();
        std::unordered_map<std::string, std::string> get_headers();
        HttpMethod get_method();
        std::string get_path();
        std::string get_host();
        void set_header(const std::string key, const std::string value);
        void set_method(HttpMethod method);
        void set_path(const std::string path);
        void set_host(const std::string host);
    };

    class HttpResponse {
    public:
        HttpResponse();
        HttpResponse(int fd);
        ~HttpResponse();
        std::unordered_map<std::string, std::string> get_headers();
        HttpStatus get_status();
        std::string get_body();
        void set_header(const std::string key, const std::string value);
        void set_status(HttpStatus status);
        void set_body(const std::string body);
    };
} // namespace yunying



#endif // YY_HTTP