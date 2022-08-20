def _install_impl(ctx):
    runfiles = []
    cclib = ctx.attr.library
    cclib_ccinfo = cclib[CcInfo]

    # Figure out installation location
    name = ctx.attr.library.label.name
    lib_dir = "/usr/lib/{}/".format(name)
    include_dir = "/usr/include/{}/".format(name)

    # If we need to uninstall
    if ctx.attr.uninstall:
      ctx.actions.write(
          output = ctx.outputs.executable,
          content = """
              echo "Uninstalling from {lib_dir} and {include_dir}"
              sudo rm -rf "{lib_dir}"
              sudo rm -rf "{include_dir}"
          """.format(
              lib_dir = lib_dir,
              include_dir = include_dir,
          ),
      )
      return DefaultInfo()

    # If we are here, then we need to install    
    # Find all headers to copy to include/
    headers = cclib_ccinfo.compilation_context.headers.to_list()
    header_copies = []
    for h in headers:
      runfiles.append(h)
      if h.is_directory:
        header_copies.append("sudo cp -r {}/* ${{INCLUDE_DIR}}".format(h.short_path))
      else:
        prefix = h.dirname
        header_copies.append("sudo mkdir -p ${{INCLUDE_DIR}}/{}".format(prefix))
        header_copies.append("sudo cp {} ${{INCLUDE_DIR}}/{}".format(h.short_path, prefix))

    # Find nested libraries to link with
    lib_copies = []
    linked_libs = cclib_ccinfo.linking_context.linker_inputs.to_list()[1:]
    for linked_lib in linked_libs:
        for lib in linked_lib.libraries:
            if lib.static_library:
                runfiles.append(lib.static_library)
                lib_copies.append("sudo cp {} ${{LIB_DIR}}".format(lib.static_library.short_path))
            elif lib.dynamic_library:
                runfiles.append(lib.dynamic_library)
                lib_copies.append("sudo cp {} ${{LIB_DIR}}".format(lib.dynamic_library.short_path))
            else:
                fail("Linking dependency unknown type")

    # Find the library .so file to copy to lib/
    cclib_outs = cclib[OutputGroupInfo]
    so_list = cclib_outs.dynamic_library.to_list()
    for so_file in so_list:
        runfiles.append(so_file)
        lib_copies.append("sudo cp {} ${{LIB_DIR}}".format(so_file.short_path))

    # Perform the copies.
    idnt = '\n            '
    ctx.actions.write(
        output = ctx.outputs.executable,
        content = """
            LIB_DIR="{lib_dir}"
            INCLUDE_DIR="{include_dir}"
            echo "Installing to ${{LIB_DIR}} and ${{INCLUDE_DIR}}"
            echo "Please add ${{LIB_DIR}} to LD_LIBRARY_PATH"
            sudo mkdir -p ${{LIB_DIR}}
            sudo mkdir -p ${{INCLUDE_DIR}}
            {lib_copies}
            {header_copies}
            sudo chmod -R 755 ${{LIB_DIR}}
            sudo chmod -R 755 ${{INCLUDE_DIR}}
        """.format(
            lib_dir = lib_dir,
            include_dir = include_dir,
            lib_copies = idnt.join(lib_copies),
            header_copies = idnt.join(header_copies),
        )
    )

    return DefaultInfo(
      runfiles = ctx.runfiles(files = runfiles),
    )

install = rule(
    doc = """
        Installs OPRF to the local system (assumes linux)
        """,
    implementation = _install_impl,
    executable = True,
    attrs = {
        "library": attr.label(
            doc = """The cc_library to install""",
            mandatory = True,
            providers = [CcInfo],
        ),
        "uninstall": attr.bool(
            doc = """Whether to install or uninstall""",
            mandatory = False,
            default = False,
        ),
    },
)
