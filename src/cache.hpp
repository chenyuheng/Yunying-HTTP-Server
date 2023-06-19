#ifndef YY_CACHE
#define YY_CACHE

#include <queue>
#include <string>
#include <unordered_map>

#include "http.hpp"
#include "origin.hpp"

namespace yunying {
    class ExpiresCompare {
    public:
        bool operator()(std::pair<int, HttpResponse*> below,
                        std::pair<int, HttpResponse*> above);
    };

    class Cache {
    public:
        Cache() = delete;
        Cache(Origin* origin, int max_size_bytes);
        ~Cache();

        HttpResponse* get(const HttpRequest request);
    
    private:
        int max_size_bytes_;
        int size_bytes_;
        Origin* origin_;
        std::unordered_map<std::string, HttpResponse*> cache_;
        std::unordered_map<HttpResponse*, std::string> reverse_cache_;
        std::priority_queue<
            std::pair<int, HttpResponse*>,
            std::vector<std::pair<int, HttpResponse*>>,
            ExpiresCompare> expires_;

        void clean();
    };
} // namespace yunying

#endif // YY_CACHE