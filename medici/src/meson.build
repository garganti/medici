
inc = include_directories('include')

meddly = static_library('meddly')
gmp = static_library('gmp')
boost = dependency('boost', modules : ['program_options'])

my_dep = declare_dependency(link_with : [meddly, gmp],
  include_directories : inc)

src = ['medici.cpp', 'logger.cpp','main.cpp','MDDList.cpp','Operations.cpp','Settings.cpp','Tuple.cpp','TupleList.cpp']

executable('medici', src, dependencies : [my_dep, boost])

