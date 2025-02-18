project('simple-os-simulator', 'c',
  default_options: ['c_std=c2x', 'warning_level=3'],
  version: '0.0.1'
)

include = include_directories('include')

sources = []
subdir('src')

ncurses = dependency('ncurses', required:true)

sim = executable('sim', sources,
  include_directories: include,
  dependencies: ncurses
)

if get_option('enable-tests')
  subdir('tests')
endif
