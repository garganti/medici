
inc = include_directories('include')

meddly = static_library('meddly')

my_dep = declare_dependency(link_with : meddly,
  include_directories : inc)

executable('medici', 'medici.cpp', dependencies : my_dep)

