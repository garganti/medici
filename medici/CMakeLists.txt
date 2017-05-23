cmake_minimum_required (VERSION 3.6)

project (medici)


set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost COMPONENTS program_options) 

# include_directories(${Boost_INCLUDE_DIRS}) 

#if(Boost_FOUND)   
#    add_executable(progname file1.cxx file2.cxx) 
#endif()

file(GLOB medici_src . src/*.cpp)

message(${medici_src})

add_executable(medici ${medici_src})

target_link_libraries(medici ${Boost_LIBRARIES})


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



