project('medici', 'cpp')

# meddly
# to download and install meddly
# 
# questi i comandi per installare meddly
# wget https://github.com/asminer/meddly/archive/refs/heads/master.zip
# unzip master.zip
# cd meddly-master
# sudo apt-get install libgmp-dev
# sudo apt-get install autoconf
# installa latre libs
# sudo ./developers/Config_generic.sh
# sudo make
# sudo install  
# questo nonbostale l'uso di su install meddly in una sub dir locale di meddly-master


# adesso leggo la libreria
dir_base = meson.current_source_dir()
meddly = meson.get_compiler('cpp').find_library('meddly' , 
	dirs: join_paths(dir_base, 'meddly-master/lib/'))


subdir('src')
