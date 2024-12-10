# ozone

Minimal dependency, C-based web framework.

# Getting Started

Consult the [Makefile](./Makefile); it contains useful documentation for dependencies
and recipes.

```
make all
```

## Compile-time Flags

| Flag              | Default  | Effect                                      |
| ----------------- | -------- | ------------------------------------------- |
| `OZONE_LOG_DEBUG` | disabled | Enables debug-level logging                 |
| `OZONE_LOG_TRACE` | disabled | Enables trace-level and debug-level logging |

## Testing

### Execute unit and integration tests

```
make test
```

### Check the debug build for memory leaks

```
make memcheck
```
