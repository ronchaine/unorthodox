project(
  'Unorthodox',
  'cpp',
  default_options : [
    'cpp_std=c++2a',
    'buildtype=release',
    'default_library=static'
  ],
)

compiler = meson.get_compiler('cpp').get_id()

if compiler == 'clang'
  add_global_arguments(['-stdlib=libc++'], language : 'cpp')
  add_global_link_arguments(['-stdlib=libc++'], language : 'cpp')

  add_project_arguments(['-Xclang', '-fconcepts-ts'], language : 'cpp')
else
  error('build requires c++20 concepts, use clang as a compiler')
endif

unorthodox_include_path = include_directories('include')

subdir('benchmarks')
subdir('tests')

