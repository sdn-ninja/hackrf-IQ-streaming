cmake_minimum_required(VERSION 2.8.4)
project(blah2-hackrf CXX C)

set( PROJ_SOURCES src/main.cpp )

add_executable(blah2-hackrf ${PROJ_SOURCES} )

target_link_libraries(blah2-hackrf hackrf)
