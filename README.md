Mnemo: A Toolkit for Memory Locality Tracing and Optimization
=============================================================

Mnemo is a library and collection of language bindings to build locality
analysis and memory optimization tools.
The library is designed as a collection of state-of-the-art tools that
can be composed to create locality analysis experiment and help optimize an
application. Contrary to common performance analysis toolkits that act as
external binaries providing application-agnostic information, this library is
intended to be used by application performance engineers to build custom,
application-centered analysis.

This library is still in the prototyping phase. APIs might break often.

## Requirements:

* autoconf
* automake

## Installation

```
sh autogen.sh
./configure
make -j install
```

# Version Management
Mnemo versioning is similar to [semantic versioning](https://semver.org/).
Meno version is a string composed of 3 integers separated by a dot: "0.1.0"
The first integer is the major version and all versions with the same
major version are supposed ABI compatible except for major version 0 which is
considered unstable.

