cmake_minimum_required(VERSION 3.20)
project(pesource VERSION 0.1.0)

# Compiler/build settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Dependencies
find_package(Geant4 11.2 REQUIRED)

# Build
add_executable(pesource
  pesource.cc
  src/PEActionInitialization.cc
  src/PEDetectorConstruction.cc
  src/PEGasHit.cc
  src/PEGasSD.cc
  src/PEEventAction.cc
  src/PEPrimaryGeneratorAction.cc
  src/PERunAction.cc)
target_include_directories(pesource PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(pesource PRIVATE ${Geant4_LIBRARIES})

