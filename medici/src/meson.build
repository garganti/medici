
# include directory 

inc = include_directories('include')


#meddly = meson.get_compiler('cpp').find_library('meddly' , dirs: dir_meddly_lib)

#meddly_inc = include_directories('../meddly-master/include')
#meddly_lib = static_library(../meddly-master/lib/libmeddly.so)
#meddly = declare_dependency(link_with : meddly_lib,
#  include_directories : meddly_inc)


boost = dependency('boost', modules : ['program_options'])
gmp = meson.get_compiler('cpp').find_library('gmp')

src = ['medici.cpp', 'logger.cpp','main.cpp','MDDList.cpp','Operations.cpp','Settings.cpp','Tuple.cpp','TupleList.cpp']

executable('medici', src, dependencies : [boost, meddly, gmp], include_directories : inc, link_args: ['-static'])

