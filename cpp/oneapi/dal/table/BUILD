package(default_visibility = ["//visibility:public"])
load("@onedal//dev/bazel:dal.bzl", "dal_module", "dal_public_includes")

dal_public_includes(
    name = "public_includes",
)

dal_module(
    name = "table",
    dpc = True,
    auto = True,
    deps = [
        "@onedal//cpp/oneapi/dal:common",
        "@onedal//cpp/oneapi/dal/util",
    ]
)