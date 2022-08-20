# OPRF

C++ implementation of OPRF. This implementation uses Libsodium Ristretto as a backend.

This was developed and tested using `Bazel 4.2.1` and `g++11` on Ubuntu.

## Depending on OPRF in your project

### Using Bazel

This library uses bazel.

If your project also uses bazel, you can add this library as a dependency by adding
the following to your `WORKSPACE` file:
```python
# Load the git_repository utility if you have not already
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

# Use the current OPRF library from github main branch
git_repository(
    name = "OPRF",
    branch = "main",
    remote = "https://github.com/KinanBab/OPRF.git",
)

# Configure the dependencies of OPRF: rules_cc, rules_foreign_cc, and libsodium
load("@OPRF//:deps.bzl", "oprf_dependencies", "oprf_dev_dependencies")
oprf_dependencies()

# Load the dependencies
load("@OPRF//:loads.bzl", "oprf_load_dependencies")
oprf_load_dependencies()
```

### Not using Bazel

If you are using linux and your project does not use bazel, you can clone this
repo and install and build the OPRF library from source using
`bazel run --config=release :install`.

This will install the compiled libraries and header files to `/usr/lib/OPRF/`
and `/usr/include/OPRF` respectively. Make sure you add `/usr/lib/OPRF/` to your
environments `LD_LIBRARY_PATH`.

You can then link OPRF when compiling your project. You need to link both
`libOPRF.so` and `libsodium.a`, both are installed into `/usr/lib/OPRF` with
our install command. For example:
```bash
g++ main.cc -I/usr/include/OPRF -L/usr/lib/OPRF -lOPRF -lsodium
```

If you are using a different operating system, you can look at `install.bzl` to see
how our installation script works, and mimic it for your system. In particular,
our script installs our `oprf.h` header (and the libsodium headers), as well as
the Bazel-built `libOPRF.so` and `libsodium.a`.

You can purge OPRF from your system using `bazel run :uninstall`.


## Library API

Our API is defined under `OPRF/oprf.h`, for example usage, check out `OPRF/oprf_unittest.cc`,
and in particular the `End2End` test, which shows the end-to-end usage.
