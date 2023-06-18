#ifndef YY_CONF
#define YY_CONF

#include <cstdint>
#include <string>

namespace yunying {
    class Conf
    {
    private:
        // hardcoded for now
        int cache_size_bytes_ = 1 << 25;
        uint16_t port_ = 8080;
        uint16_t working_threads_num_ = 5;
        std::string root_dir_ = "/tmp";

    public:
        Conf();
        ~Conf();

        int get_cache_size_bytes() { return cache_size_bytes_; }
        uint16_t get_port() { return port_; }
        void set_port(uint16_t port) { port_ = port; }
        uint16_t get_working_threads_num() { return working_threads_num_; }
        std::string get_root_dir() { return root_dir_; }

    };
} // namespace yunying

#endif // YY_CONF