# Host Information

- **Platform**: Linux 6.8.0-51-generic (x64)
- **CPU**: 12-core 13th Gen Intel(R) Core(TM) i7-1355U
- **Memory**: 15678 MB (11948 MB free)

# Benchmarks

Using server `hello_world`

# Phase: Single User

For this benchmark, send and wait on requests for 10 seconds using 1 concurrent user.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 76272 requests in 10.001 seconds
  - Requests per second: 7627
  - Mean response time: 0.131 ms

### User Experience

- For the 1 user in this phase:
  - Fastest response time: 0.086 ms
  - Slowest response time: 364.786 ms


# Phase: 30 users

For this benchmark, send and wait on requests for 10 seconds using 30 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 138499 requests in 10.003 seconds
  - Requests per second: 13846
  - Mean response time: 0.072 ms

### User Experience

- For the 30 users in this phase:
  - Fastest response time: 1.044 ms
  - Slowest response time: 284.970 ms


# Phase: 100 users

For this benchmark, send and wait on requests for 10 seconds using 100 concurrent users.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 141201 requests in 10.005 seconds
  - Requests per second: 14113
  - Mean response time: 0.071 ms

### User Experience

- For the 100 users in this phase:
  - Fastest response time: 3.914 ms
  - Slowest response time: 86.813 ms

