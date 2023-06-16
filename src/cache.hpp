#ifndef YY_CACHE
#define YY_CACHE

#include <string>
#include <unordered_map>

#include "http.hpp"

namespace yunying {
    class Cache {
    public:
        Cache(int max_size_bytes);
        ~Cache();

        std::string get(const HttpRequest request);
    
    private:
        std::unordered_map<std::string, std::string> cache;
        std::unordered_map<std::string, int> expires;
    };
} // namespace yunying

#endif // YY_CACHE