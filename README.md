# Yunying HTTP Server
Yunying(云影) is a simple HTTP server that supports static file serving, reverse proxy and cache. It is written in C++ and uses [epoll](https://man7.org/linux/man-pages/man7/epoll.7.html) to handle network events.

The development of Yunying is in progress. Current progress could be found in [issue #1](https://github.com/chenyuheng/Yunying-HTTP-Server/issues/1).

## Build and Run
```bash
cd src
make
./yunying [port_num] # port_num could be omitted, default to 8080
```

## Performance Benchmarks
Run benchmarks with [wrk](https://github.com/wg/wrk) in a cloud server with 2 CPU cores and 2 GB memory.

### Hardcoded HTTP response
The server will always responses with content `Hello, world!\n`.
```bash
$ wrk -t4 -c1000 -d60s http://localhost:8080
Running 1m test @ http://localhost:8080
  4 threads and 1000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    27.87ms   17.65ms   1.78s    92.81%
    Req/Sec     7.94k     2.33k   14.71k    62.27%
  1894353 requests in 1.00m, 95.75MB read
  Socket errors: connect 0, read 0, write 0, timeout 30
Requests/sec:  31553.27
Transfer/sec:      1.59MB

$ wrk -t4 -c10000 -d60s http://localhost:8080
Running 1m test @ http://localhost:8080
  4 threads and 10000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   335.46ms  118.71ms   1.88s    90.70%
    Req/Sec     5.80k     3.15k   13.63k    65.80%
  416071 requests in 1.02m, 21.03MB read
  Socket errors: connect 0, read 0, write 0, timeout 31224
Requests/sec:   6778.54
Transfer/sec:    350.84KB
```