# ozone

Minimal dependency, C-based web framework.

# Getting Started

## Hello, World!

Build and run `./examples/hello_world.c` with:

```bash
make ex-hello_world
```

## Tests & Other Examples

Learn more about ozone by running the unit tests and programs in `./test` and `./examples`.

Build and run tests with:

```bash
make test
```

Build all of the examples to `./build/examples` with:

```bash
make build-examples
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
