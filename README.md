# ozone

Minimal dependency, C-based web framework.

# Getting Started

## Hello, World!

Build and run `./examples/hello_world.c` with:

```bash
make build-examples && ./build/examples/hello_world
```

## Examples & Tests

Learn more about ozone by checking out the other programs in `./examples` and the `./test` directory.

Build the examples with:

```bash
make build-examples
```

Find the built examples in `./build/examples`.

### Testing

Build and run tests with:

```bash
make test
```

# Build & Install

Build the shared library, `libozone.so`.

```bash
make build
```

Install the shared library, `libozone.so`, to `/usr/lib` and copy `./include` to `/usr/include`.

```bash
make install
```

Uninstall with:

```bash
make uninstall
```

## Build options

### Compile-time Flags

| Flag              | Default  | Effect                                      |
| ----------------- | -------- | ------------------------------------------- |
| `OZONE_LOG_DEBUG` | disabled | Enables debug-level logging                 |
| `OZONE_LOG_TRACE` | disabled | Enables trace-level and debug-level logging |
