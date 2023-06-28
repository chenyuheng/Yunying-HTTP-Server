# Yunying HTTP Server
Yunying(云影) is a simple HTTP server that supports static file serving, reverse proxy and in-memory cache. It is written in C++ and uses [epoll](https://man7.org/linux/man-pages/man7/epoll.7.html) to handle network events.

Yunying can now serve static files and reverse proxy for other HTTP servers. It cannot handle HTTPS yet.

The development of Yunying is in progress. You may track the progress in the [issue list](https://github.com/chenyuheng/Yunying-HTTP-Server/issues/).

## Build and Run
You need a Linux machine and CMake to build and run Yunying.

Before building Yunying, you will need to build and install Lua first. You can follow the instructions in [Lua's official website](https://www.lua.org/manual/5.4/readme.html) to build and install Lua. After that, you can build Yunying using `cmake`:
```bash
mkdir build
cd build
cmake ..
make
```

After build, you will get `yunying` executable file. After editing the config file, you can run it directly:
```bash
build/yunying [config_file] # config_file can be omitted, default to ./config.lua
```

## Performance Benchmarks
Run benchmarks with [wrk](https://github.com/wg/wrk) in a cloud server with 2 CPU cores and 2 GB memory.

### Reverse Proxy for cpc.people.com.cn
Run wrk for two URLs, one is a `404` page which is short and the other is the index page of cpc.people.com.cn which is relatively long. The results are as follows:

```bash
$ wrk -t4 -c1000 -d60s http://localhost:8080/404
Running 1m test @ http://localhost:8080/404
  4 threads and 1000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    19.50ms    5.04ms  73.28ms   73.27%
    Req/Sec    12.87k   763.50    29.07k    80.44%
  3072002 requests in 1.00m, 1.42GB read
  Non-2xx or 3xx responses: 3072002
Requests/sec:  51114.28
Transfer/sec:     24.18MB

$ wrk -t4 -c1000 -d60s http://localhost:8080/
Running 1m test @ http://localhost:8080/
  4 threads and 1000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   120.50ms  133.78ms   1.99s    96.25%
    Req/Sec     2.28k   310.73     3.08k    84.51%
  542520 requests in 1.00m, 67.25GB read
  Socket errors: connect 0, read 0, write 0, timeout 1168
Requests/sec:   9035.20
Transfer/sec:      1.12GB
```

## Libraries Used
- [Lua](https://www.lua.org/) - A powerful, efficient, lightweight, embeddable scripting language, used as the configuration language of Yunying.
- [sol2](https://sol2.readthedocs.io/en/latest/) - A C++ wrapper for Lua.