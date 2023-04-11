
inc = include_directories('include')

meddly = meson.get_compiler('cpp').find_library('meddly')
boost = dependency('boost', modules : ['program_options'])
gmp = meson.get_compiler('cpp').find_library('gmp')

src = ['medici.cpp', 'logger.cpp','main.cpp','MDDList.cpp','Operations.cpp','Settings.cpp','Tuple.cpp','TupleList.cpp']

executable('medici', src, dependencies : [boost, meddly, gmp], include_directories : inc, link_args: ['-static'])

