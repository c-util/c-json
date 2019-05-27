#
# target: libjson.so
#

libcjson_symfile = join_paths(meson.current_source_dir(), 'libcjson.sym')

libcjson_deps = [
        dep_cstdaux,
        dep_cutf8,
]

libcjson_private = static_library(
        'cjson-private',
        [
                'c-json.c',
        ],
        c_args: [
                '-fvisibility=hidden',
                '-fno-common',
        ],
        dependencies: libcjson_deps,
        pic: true,
)

libcjson_shared = shared_library(
        'cjson',
        objects: libcjson_private.extract_all_objects(),
        dependencies: libcjson_deps,
        install: not meson.is_subproject(),
        soversion: 0,
        link_depends: libcjson_symfile,
        link_args: [
                '-Wl,--no-undefined',
                '-Wl,--version-script=@0@'.format(libcjson_symfile),
        ],
)

libcjson_dep = declare_dependency(
        include_directories: include_directories('.'),
        link_with: libcjson_private,
        dependencies: libcjson_deps,
        version: meson.project_version(),
)

if not meson.is_subproject()
        install_headers(
                'c-json.h',
        )

        mod_pkgconfig.generate(
                libraries: libcjson_shared,
                version: meson.project_version(),
                name: 'libcjson',
                filebase: 'libcjson',
                description: project_description,
        )
endif

#
# target: json-validate
#

json_validate = executable('json-validate', ['json-validate.c'], dependencies: libcjson_dep)

#
# target: test-*
#

test_basic = executable('test-basic', ['test-basic.c'], dependencies: libcjson_dep)
test('test-basic', test_basic)

test(
        'test-reader',
        find_program('test-reader'),
        args: [ json_validate, meson.source_root() + '/test'],
)
