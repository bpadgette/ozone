# Host Information

- **Platform**: Darwin 24.2.0 (x64)
- **CPU**: 4-core Intel(R) Core(TM) i3-8100B CPU @ 3.60GHz
- **Memory**: 8192 MB (199 MB free)

# Benchmarks

Using server `hello_world`

# Phase: Single User

For this benchmark, send and wait on requests for 10 seconds using 1 concurrent user.

This benchmark will be canceled if a request takes longer than 500 milliseconds or is dropped completely.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 17623 requests in 10.001 seconds
  - Requests per second: 1762
  - Mean response time: 0.567 ms

### User Experience

- For the 1 user in this phase:
  - Fastest response time: 0.384 ms
  - Slowest response time: 10.397 ms


# Phase: 30 users

For this benchmark, send and wait on requests for 10 seconds using 30 concurrent users.

This benchmark will be canceled if a request takes longer than 500 milliseconds or is dropped completely.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 7626 requests in 10.022 seconds
  - Requests per second: 761
  - Mean response time: 1.314 ms

### User Experience

- For the 30 users in this phase:
  - Fastest response time: 12.275 ms
  - Slowest response time: 65.536 ms


# Phase: 100 users

For this benchmark, send and wait on requests for 10 seconds using 100 concurrent users.

This benchmark will be canceled if a request takes longer than 500 milliseconds or is dropped completely.


## GET http://localhost:8080/ -> OK

### Server Performance

- Handled 6874 requests in 10.006 seconds
  - Requests per second: 687
  - Mean response time: 1.456 ms

### User Experience

- For the 100 users in this phase:
  - Fastest response time: 4.979 ms
  - Slowest response time: 266.957 ms

