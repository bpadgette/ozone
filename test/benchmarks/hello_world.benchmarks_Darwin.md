# Host Information

- **Platform**: Darwin 24.2.0 (x64)
- **CPU**: 4-core Intel(R) Core(TM) i3-8100B CPU @ 3.60GHz
- **Memory**: 8192 MB (146 MB free)

# Benchmarks

Using server `hello_world`

# Phase: Single User

For this benchmark, send and wait on requests for 10 seconds using 1 concurrent user.

This benchmark will be canceled if a request takes longer than 500 milliseconds or is dropped completely.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 17737 requests in 10.002 seconds
  - Requests per second: 1773
  - Mean response time: 0.564 ms

### User Experience

- For the 1 user in this phase:
  - Fastest response time: 0.379 ms
  - Slowest response time: 11.535 ms


# Phase: 30 users

For this benchmark, send and wait on requests for 10 seconds using 30 concurrent users.

This benchmark will be canceled if a request takes longer than 500 milliseconds or is dropped completely.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 7635 requests in 10.024 seconds
  - Requests per second: 762
  - Mean response time: 1.313 ms

### User Experience

- For the 30 users in this phase:
  - Fastest response time: 14.264 ms
  - Slowest response time: 73.169 ms


# Phase: 100 users

For this benchmark, send and wait on requests for 10 seconds using 100 concurrent users.

This benchmark will be canceled if a request takes longer than 500 milliseconds or is dropped completely.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 8428 requests in 10.031 seconds
  - Requests per second: 840
  - Mean response time: 1.190 ms

### User Experience

- For the 100 users in this phase:
  - Fastest response time: 9.967 ms
  - Slowest response time: 325.338 ms

