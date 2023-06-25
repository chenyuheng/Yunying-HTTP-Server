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
        if (request_raw.size() == 0) {
            failed_ = true;
            Metrics::getInstance().count("failed_parse_count", 1);
            return;
        }
        auto start_time = std::chrono::system_clock::now();
        failed_ = false;
        size_t CRLF_pos = request_raw.find("\r\n\r\n");
        if (CRLF_pos == std::string::npos) {
            failed_ = true;
            Metrics::getInstance().count("failed_parse_count", 1);
            return;
        }
        std::string header_raw = request_raw.substr(0, CRLF_pos);
        std::string body_raw = request_raw.substr(CRLF_pos + 4);
        // split header to lines
        std::vector<std::string> header_lines;
        std::string header_line = "";
        for (int i = 0; i < header_raw.size(); i++) {
            if (header_raw[i] == '\r' && header_raw[i + 1] == '\n') {
                trim(header_line);
                header_lines.push_back(header_line);
                header_line = "";
                i++;
            } else {
                header_line += header_raw[i];
            }
        }
        header_lines.push_back(header_line);
        // parse request line
        std::vector<std::string> request_line = split(header_lines[0], " ");
        if (request_line.size() != 3) {
            failed_ = true;
            Metrics::getInstance().count("failed_parse_count", 1);
            return;
        }
        std::string method_str = request_line[0];
        if (MethodDict.find(method_str) == MethodDict.end()) {
            method_ = HttpMethod::OTHER;
            failed_ = true;
            Metrics::getInstance().count("failed_parse_count", 1);
            return;
        }
        method_ = MethodDict[method_str];
        path_ = request_line[1];
        std::string http_version_str = request_line[2];
        for (auto & c: http_version_str) c = toupper(c);
        if (HttpVersionDict.find(http_version_str) == HttpVersionDict.end()) {
            http_version_ = HttpVersion::OTHER;
        } else {
            http_version_ = HttpVersionDict[http_version_str];
        }

        // parse headers
        for (int i = 1; i < header_lines.size(); i++) {
            std::vector<std::string> header_line = split(header_lines[i], ":", 2);
            std::string key = header_line[0];
            for (auto & c: key) c = toupper(c);
            std::string value = header_line[1];
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
        if (failed_) {
            return "failed request";
        }
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
        size_t CRLF_pos = response_raw.find("\r\n\r\n");
        if (CRLF_pos == std::string::npos) {
            failed_ = true;
            Metrics::getInstance().count("failed_parse_count", 1);
            return;
        }
        std::string header_raw = response_raw.substr(0, CRLF_pos);
        std::string body_raw = response_raw.substr(CRLF_pos + 4);
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
        header_lines.push_back(header_line);
        // parse header
        std::vector<std::string> status_line = split(header_lines[0], " ", 3);
        if (status_line.size() < 3) {
            failed_ = true;
            Metrics::getInstance().count("failed_parse_count", 1);
            return;
        }
        std::string http_version_str = status_line[0];
        for (auto & c: http_version_str) c = toupper(c);
        if (HttpVersionDict.find(http_version_str) == HttpVersionDict.end()) {
            http_version_ = HttpVersion::OTHER;
        } else {
            http_version_ = HttpVersionDict[http_version_str];
        }
        std::string status_str = status_line[1];
        int status_code = std::stoi(status_str);
        if (StatusString.find((HttpStatus)status_code) == StatusString.end()) {
            status_ = HttpStatus::OTHER;
        } else {
            status_ = (HttpStatus)status_code;
        }
        for (int i = 1; i < header_lines.size(); i++) {
            std::string key = header_lines[i].substr(0, header_lines[i].find(":"));
            for (auto & c: key) c = toupper(c);
            if (key == "TRANSFER-ENCODING") continue;
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
        printf("body: %s\n", body.c_str());
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
        //if (body_.size() > 0) {
            headers_["CONTENT-LENGTH"] = std::to_string(body_.size());
        //}
        for (auto it = headers_.begin(); it != headers_.end(); it++) {
            response_raw += it->first + ": " + it->second + "\r\n";
        }
        response_raw += "\r\n";
        response_raw += body_;
        return response_raw;
    }
}