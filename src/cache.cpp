#include "cache.hpp"

#include <string>
#include <unordered_map>

namespace yunying {
    Cache::Cache() {
        max_size_bytes_ = 0;
        size_bytes_ = 0;
        cache_ = std::unordered_map<std::string, std::string>();
        expires_ = std::unordered_map<std::string, int>();
    }

    Cache::Cache(int max_size_bytes) {
        max_size_bytes_ = max_size_bytes;
        size_bytes_ = 0;
        cache_ = std::unordered_map<std::string, std::string>();
        expires_ = std::unordered_map<std::string, int>();
    }

    Cache::~Cache() {
        // TODO
    }

    HttpResponse Cache::get(const HttpRequest request) {
        HttpResponse response = HttpResponse();
        response.set_body("Hello, world!\n");
        return response;
    }
}