project('medici', 'cpp')

# Define the custom target
meddly_lib = custom_target(
  'meddly',  # Name of the target
  command: 'install_meddly.sh',
  output: 'libmeddly.a'
)


# adesso leggo la libreria
dir_base = meson.current_source_dir()
meddly = meson.get_compiler('cpp').find_library('meddly' , 
	dirs: join_paths(dir_base, 'meddly-master/lib/'))


subdir('src')
