-- basic config
port = 808
working_thread_num = 0

-- cache config
cache_size_bytes = 233
cache_default_max_age = 344
cache_clean_interval = 455

-- origin config
origin_type = "upstream"            -- static_file or upstream
root_dir = "test_root"              -- static_file only
upstream_host = "test.host"         -- upstream only
upstream_ip = "1.2.3.4"             -- upstream only
upstream_port = 23                  -- upstream only