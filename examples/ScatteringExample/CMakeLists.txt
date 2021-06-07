cmake_minimum_required(VERSION 3.12)
project(scattering VERSION 0.1.0)

# Compiler/build settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# CTest if we need it
include(CTest)

# Dependencies
find_package(Geant4 10.7 REQUIRED)

# Build
add_executable(scattering
  scattering.cc
  src/SEActionInitialization.cc
  src/SEDetectorConstruction.cc
  src/SEGasHit.cc
  src/SEGasSD.cc
  src/SEEventAction.cc
  src/SEPrimaryGeneratorAction.cc
  src/SERunAction.cc 
  src/SEWatchHit.cc
  src/SEWatchSD.cc
  src/QTNMPhysicsList.cc)
target_include_directories(scattering PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(scattering PRIVATE ${Geant4_LIBRARIES})

# Test
if(BUILD_TESTING)
  add_subdirectory(test)
endif()
