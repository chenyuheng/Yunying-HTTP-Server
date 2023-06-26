# Yunying HTTP Server
Yunying(云影) is a simple HTTP server that supports static file serving, reverse proxy and cache. It is written in C++ and uses [epoll](https://man7.org/linux/man-pages/man7/epoll.7.html) to handle network events.

The development of Yunying is in progress. Current progress could be found in [issue #1](https://github.com/chenyuheng/Yunying-HTTP-Server/issues/1).

## Build and Run
```bash
make
./yunying [port_num] # port_num could be omitted, default to 8080
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
    Latency    54.58ms   15.86ms 505.11ms   81.83%
    Req/Sec     4.65k   518.55     5.96k    80.19%
  1106863 requests in 1.00m, 517.24MB read
  Non-2xx or 3xx responses: 1106863
Requests/sec:  18418.82
Transfer/sec:      8.61MB

$ wrk -t4 -c1000 -d60s http://localhost:8080/
Running 1m test @ http://localhost:8080/
  4 threads and 1000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   134.56ms   50.01ms   1.93s    90.02%
    Req/Sec     1.75k   367.33     3.57k    69.29%
  417178 requests in 1.00m, 51.66GB read
  Socket errors: connect 0, read 0, write 0, timeout 30
Requests/sec:   6944.50
Transfer/sec:      0.86GB
```