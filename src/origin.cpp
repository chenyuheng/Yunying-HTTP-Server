#include "origin.hpp"

namespace yunying {

    StaticFileOrigin::StaticFileOrigin(std::string root_dir) {
        root_dir_ = root_dir;
    }

    StaticFileOrigin::~StaticFileOrigin() {
        // TODO
    }

    std::string StaticFileOrigin::handleDefault(std::string path) {
        if (path[path.length() - 1] == '/') {
            path += "index.html";
        }
        return path;
    }

    std::string StaticFileOrigin::getKey(HttpRequest request) {
        return MethodString[request.get_method()] + handleDefault(request.get_path());
    }

    HttpResponse* StaticFileOrigin::get(HttpRequest request, int* max_age) {
        HttpResponse* response = new HttpResponse();
        *max_age = Conf::getInstance().get_default_max_age();
        if (request.get_method() != HttpMethod::GET) {
            response->set_status(HttpStatus::METHOD_NOT_ALLOWED);
            return response;
        }
        std::string path = root_dir_ + handleDefault(request.get_path());
        FILE* file = fopen(path.c_str(), "rb");
        if (file == NULL) {
            response->set_status(HttpStatus::NOT_FOUND);
        } else {
            fseek(file, 0, SEEK_END);
            int file_size = ftell(file);
            fseek(file, 0, SEEK_SET);
            char* file_content = (char*)malloc(file_size);
            fread(file_content, 1, file_size, file);
            fclose(file);
            response->set_body(std::string(file_content, file_size));
            free(file_content);
            response->set_status(HttpStatus::OK);
        }
        return response;
    }

    UpstreamOrigin::UpstreamOrigin() {
    }

    UpstreamOrigin::~UpstreamOrigin() {
        // TODO
    }

    std::string UpstreamOrigin::getKey(HttpRequest request) {
        return request.get_host() + MethodString[request.get_method()] + request.get_path();
    }

    std::vector<std::string> split(const std::string str, const std::string delimiter) {
        std::vector<std::string> result;
        std::string s = str;
        while (s.find(delimiter) != std::string::npos) {
            result.push_back(s.substr(0, s.find(delimiter)));
            s = s.substr(s.find(delimiter) + delimiter.length());
        }
        result.push_back(s);
        return result;
    }

    HttpResponse* UpstreamOrigin::get(HttpRequest request, int* max_age) {
        HttpRequest upstream_request;
        upstream_request.set_method(request.get_method());
        upstream_request.set_path(request.get_path());
        upstream_request.set_host(Conf::getInstance().get_upstream_host());
        upstream_request.set_body(request.get_body());

        std::string upstream_ip = Conf::getInstance().get_upstream_ip();
        int upstream_port = Conf::getInstance().get_upstream_port();
        int upstream_fd = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in upstream_addr;
        upstream_addr.sin_family = AF_INET;
        upstream_addr.sin_port = htons(upstream_port);
        upstream_addr.sin_addr.s_addr = inet_addr(upstream_ip.c_str());
        connect(upstream_fd, (struct sockaddr*)&upstream_addr, sizeof(upstream_addr));

        Connection upstream_connection(upstream_fd);
        upstream_connection.setSendRaw(upstream_request.to_string());
        while (!upstream_connection.getSendDone()) {
            upstream_connection.send();
        }
        while (!upstream_connection.getRecvDone()) {
            upstream_connection.recv();
        }
        HttpResponse* upstream_response = new HttpResponse(upstream_connection.getReceivedRaw());
        if (upstream_response->get_header("Cache-Control") != "") {
            std::vector<std::string> cache_controls = split(upstream_response->get_header("Cache-Control"), ",");
            for (int i = 0; i < cache_controls.size(); i++) {
                std::vector<std::string> cache_control = split(cache_controls[i], "=");
                if (cache_control[0] == "max-age") {
                    *max_age = std::stoi(cache_control[1]);
                    break;
                }
            }
        }
        if (*max_age < Conf::getInstance().get_default_max_age()) {
            *max_age = Conf::getInstance().get_default_max_age();
        }
        Metrics::getInstance().count("upstream_count", 1);
        return upstream_response;
    }
}