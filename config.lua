-- basic config
port = 8080
working_thread_num = 4

-- cache config
cache_size_bytes = 1 << 25
cache_default_max_age = 300
cache_clean_interval = 10

-- origin config
origin_type = "upstream"            -- static_file or upstream
root_dir = "."                      -- static_file only
upstream_host = "chenyuheng.github.io"     -- upstream only
upstream_domain = "chenyuheng.github.io"     -- upstream only
upstream_protocol = "https"         -- upstream only

-- upstream customized config
function upstream_set(req_path, req_method, req_host)
    if (req_path:find("^/chemidle") ~= nil) then
        set_host("chemidle.com")
        set_path(req_path.sub(req_path, 14))
    end
    if (req_path:find("^/upstream") ~= nil) then
        set_host("upstream.host")
        set_path(req_path.sub(req_path, 15))
    end
end