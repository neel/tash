cmake_minimum_required(VERSION 3.9)
project(tash-examples)

SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "-D_GLIBCXX_USE_CXX11_ABI=1 ${CMAKE_CXX_FLAGS}")
SET(CMAKE_CXX_FLAGS "-ftemplate-backtrace-limit=0 ${CMAKE_CXX_FLAGS}")

ADD_EXECUTABLE(tash-example-01 tash-example-01.cpp)
TARGET_LINK_LIBRARIES(tash-example-01 tash)

ADD_EXECUTABLE(tash-example-02 tash-example-02.cpp)
TARGET_LINK_LIBRARIES(tash-example-02 tash)
