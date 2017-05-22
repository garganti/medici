cmake_minimum_required (VERSION 2.6)

project (medici)

add_executable(medici src/main.cpp)

# TODO (per ora assumo che meddly sia già scaricato e compilato - non installato - no make install) 
# include(ExternalProject)
# ExternalProject_Add(meddly
#    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/meddly_svn
#    CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/meddly_svn/configure
#    BUILD_COMMAND ${MAKE})

# gli include di meddly in src

set(CMAKE_CXX_FLAGS "-std=c++11")

set(MEDDLY_INCLUDE meddly_svn/src meddly_svn/src/forest)

set(MEDICI_INCLUDE src/include)

include_directories(${MEDDLY_INCLUDE} ${MEDICI_INCLUDE})