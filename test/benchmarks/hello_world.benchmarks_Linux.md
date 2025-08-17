# Host Information

- **Platform**: Linux 6.16.0-arch2-1 (x64)
- **CPU**: 16-core AMD Ryzen 7 9700X 8-Core Processor
- **Memory**: 31182 MB (27101 MB available)

# Benchmarks

Using server `hello_world`

# Phase: Single User

For this benchmark, send and wait on requests for 10 seconds using 1 concurrent user.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 69250 requests in 10.031 seconds
  - Requests per second: 6904
  - Mean response time: 0.145 ms

### User Experience

- For the 1 user in this phase:
  - Fastest response time: 0.031 ms
  - Slowest response time: 28.518 ms


# Phase: 30 users

For this benchmark, send and wait on requests for 10 seconds using 30 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 574307 requests in 10.047 seconds
  - Requests per second: 57160
  - Mean response time: 0.017 ms

### User Experience

- For the 30 users in this phase:
  - Fastest response time: 0.308 ms
  - Slowest response time: 73.635 ms


# Phase: 100 users

For this benchmark, send and wait on requests for 10 seconds using 100 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 568983 requests in 10.065 seconds
  - Requests per second: 56529
  - Mean response time: 0.018 ms

### User Experience

- For the 100 users in this phase:
  - Fastest response time: 1.043 ms
  - Slowest response time: 78.599 ms

