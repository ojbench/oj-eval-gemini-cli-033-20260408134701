cmake_minimum_required(VERSION 3.10)
project(list)

file(GLOB SOURCES "*.cpp")
if(SOURCES)
    add_executable(code ${SOURCES})
else()
    message(WARNING "No .cpp files found")
endif()