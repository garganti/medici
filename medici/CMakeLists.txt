cmake_minimum_required (VERSION 2.6)

project (medici)

add_executable(medici src/medici.cpp)

# TO BE CREATED
# include(meddly/meddly.cmake)

include(ExternalProject)
ExternalProject_Add(meddly
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/meddly-0.13
    CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/meddly-0.13/configure
    BUILD_COMMAND ${MAKE})