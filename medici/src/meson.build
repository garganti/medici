
inc = include_directories('include')

meddly = meson.get_compiler('cpp').find_library('meddly')

#meddly = dependency('meddly')
boost = dependency('boost', modules : ['program_options'])

my_dep = declare_dependency(#link_with : [gmp],
  include_directories : inc)

src = ['medici.cpp', 'logger.cpp','main.cpp','MDDList.cpp','Operations.cpp','Settings.cpp','Tuple.cpp','TupleList.cpp']

executable('medici', src, dependencies : [my_dep, boost, meddly])

