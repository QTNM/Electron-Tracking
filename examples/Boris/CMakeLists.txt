cmake_minimum_required(VERSION 3.12)
project(egun VERSION 0.1.0)

# Compiler/build settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# CTest if we need it
include(CTest)

# Dependencies
find_package(Geant4 10.7 REQUIRED)

# Build
add_executable(egun
  egun.cc
  src/EGActionInitialization.cc
  src/EGDetectorConstruction.cc
  src/EGGasHit.cc
  src/EGGasSD.cc
  src/EGEventAction.cc
  src/EGPrimaryGeneratorAction.cc
  src/EGRunAction.cc
  src/QTNMPhysicsList.cc)
target_include_directories(egun PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(egun PRIVATE ${Geant4_LIBRARIES})

# Test
if(BUILD_TESTING)
  add_subdirectory(test)
endif()
