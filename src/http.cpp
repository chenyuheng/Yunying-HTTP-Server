#include "http.hpp"

namespace yunying {
    HttpRequest::HttpRequest() {
        method_ = HttpMethod::GET;
        path_ = "/";
        http_version_ = HttpVersion::HTTP_1_1;
        host_ = "";
        body_ = "";
        failed_ = false;
    }

    HttpRequest::HttpRequest(std::string request_raw) {
        failed_ = false;
        std::string header_raw = request_raw.substr(0, request_raw.find("\r\n\r\n"));
        std::string body_raw = request_raw.substr(request_raw.find("\r\n\r\n") + 4);
        // split header to lines
        std::vector<std::string> header_lines;
        std::string header_line = "";
        for (int i = 0; i < header_raw.size(); i++) {
            if (header_raw[i] == '\r' && header_raw[i + 1] == '\n') {
                header_lines.push_back(header_line);
                header_line = "";
                i++;
            } else {
                header_line += header_raw[i];
            }
        }
        for (auto & c: header_lines[0]) c = toupper(c);
        // parse header
        std::string method_str = header_lines[0].substr(0, header_lines[0].find(" "));
        if (MethodDict.find(method_str) == MethodDict.end()) {
            method_ = HttpMethod::OTHER;
        } else {
            method_ = MethodDict[method_str];
        }
        path_ = header_lines[0].substr(header_lines[0].find(" ") + 1, header_lines[0].rfind(" ") - header_lines[0].find(" ") - 1);
        std::string http_version_str = header_lines[0].substr(header_lines[0].rfind(" ") + 1);
        if (HttpVersionDict.find(http_version_str) == HttpVersionDict.end()) {
            http_version_ = HttpVersion::OTHER;
        } else {
            http_version_ = HttpVersionDict[http_version_str];
        }
        for (int i = 1; i < header_lines.size(); i++) {
            std::string key = header_lines[i].substr(0, header_lines[i].find(": "));
            for (auto & c: key) c = toupper(c);
            std::string value = header_lines[i].substr(header_lines[i].find(": ") + 2);
            headers_[key] = value;
        }
        host_ = headers_["HOST"];
        body_ = body_raw;
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

    void HttpRequest::set_body(const std::string body) {
        body_ = body;
    }

    HttpResponse::HttpResponse() {
        status_ = HttpStatus::OK;
        body_ = "";
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

    std::string HttpResponse::to_string() {
        std::string response_raw = "";
        std::string status_str = "";
        if (StatusString.find(status_) != StatusString.end()) {
            status_str = StatusString[status_];
        }
        response_raw += "HTTP/1.1 ";
        response_raw += std::to_string((int)status_) + " " + StatusString[status_];
        response_raw += "\r\n";
        headers_["Content-Length"] = std::to_string(body_.size());
        for (auto it = headers_.begin(); it != headers_.end(); it++) {
            response_raw += it->first + ": " + it->second + "\r\n";
        }
        response_raw += "\r\n";
        response_raw += body_;
        return response_raw;
    }
}