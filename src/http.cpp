#include "http.hpp"

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    rtrim(s);
    ltrim(s);
}

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
        auto start_time = std::chrono::system_clock::now();
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
        // parse header
        std::string method_str = header_lines[0].substr(0, header_lines[0].find(" "));
        for (auto & c: method_str) c = toupper(c);
        if (MethodDict.find(method_str) == MethodDict.end()) {
            method_ = HttpMethod::OTHER;
        } else {
            method_ = MethodDict[method_str];
        }
        path_ = header_lines[0].substr(header_lines[0].find(" ") + 1, header_lines[0].rfind(" ") - header_lines[0].find(" ") - 1);
        std::string http_version_str = header_lines[0].substr(header_lines[0].rfind(" ") + 1);
        for (auto & c: http_version_str) c = toupper(c);
        if (HttpVersionDict.find(http_version_str) == HttpVersionDict.end()) {
            http_version_ = HttpVersion::OTHER;
        } else {
            http_version_ = HttpVersionDict[http_version_str];
        }
        for (int i = 1; i < header_lines.size(); i++) {
            std::string key = header_lines[i].substr(0, header_lines[i].find(":"));
            for (auto & c: key) c = toupper(c);
            std::string value = header_lines[i].substr(header_lines[i].find(":") + 1);
            trim(value);
            headers_[key] = value;
        }
        host_ = headers_["HOST"];
        body_ = body_raw;
        auto end_time = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end_time - start_time;
        Metrics::getInstance().count("parse_time", elapsed_seconds.count());
        Metrics::getInstance().count("parse_count", 1);
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
        set_header("HOST", host);
        host_ = host;
    }

    void HttpRequest::set_body(const std::string body) {
        body_ = body;
    }

    std::string HttpRequest::to_string() {
        std::string request_raw = "";
        request_raw += MethodString[method_] + " " + path_ + " " + HttpVersionString[http_version_] + "\r\n";
        for (auto it = headers_.begin(); it != headers_.end(); it++) {
            request_raw += it->first + ": " + it->second + "\r\n";
        }
        request_raw += "\r\n";
        request_raw += body_;
        return request_raw;
    }

    HttpResponse::HttpResponse() {
        status_ = HttpStatus::OK;
        body_ = "";
    }

    HttpResponse::HttpResponse(std::string response_raw) {
        auto start_time = std::chrono::system_clock::now();
        std::string header_raw = response_raw.substr(0, response_raw.find("\r\n\r\n"));
        std::string body_raw = response_raw.substr(response_raw.find("\r\n\r\n") + 4);
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
        // parse header
        std::string http_version_str = header_lines[0].substr(0, header_lines[0].find(" "));
        for (auto & c: http_version_str) c = toupper(c);
        if (HttpVersionDict.find(http_version_str) == HttpVersionDict.end()) {
            http_version_ = HttpVersion::OTHER;
        } else {
            http_version_ = HttpVersionDict[http_version_str];
        }
        std::string status_str = header_lines[0].substr(header_lines[0].find(" ") + 1, 3);
        int status_code = std::stoi(status_str);
        if (StatusString.find((HttpStatus)status_code) == StatusString.end()) {
            status_ = HttpStatus::OTHER;
        } else {
            status_ = (HttpStatus)status_code;
        }
        for (int i = 1; i < header_lines.size(); i++) {
            std::string key = header_lines[i].substr(0, header_lines[i].find(":"));
            for (auto & c: key) c = toupper(c);
            std::string value = header_lines[i].substr(header_lines[i].find(":") + 1);
            trim(value);
            headers_[key] = value;
        }
        body_ = body_raw;
        auto end_time = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end_time - start_time;
        Metrics::getInstance().count("parse_time", elapsed_seconds.count());
        Metrics::getInstance().count("parse_count", 1);
    }

    HttpResponse::~HttpResponse() {
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
        response_raw += std::to_string((int)status_) + " " + status_str;
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