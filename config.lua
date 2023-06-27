-- basic config
port = 8080
working_thread_num = 4

-- cache config
cache_size_bytes = 1024 * 1024 * 1024
cache_default_max_age = 300
cache_clean_interval = 10

-- origin config
origin_type = "upstream"            -- static_file or upstream
root_dir = "."                      -- static_file only
upstream_host = "cpc.people.com.cn" -- upstream only
upstream_ip = "157.185.156.194"     -- upstream only
upstream_port = 80                  -- upstream only
