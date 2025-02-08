# Host Information

- **Platform**: Darwin 24.2.0 (x64)
- **CPU**: 4-core Intel(R) Core(TM) i3-8100B CPU @ 3.60GHz
- **Memory**: 8192 MB (201 MB free)

# Benchmarks

Using server `hello_world`

# Phase: Single User

For this benchmark, send and wait on requests for 10 seconds using 1 concurrent user.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 60459 requests in 10.055 seconds
  - Requests per second: 6013
  - Mean response time: 0.166 ms

### User Experience

- For the 1 user in this phase:
  - Fastest response time: 0.114 ms
  - Slowest response time: 77.579 ms


# Phase: 30 users

For this benchmark, send and wait on requests for 10 seconds using 30 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 122147 requests in 10.006 seconds
  - Requests per second: 12208
  - Mean response time: 0.082 ms

### User Experience

- For the 30 users in this phase:
  - Fastest response time: 1.208 ms
  - Slowest response time: 118.632 ms


# Phase: 100 users

For this benchmark, send and wait on requests for 10 seconds using 100 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 91471 requests in 10.027 seconds
  - Requests per second: 9122
  - Mean response time: 0.110 ms

### User Experience

- For the 100 users in this phase:
  - Fastest response time: 0.118 ms
  - Slowest response time: 195.289 ms

