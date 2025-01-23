# Load Test Report

Using server `hello_world`

# Phase: Single User

For this benchmark, send and wait on requests for 10 seconds using 1 concurrent user.

Cancel this benchmark if a request takes longer than 100 milliseconds.


## GET http://localhost:8080/ -> OK

- 54209 requests in 10.001 seconds (5421 requests/s)
- Fastest in 0.088 ms
- Slowest in 25.621 ms


# Phase: 30 users

For this benchmark, send and wait on requests for 10 seconds using 30 concurrent users.

Cancel this benchmark if a request takes longer than 100 milliseconds.


## GET http://localhost:8080/ -> OK

- 101629 requests in 10.002 seconds (10161 requests/s)
- Fastest in 1.190 ms
- Slowest in 63.881 ms

