#include "conf.hpp"

namespace yunying {
void Conf::parseLua() {
  lua_.open_libraries(sol::lib::base, sol::lib::string, sol::lib::table,
                      sol::lib::math,
                      // package and os modules could result vulnerability
                      // avoid using them if config file is not trusted
                      sol::lib::package, sol::lib::os);
  printf("Loading config %s\n", lua_config_path_.c_str());
  lua_.script_file(lua_config_path_);
  port_ = luaGet<uint16_t>("port", port_);
  working_threads_num_ =
      luaGet<uint16_t>("working_thread_num", working_threads_num_);
  cache_size_bytes_ = luaGet<int>("cache_size_bytes", cache_size_bytes_);
  cache_default_max_age_ =
      luaGet<int>("cache_default_max_age", cache_default_max_age_);
  cache_clean_interval_ =
      luaGet<int>("cache_clean_interval", cache_clean_interval_);
  std::string origin_type_str =
      luaGet<std::string>("origin_type", "static_file");
  if (origin_type_str == "static_file") {
    origin_type_ = OriginType::StaticFile;
  } else if (origin_type_str == "upstream") {
    origin_type_ = OriginType::Upstream;
  } else {
    printf("Unknown origin type: %s\n", origin_type_str.c_str());
    exit(1);
  }
  root_dir_ = luaGet<std::string>("root_dir", root_dir_);
  upstream_host_ = luaGet<std::string>("upstream_host", upstream_host_);
  upstream_domain_ = luaGet<std::string>("upstream_domain", upstream_domain_);
  upstream_port_ = luaGet<uint16_t>("upstream_port", upstream_port_);
  upstream_protocol_ =
      luaGet<std::string>("upstream_protocol", upstream_protocol_);
  if (upstream_protocol_ != "http" && upstream_protocol_ != "https") {
    printf("Unknown upstream protocol: %s\n", upstream_protocol_.c_str());
    exit(1);
  }
  printf("Finished loading config\n\n");
}

template <class T>
T Conf::luaGet(std::string key, T default_val) {
  sol::optional<T> opt = lua_[key];
  if (opt) {
    std::cout << "  " << key << ": " << (T)lua_[key] << "\n";
    return (T)lua_[key];
  }
  std::cout << key << ": " << default_val << " (using default)\n";
  return default_val;
}
}  // namespace yunying