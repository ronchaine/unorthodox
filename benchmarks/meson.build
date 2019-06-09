google_benchmark = subproject('benchmark')
benchmark_dependency = google_benchmark.get_variable('google_benchmark_dep')

math_benchmark_sources = [
  'math_basics.cpp'
]

math_benchmark = executable('math_benchmarks',
  math_benchmark_sources,
  include_directories : unorthodox_include_path,
  dependencies : benchmark_dependency
)

