# Host Information

- **Platform**: Linux 7.1.3-arch1-2 (x86_64)
- **CPU**: 16-thread AMD Ryzen 7 9700X 8-Core Processor
- **Memory**: 31179 MB
- **Load generator**: oha 1.16.0
- **CPU pinning**: server on 0-7, oha on 8-15

# Benchmarks

Using server `hello_world`, each phase runs for 10s against `http://localhost:8080/`.

| Phase | Connections | Rate | Req/s | Success | p50 ms | p90 ms | p99 ms | p99.9 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| single-user | 1 | max | 72157 | 100% | 0.013 | 0.014 | 0.017 | 0.026 | 0.896 |
| 30-users | 30 | max | 219891 | 100% | 0.135 | 0.144 | 0.171 | 0.272 | 5.652 |
| 100-users | 100 | max | 222087 | 100% | 0.447 | 0.475 | 0.518 | 0.629 | 4.837 |
| 100-users-no-keepalive | 100 | max | 21427 | 100% | 0.584 | 7.112 | 10.044 | 11.044 | 17.555 |
| 100-users-at-50k-rps | 100 | 50000 | 49990 | 100% | 0.164 | 0.312 | 0.461 | 0.784 | 7.748 |
