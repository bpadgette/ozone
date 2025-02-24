# ozone

Ozone is a minimal dependency, C-based web framework.

# Contents

- [Getting Started](#getting-started)
  - [Timeline](#timeline)
  - [Hello, World!](#hello-world)
  - [Other Examples](#other-examples)
- [Build & Install](#build-and-install)
- [Tests & Benchmarks](#tests-and-benchmarks)
- [Principles](#principles)

# Getting Started <a name="getting-started"></a>

> [!IMPORTANT]  
> Ozone is a hobby project. It is and will not be production-ready.
>
> Use Ozone to learn and challenge your perceptions of C code. The section [Principles](#principles) covers this subject.

## Timeline

| Delivery          | Features                                                                                                      |
| ----------------- | ------------------------------------------------------------------------------------------------------------- |
| **Milestone 0**   | _Write C code, work without non-std dependencies_                                                             |
| [x] October 2024  | Initialize repository, try building an arena allocator for socket response handling.                          |
| [x] December 2024 | Implement a nominal HTTP 1.0 parser, ignore content marshaling and other complexities, basic text templating. |
| [x] January 2025  | Performance focus: stabilize sub-millisecond responses and write a [benchmarks](./test/benchmarks) script.    |
| [x] February 2025 | Ergonomics, reduce complexity of Ozone handlers. Path-parameter parsing for HTTP requests.                    |
| **Milestone 1**   | _Toward a working web-app built on Ozone_                                                                     |
| [ ] March 2025    | Static file-serving with efficient chunking and accurate content-types.                                       |
| [ ] May 2025      | Ozone serves its own styled, readable, informative docs site.                                                 |
| **Milestone 2**   | _Dynamic web applications and APIs_                                                                           |
| [ ] ?             | Basic crypto via optional OpenSSL linking with some basic auth strategy middlewares.                          |
| [ ] ?             | Database connection pooling.                                                                                  |
| [ ] ?             | HTTP/2                                                                                                        |
| [ ] ?             | Websockets                                                                                                    |

View [closed pull requests](https://github.com/bpadgette/ozone/pulls?q=is%3Apr+is%3Aclosed) fir more details.

## Hello, World! <a name="hello-world"></a>

Here is the source code for the tiniest reasonable Ozone application:

```C
#include <ozone/ozone.h>

void handler(OzoneAppEvent* event) {
  event->response->body = *ozoneString(event->allocator, "Hello, World! This is how simple Ozone is.");
}

int main(int argc, char* argv[]) {
  OzoneAppEndpointVector endpoints = ozoneVectorFromElements(OzoneAppEndpoint, ozoneAppEndpoint(GET, "/", handler));

  return ozoneAppServe(argc, argv, &endpoints);
}
```

A version of this program is available in `./examples/tiny.c`; run it with `make tiny`.

For a more comprehensive demo, build and run a release build of `./examples/hello_world.c` with:

```bash
make hello_world
```

## Other Examples <a name="other-examples"></a>

Learn more about ozone by reading and running the programs in`./examples`.

Build all of the examples to `./build/examples` with:

```bash
make build-examples
```

> [!IMPORTANT]  
> **Valgrind** is the memory-checking binary used by the `.memcheck` make rules.

Debug builds, benchmarks, and memcheck runs can be executed for any example program:

```bash
make hello_world.debug
make hello_world.benchmarks
make hello_world.memcheck
```

# Build & Install <a name="build-and-install"></a>

Build and install the shared library, `libozone`.

> [!IMPORTANT]  
> I test builds with clang and GCC, and the root-level Makefile allows you to as well since it defines most of its binaries in variables.
> `CC` is defined for example, i.e. `CC=gcc make build` would allow you to overwrite the default value clang and build with GCC.

```bash
make build
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

> [!IMPORTANT]  
> The benchmarks script is written in TypeScript and runs with **Deno**.

Run [benchmarks](./test/benchmarks) for any example program, such as `hello_world` with:

```bash
make hello_world.benchmarks
```

# Principles <a name="principles"></a>

Ozone is:

1. a C project.
2. a tool for learning.
3. good enough to use for a personal website (in progress).

Ozone builds:

1. without linking non-std libraries.
2. without compiler warnings on its strictest settings (-Wall -Werror -pedantic).
3. for dynamic linking (I want one ozone lib for several tiny web-server binaries).

Ozone allows me:

1. to route and handle basic HTTP 1.x requests for many connections at once.
2. to provide sub-millisecond responses for simple text templating responses.
3. to manage memory and sockets as limited resources, not hogged and not leaked.

Ozone's code:

1. can use preprocessor directives freely; that is part of its charm and experience with macro pitfalls is expected.
2. should implement containers where appropriate for a web-server; i.e. strings, vectors, and maps.
3. is extensible; an Ozone handler is stateless with a simple signature so that it may be re-used across projects.
