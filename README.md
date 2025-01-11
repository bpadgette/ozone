# ozone

Ozone is a minimal dependency, C-based web framework.

# Contents

- [Getting Started](#getting-started)
  - [Hello, World!](#hello-world)
  - [Tests & Examples](#tests-and-examples)
- [Build & Install](#build-and-install)
  - [Build Options](#build-options)
- [Features](#features)

# Getting Started <a name="getting-started"></a>

## Hello, World! <a name="hello-world"></a>

Build and run `./examples/hello_world.c` with:

```bash
make hello_world
```

## Tests & Examples <a name="tests-and-examples"></a>

Learn more about ozone by running the unit tests and programs in `./test` and `./examples`.

Build and run tests with:

```bash
make test
```

Build all of the examples to `./build/examples` with:

```bash
make build-examples
```

# Build & Install <a name="build-and-install"></a>

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

## Build Options <a name="build-options"></a>

### Compile-time Flags

| Flag              | Default  | Effect                                      |
| ----------------- | -------- | ------------------------------------------- |
| `OZONE_LOG_DEBUG` | disabled | Enables debug-level logging                 |
| `OZONE_LOG_TRACE` | disabled | Enables trace-level and debug-level logging |

# Features <a name="features"></a>

## HTTP Pipelines

## Template Processor
