# Host Information

- **Platform**: Darwin 24.2.0 (x64)
- **CPU**: 4-core Intel(R) Core(TM) i3-8100B CPU @ 3.60GHz
- **Memory**: 8192 MB (16 MB free)

# Benchmarks

Using server `hello_world`

# Phase: Single User

For this benchmark, send and wait on requests for 10 seconds using 1 concurrent user.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 56662 requests in 10.041 seconds
  - Requests per second: 5643
  - Mean response time: 0.177 ms

### User Experience

- For the 1 user in this phase:
  - Fastest response time: 0.111 ms
  - Slowest response time: 78.046 ms


# Phase: 30 users

For this benchmark, send and wait on requests for 10 seconds using 30 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 112772 requests in 10.005 seconds
  - Requests per second: 11271
  - Mean response time: 0.089 ms

### User Experience

- For the 30 users in this phase:
  - Fastest response time: 0.356 ms
  - Slowest response time: 120.392 ms


# Phase: 100 users

For this benchmark, send and wait on requests for 10 seconds using 100 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 125516 requests in 10.046 seconds
  - Requests per second: 12494
  - Mean response time: 0.080 ms

### User Experience

- For the 100 users in this phase:
  - Fastest response time: 4.609 ms
  - Slowest response time: 123.366 ms

