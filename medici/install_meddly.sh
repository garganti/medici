#!/bin/sh
# questi i comandi per installare meddly
#
# install dependencies (optional - check if alresdyinstalled)
# sudo apt-get install libgmp-dev
# sudo apt-get install autoconf

# to download and install meddly
wget -c https://github.com/asminer/meddly/archive/refs/heads/master.zip
#
unzip master.zip
cd meddly-master

# sudo ./developers/Config_generic.sh
# sudo make
# sudo install  

# some files are not copied see https://github.com/asminer/meddly/issues/4
cp src/rangeval.h include/meddly
cp src/oper_item.h include/meddly
#and aothers ... I copied all the .h
