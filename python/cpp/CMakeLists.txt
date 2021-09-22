cmake_minimum_required(VERSION 3.12)
project(qtnm_fields VERSION 0.1.0)

# Compiler/build settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Look for Boost.
find_package(Boost)

if(Boost_FOUND)
  add_compile_definitions(HAVE_BOOST)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

add_library(qtnm_bathtub MODULE
  src/qtnm_bathtub.cpp)

target_include_directories(qtnm_bathtub PRIVATE ${PROJECT_SOURCE_DIR}/include)
