#include "cache.hpp"

namespace yunying {
bool ExpiresCompare::operator()(std::pair<int, HttpResponse*> below,
                                std::pair<int, HttpResponse*> above) {
  return below.first > above.first;
}

Cache::Cache(Origin* origin) {
  max_size_bytes_ = Conf::getInstance().get_cache_size_bytes();
  clean_interval_ = Conf::getInstance().get_cache_clean_interval();
  size_bytes_ = 0;
  cache_ = std::unordered_map<std::string, HttpResponse*>();
  expires_ = std::priority_queue<std::pair<int, HttpResponse*>,
                                 std::vector<std::pair<int, HttpResponse*>>,
                                 ExpiresCompare>();
  origin_ = origin;
  clean_thread_ = new std::thread(&Cache::cleanThread, this);
}

Cache::~Cache() {
  // TODO
}

HttpResponse* Cache::safeCacheGet(std::string key) {
  const std::lock_guard<std::mutex> lock(read_mutex_);
  if (cache_.find(key) != cache_.end()) {
    return cache_[key];
  }
  return nullptr;
}

HttpResponse* Cache::get(const HttpRequest request) {
  // clean();
  std::string key = origin_->getKey(request);
  while (true) {
    while (originating_keys_.find(key) != originating_keys_.end()) {
      // wait
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    HttpResponse* response = safeCacheGet(key);
    if (response != nullptr) {
      return response;
    }
    originating_mutex_.lock();
    if (originating_keys_.find(key) == originating_keys_.end()) {
      originating_keys_.insert(key);
      originating_mutex_.unlock();
      break;
    }
    originating_mutex_.unlock();
  }
  int max_age;
  HttpResponse* response = origin_->get(request, &max_age);
  if (response->get_body().size() > max_size_bytes_) {
    originating_keys_.erase(key);
    return response;
  }
  int timestamp = time(NULL);
  expires_.push(std::make_pair(timestamp + max_age, response));
  cache_[key] = response;
  reverse_cache_[response] = key;
  size_bytes_ += response->get_size();
  originating_keys_.erase(key);
  return response;
}

void Cache::cleanThread() {
  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(clean_interval_));
    while (to_delete_.size() > 0) {
      HttpResponse* response = to_delete_.back();
      to_delete_.pop_back();
      delete response;
    }
    int timestamp = time(NULL);
    while (!expires_.empty() && expires_.top().first < timestamp) {
      HttpResponse* response = expires_.top().second;
      expires_.pop();
      std::string key = reverse_cache_[response];
      cache_.erase(key);
      reverse_cache_.erase(response);
      size_bytes_ -= response->get_size();
      to_delete_.push_back(response);
    }
    while (size_bytes_ > max_size_bytes_) {
      HttpResponse* response = expires_.top().second;
      expires_.pop();
      std::string key = reverse_cache_[response];
      cache_.erase(key);
      reverse_cache_.erase(response);
      size_bytes_ -= response->get_size();
      to_delete_.push_back(response);
    }
  }
}
}  // namespace yunying