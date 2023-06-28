#ifndef YY_CONF
#define YY_CONF

#include <cstdint>
#include <sol/sol.hpp>
#include <string>
#include <unordered_map>

namespace yunying {
enum class OriginType { StaticFile, Upstream };

class Conf {
 private:
  Conf(){};                     // private constructor
  Conf(Conf const&);            // prevent copies
  void operator=(Conf const&);  // prevent assignments

  std::string lua_config_path_ = "config.lua";
  sol::state lua_;
  template <class T>
  T luaGet(std::string key, T default_val);

  // hardcoded for now
  uint16_t port_ = 8080;
  uint16_t working_threads_num_ = 5;

  // cache
  size_t cache_size_bytes_ = 1 << 25;
  int cache_default_max_age_ = 300;
  int cache_clean_interval_ = 10;

  // origin
  OriginType origin_type_ = OriginType::StaticFile;
  // used for static file origin only
  std::string root_dir_ = ".";
  // used for upstream origin only
  std::string upstream_host_ = "upstream.host";
  std::string upstream_ip_ = "185.199.110.153";
  uint16_t upstream_port_ = 80;

 public:
  static Conf& getInstance() {
    static Conf instance;
    return instance;
  }
  void parseLua();
  void set_lua_config_path(std::string path) { lua_config_path_ = path; }
  sol::state& get_lua() { return lua_; }

  size_t get_cache_size_bytes() { return cache_size_bytes_; }
  uint16_t get_port() { return port_; }
  uint16_t get_working_threads_num() { return working_threads_num_; }
  OriginType get_origin_type() { return origin_type_; }
  std::string get_root_dir() { return root_dir_; }
  int get_cache_default_max_age() { return cache_default_max_age_; }
  std::string get_upstream_host() { return upstream_host_; }
  std::string get_upstream_ip() { return upstream_ip_; }
  uint16_t get_upstream_port() { return upstream_port_; }
  int get_cache_clean_interval() { return cache_clean_interval_; }
};
}  // namespace yunying

#endif  // YY_CONF