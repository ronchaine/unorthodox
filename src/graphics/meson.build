# Remote TAKE-FALLBACK if system GLFW is wanted
glfw      = dependency('glfw3-TAKE-FALLBACK', fallback : ['glfw', 'glfw_dep'])
#subproject('glfw')

libepoxy  = dependency('epoxy')

graphics_lib_sources = [
  'glfw_target/window.cpp'
]
graphics_lib_dependencies = [
  glfw,
  libepoxy,
]

unorthodox_graphics_lib = library(
  'unorthodox_graphics',
  graphics_lib_sources,
  include_directories: [unorthodox_include_path],
  dependencies: graphics_lib_dependencies,
  install: true,
)

unorthodox_graphics_lib_dep = declare_dependency(
  link_with: unorthodox_graphics_lib,
  dependencies: graphics_lib_dependencies,
  include_directories: [unorthodox_include_path]
)
