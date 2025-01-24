# Load Test Report

Using server `hello_world`

# Phase: Single User

For this benchmark, send and wait on requests for 10 seconds using 1 concurrent user.

Cancel this benchmark if a request takes longer than 100 milliseconds.


## GET http://localhost:8080/ -> OK

- 81441 requests in 10.001 seconds (8143 requests/s)
- Fastest in 0.078 ms
- Slowest in 17.038 ms


# Phase: 30 users

For this benchmark, send and wait on requests for 10 seconds using 30 concurrent users.

Cancel this benchmark if a request takes longer than 100 milliseconds.


## GET http://localhost:8080/ -> OK

- 105920 requests in 10.002 seconds (10590 requests/s)
- Fastest in 1.090 ms
- Slowest in 60.822 ms


# Phase: 100 users

For this benchmark, send and wait on requests for 10 seconds using 100 concurrent users.

Cancel this benchmark if a request takes longer than 100 milliseconds.


## GET http://localhost:8080/ -> OK

- 96528 requests in 10.005 seconds (9648 requests/s)
- Fastest in 1.289 ms
- Slowest in 84.703 ms

