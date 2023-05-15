cmake_minimum_required(VERSION 3.12)
project(MagneticFields VERSION 0.1.0)

# Compiler/build settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# CTest if we need it
include(CTest)

# Dependencies
find_package(Geant4 11.1 REQUIRED)

# Look for Boost
#TODO This should be a required package if using clang
find_package(Boost)

if(Boost_FOUND)
  add_compile_definitions(HAVE_BOOST)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

# Build
add_executable(boris
  boris.cc
  src/MFActionInitialization.cc
  src/MFDetectorConstruction.cc
  src/MFGasHit.cc
  src/MFGasSD.cc
  src/MFEquationOfMotion.cc
  src/MFEventAction.cc
  src/MFMagneticFieldSetup.cc
  src/MFFieldMessenger.cc
  src/MFMagneticTrap.cc
  src/MFPrimaryGeneratorAction.cc
  src/MFRunAction.cc
  src/MFWatchHit.cc
  src/MFWatchSD.cc
  src/BorisStepper.cc
  src/QTNMPhysicsList.cc)
target_include_directories(boris PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(boris PRIVATE ${Geant4_LIBRARIES})

# Test
if(BUILD_TESTING)
  add_subdirectory(test)
endif()
