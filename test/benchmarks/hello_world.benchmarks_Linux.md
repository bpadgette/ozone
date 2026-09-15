# Host Information

- **Platform**: Linux 7.1.3-arch1-2 (x64)
- **CPU**: 16-core AMD Ryzen 7 9700X 8-Core Processor
- **Memory**: 31179 MB (25597 MB available)

# Benchmarks

Using server `hello_world`

# Phase: Single User

For this benchmark, send and wait on requests for 10 seconds using 1 concurrent user.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 119868 requests in 10.003 seconds
  - Requests per second: 11984
  - Mean response time: 0.083 ms

### User Experience

- For the 1 user in this phase:
  - Fastest response time: 0.019 ms
  - Slowest response time: 3.848 ms


# Phase: 30 users

For this benchmark, send and wait on requests for 10 seconds using 30 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 793986 requests in 10.005 seconds
  - Requests per second: 79357
  - Mean response time: 0.013 ms

### User Experience

- For the 30 users in this phase:
  - Fastest response time: 0.132 ms
  - Slowest response time: 19.258 ms


# Phase: 100 users

For this benchmark, send and wait on requests for 10 seconds using 100 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 771638 requests in 10.003 seconds
  - Requests per second: 77141
  - Mean response time: 0.013 ms

### User Experience

- For the 100 users in this phase:
  - Fastest response time: 0.691 ms
  - Slowest response time: 18.556 ms

