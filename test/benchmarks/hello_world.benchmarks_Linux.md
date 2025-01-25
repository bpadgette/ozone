# Host Information

- **Platform**: Linux 6.8.0-51-generic (x64)
- **CPU**: 12-core 13th Gen Intel(R) Core(TM) i7-1355U
- **Memory**: 15678 MB (7238 MB free)

# Benchmarks

Using server `hello_world`

# Phase: Single User

For this benchmark, send and wait on requests for 10 seconds using 1 concurrent user.

This benchmark will be canceled if a request takes longer than 500 milliseconds or is dropped completely.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 69010 requests in 10.001 seconds
  - Requests per second: 6900
  - Mean response time: 0.145 ms

### User Experience

- For the 1 user in this phase:
  - Fastest response time: 0.082 ms
  - Slowest response time: 24.085 ms


# Phase: 30 users

For this benchmark, send and wait on requests for 10 seconds using 30 concurrent users.

This benchmark will be canceled if a request takes longer than 500 milliseconds or is dropped completely.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 119379 requests in 10.002 seconds
  - Requests per second: 11935
  - Mean response time: 0.084 ms

### User Experience

- For the 30 users in this phase:
  - Fastest response time: 1.465 ms
  - Slowest response time: 42.351 ms


# Phase: 100 users

For this benchmark, send and wait on requests for 10 seconds using 100 concurrent users.

This benchmark will be canceled if a request takes longer than 500 milliseconds or is dropped completely.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 112189 requests in 10.003 seconds
  - Requests per second: 11215
  - Mean response time: 0.089 ms

### User Experience

- For the 100 users in this phase:
  - Fastest response time: 2.555 ms
  - Slowest response time: 60.608 ms

