# ozone

[![CI](https://github.com/bpadgette/ozone/actions/workflows/main-ci.yml/badge.svg)](https://github.com/bpadgette/ozone/actions/workflows/main-ci.yml)

Ozone is a minimal dependency, C-based web framework.

# Contents

- [Getting Started](#getting-started)
  - [Hello, World!](#hello-world)
  - [Other Examples](#other-examples)
- [Build & Install](#build-and-install)
  - [Tests & Benchmarks](#tests-and-benchmarks)
- [Features](#features)
  - [Router](#router)
  - [Middlewares](#middlewares)
  - [Workers](#workers)
- [Principles](#principles)

# Getting Started <a name="getting-started"></a>

Ozone is a hobby project. It is and will not be production-ready.

Use Ozone to learn and challenge your perceptions of C code. The section
[Principles](#principles) covers this subject.

## Hello, World! <a name="hello-world"></a>

Here is the source code for the tiniest reasonable Ozone application:

```C
#include <ozone/ozone.h>

void handler(OzoneAppEvent* event) {
  ozoneStringWrite(event->allocator, &event->response->body, "Hello, World! This is how simple Ozone is.");
}

int main(int argc, char* argv[]) {
  OzoneAppEndpointVector endpoints = ozoneVector(OzoneAppEndpoint, ozoneAppEndpoint(GET, "/", handler));

  return ozoneAppServe(argc, argv, &endpoints);
}
```

A version of this program is available in `./examples/tiny.c`; run it with
`make tiny`.

For a more comprehensive demo, build and run a release build of
`./examples/hello_world.c` with:

```bash
make hello_world

# Not using clang? Override the C-compiler value here:
make hello_world CC=gcc
```

## Other Examples <a name="other-examples"></a>

Learn more about ozone by reading and running the programs in`./examples`.

Build all of the examples to `./build/examples` with:

```bash
make build-examples
```

> [!IMPORTANT]\
> **Valgrind** is the memory-checking binary used by the `.memcheck` make rules.

Debug builds, benchmarks, and memcheck runs can be executed for any example
program:

```bash
make hello_world.debug
make hello_world.benchmarks
make hello_world.memcheck
```

# Build & Install <a name="build-and-install"></a>

Build and install the shared library, `libozone`.

> [!IMPORTANT]\
> I test builds with clang and GCC, and the root-level Makefile allows you to as
> well since it defines most of its binaries in variables. `CC` is defined for
> example, i.e. `make build CC=gcc` would allow you to overwrite the default
> value clang and build with GCC.

```bash
make build
make install
```

Uninstall with:

```bash
make uninstall
```

## Tests & Benchmarks <a name="tests-and-benchmarks"></a>

Build and run the tests in `./tests` with:

```bash
make test
```

> [!IMPORTANT]\
> The benchmarks script is written in TypeScript and runs with **Deno**. This
> script is not consistent, a goal of mine is to later use an industry-standard
> benchmarks framework for this purpose.

Run [benchmarks](./test/benchmarks) for any example program, such as
`hello_world` with:

```bash
make hello_world.benchmarks
```

# Features <a name="features"></a>

WIP: list features to document for now

## Router <a name="router"></a>

## Middlewares <a name="middlewares"></a>

## Workers <a name="workers"></a>

# Principles <a name="principles"></a>

Ozone is:

1. a C project.
2. a tool for learning.
3. good enough to use for a personal website (work in progress).

Ozone builds:

1. without linking non-std libraries.
2. without compiler warnings on its strictest settings (-Wall -Werror
   -pedantic).
3. for dynamic linking; I want one ozone lib for several tiny web-server
   binaries.

Ozone allows me:

1. to route and handle basic HTTP 1.x requests for many connections at once.
2. to provide sub-millisecond responses for simple text templating responses.
3. to manage memory and sockets as limited resources, not hogged and not leaked.

Ozone's code:

1. can use preprocessor directives freely; this is part of its charm and
   experience with macro pitfalls is expected of the developer.
2. should implement containers where appropriate for a web-server; i.e. strings,
   vectors, and maps.
3. is extensible; an Ozone handler is stateless with a simple signature so that
   it may be re-used across projects.
