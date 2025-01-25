# Host Information

- **Platform**: Linux 6.8.0-51-generic (x64)
- **CPU**: 12-core 13th Gen Intel(R) Core(TM) i7-1355U
- **Memory**: 15678 MB (6013 MB free)

# Benchmarks

Using server `hello_world`

# Phase: Single User

For this benchmark, send and wait on requests for 10 seconds using 1 concurrent user.

This benchmark will be canceled if a request takes longer than 500 milliseconds or is dropped completely.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 66803 requests in 10.001 seconds
  - Requests per second: 6680
  - Mean response time: 0.150 ms

### User Experience

- For the 1 user in this phase:
  - Fastest response time: 0.081 ms
  - Slowest response time: 18.957 ms


# Phase: 30 users

For this benchmark, send and wait on requests for 10 seconds using 30 concurrent users.

This benchmark will be canceled if a request takes longer than 500 milliseconds or is dropped completely.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 98132 requests in 10.002 seconds
  - Requests per second: 9811
  - Mean response time: 0.102 ms

### User Experience

- For the 30 users in this phase:
  - Fastest response time: 1.424 ms
  - Slowest response time: 38.718 ms


# Phase: 100 users

For this benchmark, send and wait on requests for 10 seconds using 100 concurrent users.

This benchmark will be canceled if a request takes longer than 500 milliseconds or is dropped completely.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 98576 requests in 10.004 seconds
  - Requests per second: 9853
  - Mean response time: 0.101 ms

### User Experience

- For the 100 users in this phase:
  - Fastest response time: 4.027 ms
  - Slowest response time: 53.388 ms

