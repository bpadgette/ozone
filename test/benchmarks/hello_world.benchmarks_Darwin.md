# Host Information

- **Platform**: Darwin 24.2.0 (x64)
- **CPU**: 4-core Intel(R) Core(TM) i3-8100B CPU @ 3.60GHz
- **Memory**: 8192 MB (3294 MB available)

# Benchmarks

Using server `hello_world`

# Phase: Single User

For this benchmark, send and wait on requests for 10 seconds using 1 concurrent user.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 51786 requests in 10.013 seconds
  - Requests per second: 5172
  - Mean response time: 0.193 ms

### User Experience

- For the 1 user in this phase:
  - Fastest response time: 0.122 ms
  - Slowest response time: 77.169 ms


# Phase: 30 users

For this benchmark, send and wait on requests for 10 seconds using 30 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 127988 requests in 10.063 seconds
  - Requests per second: 12719
  - Mean response time: 0.079 ms

### User Experience

- For the 30 users in this phase:
  - Fastest response time: 0.711 ms
  - Slowest response time: 109.740 ms


# Phase: 100 users

For this benchmark, send and wait on requests for 10 seconds using 100 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 87330 requests in 10.079 seconds
  - Requests per second: 8664
  - Mean response time: 0.115 ms

### User Experience

- For the 100 users in this phase:
  - Fastest response time: 2.418 ms
  - Slowest response time: 118.454 ms

