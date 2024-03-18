cmake_minimum_required(VERSION 3.12)
project(cd109source VERSION 0.1.0)

# Compiler/build settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Dependencies
find_package(Geant4 11.2 REQUIRED)

# Build
add_executable(cd109source
  cd109.cc
  src/CDActionInitialization.cc
  src/CDDetectorConstruction.cc
  src/CDGasHit.cc
  src/CDGasSD.cc
  src/CDEventAction.cc
  src/CDPrimaryGeneratorAction.cc
  src/CDRunAction.cc)
target_include_directories(cd109source PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(cd109source PRIVATE ${Geant4_LIBRARIES})

