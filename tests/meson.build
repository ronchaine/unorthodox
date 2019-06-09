math_test_sources = [
  'run_tests.cpp',
  'math_basics.cpp'
]

math_test = executable('math_tests',
  math_test_sources,
  include_directories : unorthodox_include_path,
)

colour_test_sources = [
  'run_tests.cpp',
  'colour.cpp'
]

colour_test = executable('colour_test',
  colour_test_sources,
  include_directories : unorthodox_include_path,
)

all_test_sources = math_test_sources + colour_test_sources

all_tests = executable('all_tests',
  all_test_sources,
  include_directories : unorthodox_include_path,
)


test('unorthodox mathematics test', math_test)
test('unorthodox colour test', colour_test)

