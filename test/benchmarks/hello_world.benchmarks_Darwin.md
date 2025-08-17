# Host Information

- **Platform**: Darwin 24.2.0 (x64)
- **CPU**: 4-core Intel(R) Core(TM) i3-8100B CPU @ 3.60GHz
- **Memory**: 8192 MB (3382 MB available)

# Benchmarks

Using server `hello_world`

# Phase: Single User

For this benchmark, send and wait on requests for 10 seconds using 1 concurrent user.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 53129 requests in 10.019 seconds
  - Requests per second: 5303
  - Mean response time: 0.189 ms

### User Experience

- For the 1 user in this phase:
  - Fastest response time: 0.122 ms
  - Slowest response time: 77.259 ms


# Phase: 30 users

For this benchmark, send and wait on requests for 10 seconds using 30 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 92237 requests in 10.045 seconds
  - Requests per second: 9182
  - Mean response time: 0.109 ms

### User Experience

- For the 30 users in this phase:
  - Fastest response time: 0.586 ms
  - Slowest response time: 117.736 ms


# Phase: 100 users

For this benchmark, send and wait on requests for 10 seconds using 100 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 77168 requests in 10.092 seconds
  - Requests per second: 7647
  - Mean response time: 0.131 ms

### User Experience

- For the 100 users in this phase:
  - Fastest response time: 1.791 ms
  - Slowest response time: 116.058 ms

