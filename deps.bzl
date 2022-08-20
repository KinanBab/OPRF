load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def oprf_dependencies():
    maybe(
        http_archive,
        name = "rules_cc",
        sha256 = "35f2fb4ea0b3e61ad64a369de284e4fbbdcdba71836a5555abb5e194cf119509",
        strip_prefix = "rules_cc-624b5d59dfb45672d4239422fa1e3de1822ee110",
        urls = [
            "https://mirror.bazel.build/github.com/bazelbuild/rules_cc/archive/624b5d59dfb45672d4239422fa1e3de1822ee110.tar.gz",
            "https://github.com/bazelbuild/rules_cc/archive/624b5d59dfb45672d4239422fa1e3de1822ee110.tar.gz",
        ],
    )

    maybe(
        http_archive,
        name = "rules_foreign_cc",
        sha256 = "6041f1374ff32ba711564374ad8e007aef77f71561a7ce784123b9b4b88614fc",
        strip_prefix = "rules_foreign_cc-0.8.0",
        url = "https://github.com/bazelbuild/rules_foreign_cc/archive/0.8.0.tar.gz",
    )

    # libsodium
    maybe(
        http_archive,
        name = "libsodium",
        build_file_content = """
load("@rules_foreign_cc//foreign_cc:defs.bzl", "configure_make")

filegroup(
    name = "all_srcs",
    srcs = glob(["**"]),
    visibility = ["//visibility:public"]
)

configure_make(
    name = "libsodium",
    lib_source = ":all_srcs",
    out_lib_dir = "lib",
    visibility = ["//visibility:public"],
)
""",
        strip_prefix = "libsodium-1.0.18-RELEASE",
        type = "zip",
        url = "https://github.com/jedisct1/libsodium/archive/1.0.18-RELEASE.zip",
    )

def oprf_dev_dependencies():
    # Google Test
    maybe(
        http_archive,
        name = "com_google_googletest",  # 2021-05-19T20:10:13Z
        sha256 = "8cf4eaab3a13b27a95b7e74c58fb4c0788ad94d1f7ec65b20665c4caf1d245e8",
        strip_prefix = "googletest-aa9b44a18678dfdf57089a5ac22c1edb69f35da5",
        urls = ["https://github.com/google/googletest/archive/aa9b44a18678dfdf57089a5ac22c1edb69f35da5.zip"],
    )
