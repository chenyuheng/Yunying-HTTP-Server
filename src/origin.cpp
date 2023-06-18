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
        return handleDefault(request.get_path());
    }

    HttpResponse* StaticFileOrigin::get(HttpRequest request, int* max_age) {
        std::string path = root_dir_ + handleDefault(request.get_path());
        HttpResponse* response = new HttpResponse();
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