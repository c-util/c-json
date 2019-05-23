#
# target: libjson.so
#

libcjson_symfile = join_paths(meson.current_source_dir(), 'libcjson.sym')

libcjson_deps = [
        dep_cstdaux,
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
# target: test-*
#

test_basic = executable('test-basic', ['test-basic.c'], dependencies: libcjson_dep)
test('Basic API Behavior', test_basic)

# test_basic = executable('test-basic', ['test-basic.c'], dependencies: libcjson_dep)
# test('Basic API Behavior', test_basic)
# 
# if dep_typenum.found()
#         test_enumerated = executable('test-enumerated', ['test-enumerated.c'], dependencies: [ libcjson_dep, dep_typenum ])
#         test('Type and Data Verification with Enumerated Types', test_enumerated)
# endif
# 
# test_string = executable('test-string', ['test-string.c'], dependencies: libcjson_dep)
# test('D-Bus String Restrictions', test_string)
# 
# test_type = executable('test-type', ['test-type.c'], dependencies: libcjson_dep)
# test('Type and Signature Parser', test_type)
