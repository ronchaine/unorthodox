project(
  'Unorthodox',
  'cpp',
  default_options : [
    'cpp_std=c++2a',
    'buildtype=release',
    'default_library=static'
  ],
)
 
if meson.is_subproject() == false
  add_global_arguments(['-g', '-pg'], language : 'c')
  add_global_arguments(['-g', '-pg'], language : 'cpp')
endif

add_project_arguments(['-Wall', '-Wextra', '-Wno-missing-braces', '-pg'], language : 'cpp')

compiler = meson.get_compiler('cpp').get_id()

if compiler == 'clang'
  if meson.is_subproject() == false
    add_global_arguments(['-stdlib=libc++'], language : 'cpp')
    add_global_link_arguments(['-stdlib=libc++'], language : 'cpp')
  endif
endif

unorthodox_include_path = include_directories('include')

#subdir('src')

unorthodox_dep = declare_dependency(
    include_directories: unorthodox_include_path,
)

if meson.is_subproject() == false
  subdir('benchmarks')
  subdir('tests')
  subdir('examples')
endif
