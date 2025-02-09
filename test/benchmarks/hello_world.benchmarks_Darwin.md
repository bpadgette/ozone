# Host Information

- **Platform**: Darwin 24.2.0 (x64)
- **CPU**: 4-core Intel(R) Core(TM) i3-8100B CPU @ 3.60GHz
- **Memory**: 8192 MB (335 MB free)

# Benchmarks

Using server `hello_world`

# Phase: Single User

For this benchmark, send and wait on requests for 10 seconds using 1 concurrent user.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 59177 requests in 10.055 seconds
  - Requests per second: 5885
  - Mean response time: 0.170 ms

### User Experience

- For the 1 user in this phase:
  - Fastest response time: 0.114 ms
  - Slowest response time: 79.765 ms


# Phase: 30 users

For this benchmark, send and wait on requests for 10 seconds using 30 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 111741 requests in 10.019 seconds
  - Requests per second: 11153
  - Mean response time: 0.090 ms

### User Experience

- For the 30 users in this phase:
  - Fastest response time: 1.360 ms
  - Slowest response time: 120.548 ms


# Phase: 100 users

For this benchmark, send and wait on requests for 10 seconds using 100 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 105987 requests in 10.041 seconds
  - Requests per second: 10556
  - Mean response time: 0.095 ms

### User Experience

- For the 100 users in this phase:
  - Fastest response time: 5.203 ms
  - Slowest response time: 124.777 ms

