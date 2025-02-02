# Host Information

- **Platform**: Linux 6.8.0-51-generic (x64)
- **CPU**: 12-core 13th Gen Intel(R) Core(TM) i7-1355U
- **Memory**: 15678 MB (9050 MB free)

# Benchmarks

Using server `hello_world`

# Phase: Single User

For this benchmark, send and wait on requests for 10 seconds using 1 concurrent user.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 100995 requests in 10.009 seconds
  - Requests per second: 10090
  - Mean response time: 0.099 ms

### User Experience

- For the 1 user in this phase:
  - Fastest response time: 0.042 ms
  - Slowest response time: 34.965 ms


# Phase: 30 users

For this benchmark, send and wait on requests for 10 seconds using 30 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 312979 requests in 10.031 seconds
  - Requests per second: 31202
  - Mean response time: 0.032 ms

### User Experience

- For the 30 users in this phase:
  - Fastest response time: 0.460 ms
  - Slowest response time: 91.703 ms


# Phase: 100 users

For this benchmark, send and wait on requests for 10 seconds using 100 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 290152 requests in 10.038 seconds
  - Requests per second: 28907
  - Mean response time: 0.035 ms

### User Experience

- For the 100 users in this phase:
  - Fastest response time: 2.101 ms
  - Slowest response time: 71.219 ms

