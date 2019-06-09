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
google_benchmark = subproject('benchmark')

benchmark_dependency = google_benchmark.get_variable('google_benchmark_dep')

math_benchmark_sources = [
  'benchmarks/math_basics.cpp'
]

math_test_sources = [
  'tests/math_basics.cpp'
]

colour_test_sources = [
  'tests/colour.cpp'
]

math_benchmark = executable('math_benchmarks',
  math_benchmark_sources,
  include_directories : unorthodox_include_path,
  dependencies : benchmark_dependency
)

math_test = executable('math_tests',
  math_test_sources,
  include_directories : unorthodox_include_path,
  #  dependencies : test_dependency
)

colour_test = executable('colour_test',
  colour_test_sources,
  include_directories : unorthodox_include_path,
)

test('unorthodox mathematics test', math_test)
test('unorthodox colour test', colour_test)


