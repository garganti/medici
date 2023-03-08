
inc = include_directories('include')

meddly = meson.get_compiler('cpp').find_library('meddly')
boost = dependency('boost', modules : ['program_options'])

src = ['medici.cpp', 'logger.cpp','main.cpp','MDDList.cpp','Operations.cpp','Settings.cpp','Tuple.cpp','TupleList.cpp']

executable('medici', src, dependencies : [boost, meddly], include_directories : inc)

