#include "http.hpp"

#include <iostream>
#include <unistd.h>

namespace yunying {
    HttpRequest::HttpRequest() {
        method_ = HttpMethod::GET;
        path_ = "/";
        host_ = "";
    }

    HttpRequest::HttpRequest(int fd) {
        char buffer[1<<15];
        int bytes_read = read(fd, buffer, 1<<15);
        // std::string request_string = std::string(buffer, bytes_read);
        // std::cout << "got request length: " << bytes_read << std::endl;
        // std::cout << request_string << std::endl;
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
        // TODO
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
        headers_["Content-Length"] = std::to_string(body_.length());
        std::string response = "HTTP/1.1 ";
        response += std::to_string(static_cast<int>(status_));
        response += " ";
        response += StatusString[status_];
        response += "\r\n";
        for (auto header : headers_) {
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