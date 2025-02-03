# Host Information

- **Platform**: Linux 6.8.0-52-generic (x64)
- **CPU**: 12-core 13th Gen Intel(R) Core(TM) i7-1355U
- **Memory**: 15678 MB (9152 MB free)

# Benchmarks

Using server `hello_world`

# Phase: Single User

For this benchmark, send and wait on requests for 10 seconds using 1 concurrent user.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 61945 requests in 10.005 seconds
  - Requests per second: 6191
  - Mean response time: 0.162 ms

### User Experience

- For the 1 user in this phase:
  - Fastest response time: 0.045 ms
  - Slowest response time: 48.202 ms


# Phase: 30 users

For this benchmark, send and wait on requests for 10 seconds using 30 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 242591 requests in 10.040 seconds
  - Requests per second: 24162
  - Mean response time: 0.041 ms

### User Experience

- For the 30 users in this phase:
  - Fastest response time: 0.329 ms
  - Slowest response time: 67.130 ms


# Phase: 100 users

For this benchmark, send and wait on requests for 10 seconds using 100 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 171594 requests in 10.015 seconds
  - Requests per second: 17134
  - Mean response time: 0.058 ms

### User Experience

- For the 100 users in this phase:
  - Fastest response time: 2.095 ms
  - Slowest response time: 91.944 ms

