# Host Information

- **Platform**: Darwin 24.2.0 (x64)
- **CPU**: 4-core Intel(R) Core(TM) i3-8100B CPU @ 3.60GHz
- **Memory**: 8192 MB (117 MB free)

# Benchmarks

Using server `hello_world`

# Phase: Single User

For this benchmark, send and wait on requests for 10 seconds using 1 concurrent user.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 56369 requests in 10.047 seconds
  - Requests per second: 5611
  - Mean response time: 0.178 ms

### User Experience

- For the 1 user in this phase:
  - Fastest response time: 0.116 ms
  - Slowest response time: 78.364 ms


# Phase: 30 users

For this benchmark, send and wait on requests for 10 seconds using 30 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 125542 requests in 10.066 seconds
  - Requests per second: 12472
  - Mean response time: 0.080 ms

### User Experience

- For the 30 users in this phase:
  - Fastest response time: 0.708 ms
  - Slowest response time: 113.401 ms


# Phase: 100 users

For this benchmark, send and wait on requests for 10 seconds using 100 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 89378 requests in 10.135 seconds
  - Requests per second: 8819
  - Mean response time: 0.113 ms

### User Experience

- For the 100 users in this phase:
  - Fastest response time: 0.149 ms
  - Slowest response time: 180.426 ms

