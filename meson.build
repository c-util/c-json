project(
        'c-json',
        'c',
        default_options: [
                'c_std=c11',
        ],
        license: 'Apache',
        meson_version: '>=0.60.0',
        version: '1.0.0',
)
major = meson.project_version().split('.')[0]
project_description = 'Streaming-capable JSON Implementation'

mod_pkgconfig = import('pkgconfig')

dep_cstdaux = dependency('libcstdaux-1')
dep_cutf8 = dependency('libcutf8-1')
add_project_arguments(dep_cstdaux.get_variable('cflags').split(' '), language: 'c')

subdir('src')

meson.override_dependency('libcjson-'+major, libcjson_dep, static: true)
