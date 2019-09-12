unorthodox_event_lib_sources = [
    'event.cpp'
]

unorthodox_network_lib_sources = [
]

unorthodox_graphics_lib_sources = [
]

unorthodox_event = shared_library('unorthodox_event',
    unorthodox_event_lib_sources,
    include_directories : unorthodox_include_path
)

unorthodox_event_dep = declare_dependency(
    link_with: unorthodox_event,
    include_directories: unorthodox_include_path,
)

unorthodox_network = shared_library('unorthodox_network',
    unorthodox_network_lib_sources,
    include_directories : unorthodox_include_path
)

unorthodox_network_dep = declare_dependency(
    link_with: unorthodox_network,
    include_directories: unorthodox_include_path,
)

unorthodox_graphics = shared_library('unorthodox_gfw',
    unorthodox_graphics_lib_sources,
    include_directories : unorthodox_include_path
)

unorthodox_graphics_dep = declare_dependency(
    link_with: unorthodox_graphics,
    include_directories: unorthodox_include_path,
)
