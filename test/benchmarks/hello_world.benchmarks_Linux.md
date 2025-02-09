# Host Information

- **Platform**: Linux 6.8.0-52-generic (x64)
- **CPU**: 12-core 13th Gen Intel(R) Core(TM) i7-1355U
- **Memory**: 15678 MB (8829 MB free)

# Benchmarks

Using server `hello_world`

# Phase: Single User

For this benchmark, send and wait on requests for 10 seconds using 1 concurrent user.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 113592 requests in 10.031 seconds
  - Requests per second: 11324
  - Mean response time: 0.088 ms

### User Experience

- For the 1 user in this phase:
  - Fastest response time: 0.041 ms
  - Slowest response time: 41.760 ms


# Phase: 30 users

For this benchmark, send and wait on requests for 10 seconds using 30 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 303233 requests in 10.058 seconds
  - Requests per second: 30150
  - Mean response time: 0.033 ms

### User Experience

- For the 30 users in this phase:
  - Fastest response time: 0.484 ms
  - Slowest response time: 68.785 ms


# Phase: 100 users

For this benchmark, send and wait on requests for 10 seconds using 100 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 200335 requests in 10.051 seconds
  - Requests per second: 19931
  - Mean response time: 0.050 ms

### User Experience

- For the 100 users in this phase:
  - Fastest response time: 2.042 ms
  - Slowest response time: 71.424 ms

