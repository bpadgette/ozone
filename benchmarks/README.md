# Load Test Report

Using server `hello_world`

# Phase: Single User

For this benchmark, send and wait on requests for 10 seconds using 1 concurrent user.

Cancel this benchmark if a request takes longer than 100 milliseconds.


## GET http://localhost:8080/ -> OK

- 7491 requests in 8.497 seconds (882 requests/s)
- Fastest in 0.740 ms
- Slowest in 169.088 ms


# Phase: 30 users

For this benchmark, send and wait on requests for 10 seconds using 30 concurrent users.

Cancel this benchmark if a request takes longer than 100 milliseconds.


## GET http://localhost:8080/ -> OK

- 6064 requests in 4.641 seconds (1307 requests/s)
- Fastest in 9.197 ms
- Slowest in 223.757 ms

