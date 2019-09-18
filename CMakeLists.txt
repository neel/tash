cmake_minimum_required(VERSION 3.9)
project(tokai)

SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "-D_GLIBCXX_USE_CXX11_ABI=1 ${CMAKE_CXX_FLAGS}")
SET(CMAKE_CXX_FLAGS "-ftemplate-backtrace-limit=0 ${CMAKE_CXX_FLAGS}")

FIND_PACKAGE(Threads)
FIND_PACKAGE(Boost COMPONENTS thread system REQUIRED) 

set(TOKAI_USE_EMBEDDED_JSON ON)
message(STATUS "TOKAI_PACKAGE_USE_EMBEDDED_JSON" ${TOKAI_PACKAGE_USE_EMBEDDED_JSON})
if(TOKAI_USE_EMBEDDED_JSON)
  find_package(nlohmann_json QUIET)
  if(NOT nlohmann_json_FOUND)
    set(TOKAI_PACKAGE_USE_EMBEDDED_JSON ON)
    add_library(nlohmann_json INTERFACE)
    add_library(tokai::nlohmann_json ALIAS nlohmann_json)

    target_include_directories(nlohmann_json INTERFACE
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/dependencies>
    )
    target_compile_features(nlohmann_json INTERFACE cxx_std_11)
    set(TOKAI_SELECTED_JSON_LIBRARY "tokai::nlohmann_json")
  else()
    set(TOKAI_SELECTED_JSON_LIBRARY "nlohmann_json::nlohmann_json")
  endif()
else()
  if(TARGET nlohmann_json)
    set(TOKAI_SELECTED_JSON_LIBRARY "nlohmann_json::nlohmann_json")
  else()
    find_package(nlohmann_json REQUIRED)
    set(TOKAI_SELECTED_JSON_LIBRARY "nlohmann_json::nlohmann_json")
  endif()
endif()


SET(TOKAI_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/includes
)
SET(TOKAI_HEADERS 
    includes/tokai/connection.h 
    includes/tokai/query.h 
    includes/tokai/filter.h 
    includes/tokai/compositor.h 
    includes/tokai/ddl.h
    includes/tokai/arango.h
)
SET(TOKAI_SOURCES 
    connection.cpp 
    query.cpp 
    filter.cpp 
    compositor.cpp 
    ddl.cpp
    arango.cpp
)

add_library(tokai SHARED ${TOKAI_SOURCES} ${TOKAI_HEADERS})
TARGET_LINK_LIBRARIES(tokai PUBLIC ${Boost_SYSTEM_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} ${TOKAI_SELECTED_JSON_LIBRARY})
TARGET_INCLUDE_DIRECTORIES(tokai PUBLIC ${TOKAI_INCLUDE_DIRS})
