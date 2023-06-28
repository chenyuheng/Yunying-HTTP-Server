-- basic config
port = "string"
working_thread_num = function (a,b)return (a.name > b.name) end

-- cache config
cache_size_bytes = "string233"
cache_default_max_age = "string344"
cache_clean_interval = "string455"

-- origin config
origin_type = "static_file"         -- static_file or upstream
root_dir = 12                       -- static_file only
upstream_host = 12.222              -- upstream only
upstream_ip = 1.2                   -- upstream only
upstream_port = "/?"                -- upstream only