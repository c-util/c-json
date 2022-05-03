#
# target: libjson.so
#

libcjson_symfile = join_paths(meson.current_source_dir(), 'libcjson.sym')

libcjson_deps = [
        dep_cstdaux,
        dep_cutf8,
]

libcjson_both = both_libraries(
        'cjson-'+major,
        [
                'c-json-reader.c',
        ],
        c_args: [
                '-fvisibility=hidden',
                '-fno-common',
        ],
        dependencies: libcjson_deps,
        install: not meson.is_subproject(),
        link_args: [
                '-Wl,--no-undefined',
                '-Wl,--version-script=@0@'.format(libcjson_symfile),
        ],
        link_depends: libcjson_symfile,
        soversion: 0,
)

libcjson_dep = declare_dependency(
        dependencies: libcjson_deps,
        include_directories: include_directories('.'),
        link_with: libcjson_both.get_static_lib(),
        version: meson.project_version(),
)

if not meson.is_subproject()
        install_headers('c-json.h')

        mod_pkgconfig.generate(
                description: project_description,
                filebase: 'libcjson-'+major,
                libraries: libcjson_both.get_shared_lib(),
                name: 'libcjson',
                version: meson.project_version(),
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
        args: [ json_validate, meson.project_source_root() + '/test'],
)
