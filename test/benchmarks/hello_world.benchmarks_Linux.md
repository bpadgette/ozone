# Host Information

- **Platform**: Linux 6.8.0-52-generic (x64)
- **CPU**: 12-core 13th Gen Intel(R) Core(TM) i7-1355U
- **Memory**: 15678 MB (5166 MB free)

# Benchmarks

Using server `hello_world`

# Phase: Single User

For this benchmark, send and wait on requests for 10 seconds using 1 concurrent user.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 68143 requests in 10.017 seconds
  - Requests per second: 6803
  - Mean response time: 0.147 ms

### User Experience

- For the 1 user in this phase:
  - Fastest response time: 0.047 ms
  - Slowest response time: 37.640 ms


# Phase: 30 users

For this benchmark, send and wait on requests for 10 seconds using 30 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 276121 requests in 10.051 seconds
  - Requests per second: 27472
  - Mean response time: 0.036 ms

### User Experience

- For the 30 users in this phase:
  - Fastest response time: 0.325 ms
  - Slowest response time: 96.010 ms


# Phase: 100 users

For this benchmark, send and wait on requests for 10 seconds using 100 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 169981 requests in 10.016 seconds
  - Requests per second: 16971
  - Mean response time: 0.059 ms

### User Experience

- For the 100 users in this phase:
  - Fastest response time: 2.064 ms
  - Slowest response time: 90.513 ms

