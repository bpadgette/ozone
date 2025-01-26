# Host Information

- **Platform**: Darwin 24.2.0 (x64)
- **CPU**: 4-core Intel(R) Core(TM) i3-8100B CPU @ 3.60GHz
- **Memory**: 8192 MB (1008 MB free)

# Benchmarks

Using server `hello_world`

# Phase: Single User

For this benchmark, send and wait on requests for 10 seconds using 1 concurrent user.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 18178 requests in 10.002 seconds
  - Requests per second: 1817
  - Mean response time: 0.550 ms

### User Experience

- For the 1 user in this phase:
  - Fastest response time: 0.369 ms
  - Slowest response time: 144.841 ms


# Phase: 30 users

For this benchmark, send and wait on requests for 10 seconds using 30 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 7719 requests in 10.020 seconds
  - Requests per second: 770
  - Mean response time: 1.298 ms

### User Experience

- For the 30 users in this phase:
  - Fastest response time: 13.484 ms
  - Slowest response time: 89.771 ms


# Phase: 100 users

For this benchmark, send and wait on requests for 10 seconds using 100 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 6351 requests in 10.037 seconds
  - Requests per second: 633
  - Mean response time: 1.580 ms

### User Experience

- For the 100 users in this phase:
  - Fastest response time: 8.999 ms
  - Slowest response time: 271.928 ms

