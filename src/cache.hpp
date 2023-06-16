#ifndef YY_CACHE
#define YY_CACHE

#include <string>
#include <unordered_map>

#include "http.hpp"

namespace yunying {
    class Cache {
    public:
        Cache();
        Cache(int max_size_bytes);
        ~Cache();

        HttpResponse get(const HttpRequest request);
    
    private:
        int max_size_bytes_;
        int size_bytes_;
        std::unordered_map<std::string, std::string> cache_;
        std::unordered_map<std::string, int> expires_;
    };
} // namespace yunying

#endif // YY_CACHE