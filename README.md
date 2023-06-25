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
    Latency    45.79ms   24.46ms   1.72s    94.17%
    Req/Sec     4.72k     1.57k   10.55k    69.03%
  1124283 requests in 1.00m, 531.81MB read
  Socket errors: connect 0, read 0, write 0, timeout 36
  Non-2xx or 3xx responses: 1124283
Requests/sec:  18720.11
Transfer/sec:      8.86MB

$ wrk -t4 -c1000 -d60s http://localhost:8080/
Running 1m test @ http://localhost:8080/
  4 threads and 1000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   131.15ms   71.15ms   1.97s    94.04%
    Req/Sec     1.71k   366.10     2.69k    68.12%
  408814 requests in 1.00m, 50.55GB read
  Socket errors: connect 0, read 0, write 0, timeout 196
Requests/sec:   6806.24
Transfer/sec:    861.82MB
```