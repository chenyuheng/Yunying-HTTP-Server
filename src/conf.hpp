#ifndef YY_CONF
#define YY_CONF

#include <cstdint>

namespace yunying {
    class Conf
    {
    private:
        // hardcoded for now
        int cache_size_bytes = 1 << 25;
        uint16_t port = 8080;

    public:
        Conf();
        ~Conf();

        int get_cache_size_bytes() { return cache_size_bytes; }
    };
} // namespace yunying

#endif // YY_CONF