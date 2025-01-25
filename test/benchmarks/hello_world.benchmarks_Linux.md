# Host Information

- **Platform**: Linux 6.8.0-51-generic (x64)
- **CPU**: 12-core 13th Gen Intel(R) Core(TM) i7-1355U
- **Memory**: 15678 MB (4613 MB free)

# Benchmarks

Using server `hello_world`

# Phase: Single User

For this benchmark, send and wait on requests for 10 seconds using 1 concurrent user.

This benchmark will be canceled if a request takes longer than 500 milliseconds or is dropped completely.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 70567 requests in 10.001 seconds
  - Requests per second: 7056
  - Mean response time: 0.142 ms

### User Experience

- For the 1 user in this phase:
  - Fastest response time: 0.081 ms
  - Slowest response time: 16.272 ms


# Phase: 30 users

For this benchmark, send and wait on requests for 10 seconds using 30 concurrent users.

This benchmark will be canceled if a request takes longer than 500 milliseconds or is dropped completely.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 130686 requests in 10.002 seconds
  - Requests per second: 13066
  - Mean response time: 0.077 ms

### User Experience

- For the 30 users in this phase:
  - Fastest response time: 1.231 ms
  - Slowest response time: 32.914 ms


# Phase: 100 users

For this benchmark, send and wait on requests for 10 seconds using 100 concurrent users.

This benchmark will be canceled if a request takes longer than 500 milliseconds or is dropped completely.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 125020 requests in 10.003 seconds
  - Requests per second: 12498
  - Mean response time: 0.080 ms

### User Experience

- For the 100 users in this phase:
  - Fastest response time: 2.571 ms
  - Slowest response time: 45.612 ms

