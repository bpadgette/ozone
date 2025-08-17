# Host Information

- **Platform**: Linux 6.16.0-arch2-1 (x64)
- **CPU**: 16-core AMD Ryzen 7 9700X 8-Core Processor
- **Memory**: 31182 MB (27095 MB available)

# Benchmarks

Using server `hello_world`

# Phase: Single User

For this benchmark, send and wait on requests for 10 seconds using 1 concurrent user.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 64821 requests in 10.018 seconds
  - Requests per second: 6470
  - Mean response time: 0.155 ms

### User Experience

- For the 1 user in this phase:
  - Fastest response time: 0.030 ms
  - Slowest response time: 26.555 ms


# Phase: 30 users

For this benchmark, send and wait on requests for 10 seconds using 30 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 573453 requests in 10.082 seconds
  - Requests per second: 56877
  - Mean response time: 0.018 ms

### User Experience

- For the 30 users in this phase:
  - Fastest response time: 0.309 ms
  - Slowest response time: 77.265 ms


# Phase: 100 users

For this benchmark, send and wait on requests for 10 seconds using 100 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 564734 requests in 10.064 seconds
  - Requests per second: 56115
  - Mean response time: 0.018 ms

### User Experience

- For the 100 users in this phase:
  - Fastest response time: 1.034 ms
  - Slowest response time: 77.884 ms

