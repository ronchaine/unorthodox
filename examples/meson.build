cppevents_dep = dependency('libcppevents', fallback:['libcppevents', 'cppevents_dep'])
cppevents_sdl2_dep = dependency('libcppevents_sdl2', fallback:['libcppevents', 'cppevents_sdl2_dep'])

sdl2_dep = dependency('sdl2')
epoxy_dep = dependency('epoxy')

window_example_sources = [
  'window.cpp',
  'gfx/sdl2-window.cpp',
  'gfx/opengl-es-renderer.cpp',
]

window_example_dependencies = [
  cppevents_dep,
  cppevents_sdl2_dep,
  sdl2_dep,
  epoxy_dep,
]

window_example = executable('window',
  window_example_sources,
  include_directories : unorthodox_include_path,
  dependencies: window_example_dependencies,
)

pipe_example_sources = [
  'pipes.cpp',
]

pipe_example_dependencies = [
]

pipe_example = executable('pipe',
  pipe_example_sources,
  include_directories : unorthodox_include_path,
  dependencies: pipe_example_dependencies,
)

# unfinished
#file_example_sources = [
#    'files_and_buffers.cpp',
#]
#
#file_example_dependencies = [
#]
#
#file_example = executable('files_and_buffers',
#  file_example_sources,
#  include_directories : unorthodox_include_path,
#  dependencies: file_example_dependencies,
#)


# Network examples
cppevents_socket_example_sources = [
  'cppevents-server.cpp',
]

cppevents_socket_example = executable('socket_server',
  cppevents_socket_example_sources,
  include_directories : unorthodox_include_path,
  dependencies: cppevents_dep,
)
