load("@rules_cc//cc:repositories.bzl", "rules_cc_dependencies")
load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")

def oprf_load_dependencies():
    rules_cc_dependencies()
    rules_foreign_cc_dependencies()

