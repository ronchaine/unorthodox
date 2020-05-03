data_structure_test_sources = [
  'run_tests.cpp',
  'dynamic_array.cpp',
]

data_structure_test = executable('datastruct_tests',
  data_structure_test_sources,
  include_directories : unorthodox_include_path,
)

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

thread_dep = dependency('threads')

# Network
tcp_test_sources = [
  'run_tests.cpp',
  'tcp_tests.cpp',
]

tcp_test = executable('tcp_test',
  tcp_test_sources,
  include_directories : unorthodox_include_path,
  dependencies: thread_dep,
)

all_test_sources = data_structure_test_sources + math_test_sources + colour_test_sources + tcp_test_sources

all_tests = executable('all_tests',
  all_test_sources,
  include_directories : unorthodox_include_path,
  dependencies: thread_dep,
)


test('unorthodox mathematics test', math_test)
test('unorthodox colour test', colour_test)
test('unorthodox tcp sockets test', tcp_test)

