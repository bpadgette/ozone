# Host Information

- **Platform**: Linux 6.8.0-52-generic (x64)
- **CPU**: 12-core 13th Gen Intel(R) Core(TM) i7-1355U
- **Memory**: 15678 MB (7703 MB free)

# Benchmarks

Using server `hello_world`

# Phase: Single User

For this benchmark, send and wait on requests for 10 seconds using 1 concurrent user.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 107698 requests in 10.012 seconds
  - Requests per second: 10757
  - Mean response time: 0.093 ms

### User Experience

- For the 1 user in this phase:
  - Fastest response time: 0.041 ms
  - Slowest response time: 33.440 ms


# Phase: 30 users

For this benchmark, send and wait on requests for 10 seconds using 30 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 277754 requests in 10.035 seconds
  - Requests per second: 27678
  - Mean response time: 0.036 ms

### User Experience

- For the 30 users in this phase:
  - Fastest response time: 0.221 ms
  - Slowest response time: 88.326 ms


# Phase: 100 users

For this benchmark, send and wait on requests for 10 seconds using 100 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 162794 requests in 10.056 seconds
  - Requests per second: 16189
  - Mean response time: 0.062 ms

### User Experience

- For the 100 users in this phase:
  - Fastest response time: 1.641 ms
  - Slowest response time: 79.297 ms

