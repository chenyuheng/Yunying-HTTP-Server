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
    Latency    16.32ms    6.69ms 108.94ms   73.45%
    Req/Sec    15.27k     1.65k   25.24k    73.31%
  3648646 requests in 1.00m, 184.42MB read
Requests/sec:  60705.60
Transfer/sec:      3.07MB

$ wrk -t4 -c10000 -d60s http://localhost:8080
Running 1m test @ http://localhost:8080
  4 threads and 10000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   178.36ms   47.20ms   1.88s    76.29%
    Req/Sec    11.84k     2.23k   28.88k    72.97%
  2789243 requests in 1.00m, 140.98MB read
  Socket errors: connect 0, read 0, write 0, timeout 31
Requests/sec:  46393.19
Transfer/sec:      2.34MB
```