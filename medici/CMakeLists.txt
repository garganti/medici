cmake_minimum_required (VERSION 3.6)

project (medici)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost COMPONENTS REQUIRED program_options) 
find_package(GMP REQUIRED)

# include_directories(${Boost_INCLUDE_DIRS}) 

#if(Boost_FOUND)   
#    add_executable(progname file1.cxx file2.cxx) 
#endif()

file(GLOB medici_src . src/*.cpp)

message(${medici_src})

#add_subdirectory(meddly_svn/lib)
#LINK_DIRECTORIES(meddly_svn/lib)
#add_library(libmeddly STATIC IMPORTED)
#set_property(TARGET libmeddly PROPERTY IMPORTED_LOCATION meddly_svn/lib/libmeddly.la)
LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/meddly_svn/lib)

add_executable(medici ${medici_src})

include_directories(meddly_svn/include)
target_link_libraries(medici ${Boost_LIBRARIES})
target_link_libraries(medici libmeddly.a)
target_link_libraries(medici gmp libgmp)

# TODO (per ora assumo che meddly sia già scaricato e compilato - non installato - no make install) 
# include(ExternalProject)
# ExternalProject_Add(meddly
#    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/meddly_svn
#    CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/meddly_svn/configure
#    BUILD_COMMAND ${MAKE})

#LINK_DIRECTORIES(meddly_svn/lib)

# gli include di meddly in src

set(CMAKE_CXX_FLAGS "-std=c++11")

set(MEDDLY_INCLUDE meddly_svn/src meddly_svn/src/forest)
 
set(MEDICI_INCLUDE src/include)

include_directories(${MEDDLY_INCLUDE} ${MEDICI_INCLUDE})



