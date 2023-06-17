#include "http.hpp"

namespace yunying {
    HttpRequest::HttpRequest() {
        method_ = HttpMethod::GET;
        path_ = "/";
        host_ = "";
        failed_ = false;
    }

    HttpRequest::HttpRequest(int fd) {
        failed_ = false;
        char buffer[1<<15];
        int bytes_read = read(fd, buffer, 1<<15);
        if (bytes_read == 0) {
            failed_ = true;
            close(fd);
        }
    }

    HttpRequest::~HttpRequest() {
        // TODO
    }

    void HttpRequest::set_header(const std::string key, const std::string value) {
        headers_[key] = value;
    }

    void HttpRequest::set_method(HttpMethod method) {
        method_ = method;
    }

    void HttpRequest::set_path(const std::string path) {
        path_ = path;
    }

    void HttpRequest::set_host(const std::string host) {
        host_ = host;
    }

    void HttpRequest::send(int fd) {
        // TODO
    }

    HttpResponse::HttpResponse() {
        status_ = HttpStatus::OK;
        body_ = "";
    }

    HttpResponse::HttpResponse(int fd) {
        // TODO
    }

    HttpResponse::~HttpResponse() {
        printf("HttpResponse destructor\n");
    }

    void HttpResponse::set_header(const std::string key, const std::string value) {
        headers_[key] = value;
    }

    void HttpResponse::set_status(HttpStatus status) {
        status_ = status;
    }

    void HttpResponse::set_body(const std::string body) {
        body_ = body;
    }

    void HttpResponse::send(int fd) {
        body_ = "Hello, world!\n";
        HttpStatus status = HttpStatus::OK;
        std::unordered_map<std::string, std::string> headers;
        headers["Content-Length"] = std::to_string(body_.length());
        std::string response = "HTTP/1.1 ";
        response += std::to_string(static_cast<int>(status));
        response += " ";
        response += StatusString[status];
        response += "\r\n";
        for (auto header : headers) {
            response += header.first;
            response += ": ";
            response += header.second;
            response += "\r\n";
        }
        response += "\r\n";
        response += body_;
        write(fd, response.c_str(), response.length());
    }
}