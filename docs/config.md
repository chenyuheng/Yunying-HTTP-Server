The configuration file of Yunying is written in Lua, which provides flexibility and extensibility.

## Run Yunying with a configuration file
Running Yunying requires a config file, even if the config file is empty. If the config file path is not specified, Yunying will try to load the default config file `./config.lua` in the current directory.

```bash
./yunying [config_file]
```

## Configuration items
If any configuration item is not specified, the default value will be used.

### Basic config

| Name | Type | Default | Description |
| ---- | ---- | ------- | ----------- |
| port | int16 | 8080 | The port Yunying listens on |
| working_thread_num | int16 | 4 | The number of working threads |

### Cache config
Config for the in-memory cache, by default you don't need to change it.
| Name | Type | Default | Description |
| ---- | ---- | ------- | ----------- |
| cache_size_bytes | int64 | 1<<25 (2^25) | The size of the cache in bytes |
| cache_default_max_age | int64 | 300 | The default max age of the cache in seconds |
| cache_clean_interval | int64 | 10 | The interval of the cache clean in seconds |

### Origin config
Origin is the source of the content, it can be a static file server or an upstream server.

If static file server is used, the `origin_type` should be set to `static_file`, and the `root_dir` should be set to the root directory of the static file server.

If upstream server is used, the `origin_type` should be set to `upstream`, and the `upstream_host`, `upstream_ip` and `upstream_port` should be set to the host, ip and port of the upstream server. You can also write your own upstream logic in the `upstream_set` function.

| Name | Type | Default | Description | Used For |
| ---- | ---- | ------- | ----------- | ------- |
| origin_type | string | "static_file" | The type of the origin, can be "static_file" or "upstream" | - |
| root_dir | string | "." | The root directory of the static file server | static_file |
| upstream_host | string | "upstream.host" | The host of the upstream server | upstream |
| upstream_ip | string | "185.199.110.153" | The ip of the upstream server | upstream |
| upstream_port | int16 | 80 | The port of the upstream server | upstream |

#### `upstream_set` function
You can define your own upstream logic in the `upstream_set` function. When Yunying receives a request, it will generate an upstream request to retrieve the content from the upstream server. You can use the `upstream_set` function to customize the upstream request before it is sent to the upstream server.

You can use this function to implement multiple upstream servers, load balancing, etc.

You are provided with the request path, method and host, and you can set upstream request using provides request-setting functions.

You may implement this function:
```lua
function upstream_set(req_path, req_method, req_host)
```

Request-setting functions you can use:
```lua
function set_path(path)
function set_method(method)
function set_host(host)
function set_body(body)
function set_upstream_ip(ip_str)
function set_upstream_port(port)
```

## Example config
### Static file server
```lua
port = 80
origin_type = "static_file"
root_dir = "/var/www"
```

### Upstream server
```lua
port = 80
origin_type = "upstream"
upstream_host = "upstream.host"
upstream_ip = "185.199.110.153"
upstream_port = 80
```

### Upstream server with multiple upstream servers
```lua
port = 80
origin_type = "upstream"
-- default upstream server
upstream_host = "upstream.host"
upstream_ip = "185.199.110.153"
upstream_port = 80

function upstream_set(req_path, req_method, req_host)
    if req_host == "github.example.com" then
        set_upstream_ip("185.199.110.153")
        set_host("github.com")
    end
    if req_host == "google.example.com" then
        set_upstream_ip("142.251.42.238")
        set_host("google.com")
    end
end
```