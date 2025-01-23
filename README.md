# ozone

Ozone is a minimal dependency, C-based web framework.

# Contents

- [Getting Started](#getting-started)
  - [Hello, World!](#hello-world)
  - [Other Examples](#other-examples)
- [Build & Install](#build-and-install)
- [Tests & Benchmarks](#tests-and-benchmarks)

# Getting Started <a name="getting-started"></a>

## Hello, World! <a name="hello-world"></a>

Build and run a release build of `./examples/hello_world.c` with:

```bash
make hello_world
```

## Other Examples <a name="other-examples"></a>

Learn more about ozone by reading and running the programs in`./examples`.

Build all of the examples to `./build/examples` with:

```bash
make build-examples
```

Debug builds and memcheck run using Valgrind can be executed respectively for any example program:

```bash
make hello_world.debug
make hello_world.memcheck
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

# Tests & Benchmarks <a name="tests-and-benchmarks"></a>

Build and run the tests in `./tests` with:

```bash
make test
```

Generate [benchmarks](./benchmarks/README.md) with:

```bash
make benchmarks
```
