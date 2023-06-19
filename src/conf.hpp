#ifndef YY_CONF
#define YY_CONF

#include <cstdint>
#include <string>

namespace yunying {
    class Conf
    {
    private:
        Conf() {} // private constructor
        Conf(Conf const&); // prevent copies
        void operator=(Conf const&); // prevent assignments

        // hardcoded for now
        int cache_size_bytes_ = 1 << 25;
        uint16_t port_ = 8080;
        uint16_t working_threads_num_ = 5;
        std::string root_dir_ = "/tmp";
        int default_max_age_ = 300;

    public:
        static Conf& getInstance() {
            static Conf instance;
            return instance;
        }

        int get_cache_size_bytes() { return cache_size_bytes_; }
        uint16_t get_port() { return port_; }
        uint16_t get_working_threads_num() { return working_threads_num_; }
        std::string get_root_dir() { return root_dir_; }
        int get_default_max_age() { return default_max_age_; }

        void set_port(uint16_t port) { port_ = port; }
        void set_root_dir(std::string root_dir) { root_dir_ = root_dir; }

    };
} // namespace yunying

#endif // YY_CONF