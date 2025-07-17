# Host Information

- **Platform**: Linux 6.15.6-arch1-1 (x64)
- **CPU**: 16-core AMD Ryzen 7 9700X 8-Core Processor
- **Memory**: 31182 MB (24718 MB available)

# Benchmarks

Using server `hello_world`

# Phase: Single User

For this benchmark, send and wait on requests for 10 seconds using 1 concurrent user.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 142843 requests in 10.011 seconds
  - Requests per second: 14269
  - Mean response time: 0.070 ms

### User Experience

- For the 1 user in this phase:
  - Fastest response time: 0.024 ms
  - Slowest response time: 33.466 ms


# Phase: 30 users

For this benchmark, send and wait on requests for 10 seconds using 30 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 575738 requests in 10.071 seconds
  - Requests per second: 57167
  - Mean response time: 0.017 ms

### User Experience

- For the 30 users in this phase:
  - Fastest response time: 0.111 ms
  - Slowest response time: 74.390 ms


# Phase: 100 users

For this benchmark, send and wait on requests for 10 seconds using 100 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 566468 requests in 10.040 seconds
  - Requests per second: 56422
  - Mean response time: 0.018 ms

### User Experience

- For the 100 users in this phase:
  - Fastest response time: 1.003 ms
  - Slowest response time: 78.804 ms

