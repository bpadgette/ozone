# Host Information

- **Platform**: Linux 6.8.0-51-generic (x64)
- **CPU**: 12-core 13th Gen Intel(R) Core(TM) i7-1355U
- **Memory**: 15678 MB (9724 MB free)

# Benchmarks

Using server `hello_world`

# Phase: Single User

For this benchmark, send and wait on requests for 10 seconds using 1 concurrent user.

This benchmark will be canceled if a request takes longer than 100 milliseconds.


## GET http://localhost:8080/ -> OK

- 81823 requests in 10.001 seconds (8181 requests/s)
- Fastest in 0.080 ms
- Slowest in 17.665 ms

# Phase: 30 users

For this benchmark, send and wait on requests for 10 seconds using 30 concurrent users.

This benchmark will be canceled if a request takes longer than 100 milliseconds.


## GET http://localhost:8080/ -> OK

- 112315 requests in 10.003 seconds (11228 requests/s)
- Fastest in 1.188 ms
- Slowest in 60.604 ms

# Phase: 100 users

For this benchmark, send and wait on requests for 10 seconds using 100 concurrent users.

This benchmark will be canceled if a request takes longer than 100 milliseconds.


## GET http://localhost:8080/ -> OK

- 101516 requests in 10.013 seconds (10139 requests/s)
- Fastest in 1.626 ms
- Slowest in 99.120 ms
