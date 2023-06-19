#include "origin.hpp"

namespace yunying {

    StaticFileOrigin::StaticFileOrigin() {
        root_dir_ = "/tmp";
    }

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
}