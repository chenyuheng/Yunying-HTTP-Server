#include "cache.hpp"

#include <string>
#include <unordered_map>

namespace yunying {
    bool ExpiresCompare::operator()(std::pair<int, HttpResponse*> below, 
                    std::pair<int, HttpResponse*> above) {
        return below.first > above.first;
    }

    Cache::Cache(Origin* origin, int max_size_bytes) {
        max_size_bytes_ = max_size_bytes;
        size_bytes_ = 0;
        cache_ = std::unordered_map<std::string, HttpResponse*>();
        expires_ = std::priority_queue<std::pair<int, HttpResponse*>,
            std::vector<std::pair<int, HttpResponse*>>, ExpiresCompare>();
        origin_ = origin;
    }

    Cache::~Cache() {
        // TODO
    }

    HttpResponse* Cache::get(const HttpRequest request) {
        clean();
        std::string key = origin_->getKey(request);
        if (cache_.find(key) != cache_.end()) {
            HttpResponse* response = cache_[key];
            return response;
        }
        int max_age;
        HttpResponse* response = origin_->get(request, &max_age);
        if (response->get_body().size() > max_size_bytes_) {
            return response;
        }
        int timestamp = time(NULL);
        expires_.push(std::make_pair(timestamp + max_age, response));
        cache_[key] = response;
        reverse_cache_[response] = key;
        size_bytes_ += response->get_body().length();
        return response;
    }

    void Cache::clean() {
        int timestamp = time(NULL);
        while (!expires_.empty() && expires_.top().first < timestamp) {
            HttpResponse* response = expires_.top().second;
            expires_.pop();
            std::string key = reverse_cache_[response];
            cache_.erase(key);
            reverse_cache_.erase(response);
            size_bytes_ -= response->get_body().length();
            delete response;
        }
        while (size_bytes_ > max_size_bytes_) {
            HttpResponse* response = expires_.top().second;
            expires_.pop();
            std::string key = reverse_cache_[response];
            cache_.erase(key);
            reverse_cache_.erase(response);
            size_bytes_ -= response->get_body().length();
            delete response;
        }
    }
}