cmake_minimum_required(VERSION 3.9)
project(tash)

SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "-D_GLIBCXX_USE_CXX11_ABI=1 ${CMAKE_CXX_FLAGS}")
SET(CMAKE_CXX_FLAGS "-ftemplate-backtrace-limit=0 ${CMAKE_CXX_FLAGS}")

FIND_PACKAGE(Threads)
FIND_PACKAGE(Boost COMPONENTS thread system REQUIRED) 

set(TASH_USE_EMBEDDED_JSON ON)
message(STATUS "TASH_PACKAGE_USE_EMBEDDED_JSON" ${TASH_PACKAGE_USE_EMBEDDED_JSON})
if(TASH_USE_EMBEDDED_JSON)
  find_package(nlohmann_json QUIET)
  if(NOT nlohmann_json_FOUND)
    set(TASH_PACKAGE_USE_EMBEDDED_JSON ON)
    add_library(nlohmann_json INTERFACE)
    add_library(tash::nlohmann_json ALIAS nlohmann_json)

    target_include_directories(nlohmann_json INTERFACE
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/dependencies>
    )
    target_compile_features(nlohmann_json INTERFACE cxx_std_11)
    set(TASH_SELECTED_JSON_LIBRARY "tash::nlohmann_json")
  else()
    set(TASH_SELECTED_JSON_LIBRARY "nlohmann_json::nlohmann_json")
  endif()
else()
  if(TARGET nlohmann_json)
    set(TASH_SELECTED_JSON_LIBRARY "nlohmann_json::nlohmann_json")
  else()
    find_package(nlohmann_json REQUIRED)
    set(TASH_SELECTED_JSON_LIBRARY "nlohmann_json::nlohmann_json")
  endif()
endif()

include_directories(dependencies/base-n/include)

SET(TASH_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/includes
)
SET(TASH_HEADERS 
    includes/tash/connection.h 
    includes/tash/query.h 
    includes/tash/filter.h 
    includes/tash/compositor.h 
    includes/tash/ddl.h
    includes/tash/arango.h
)
SET(TASH_SOURCES 
    connection.cpp 
    query.cpp 
    filter.cpp 
    compositor.cpp 
    ddl.cpp
    arango.cpp
)

add_library(tash SHARED ${TASH_SOURCES} ${TASH_HEADERS})
TARGET_LINK_LIBRARIES(tash PUBLIC ${Boost_SYSTEM_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} ${TASH_SELECTED_JSON_LIBRARY})
TARGET_INCLUDE_DIRECTORIES(tash PUBLIC ${TASH_INCLUDE_DIRS})

add_subdirectory(examples)
