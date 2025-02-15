cmake_minimum_required(VERSION 3.5)
project(
  baobzi
  LANGUAGES C CXX Fortran)

set(CMAKE_CXX_STANDARD 17)

get_property(gen_multi GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT gen_multi AND NOT (CMAKE_BUILD_TYPE OR DEFINED ENV{CMAKE_BUILD_TYPE}))
  set(CMAKE_BUILD_TYPE Release CACHE STRING "")
endif()


find_package(OpenMP REQUIRED)

set(BAOBZI_INCLUDES
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/extern/msgpack-c/include
  ${PROJECT_SOURCE_DIR}/extern/eigen
  ${PROJECT_SOURCE_DIR}/extern/catch2/src
  )

option(BAOBZI_BUILD_STATIC "Build the static library" OFF)
option(BAOBZI_BUILD_SHARED "Build the shared library" ON)
option(BAOBZI_BUILD_EXAMPLES "Build C/C++ examples" ON)
option(BAOBZI_BUILD_TESTS "Build tests" ON)
option(BAOBZI_BUILD_MATLAB "Build MATLAB bindings" OFF)
option(BAOBZI_BUILD_FORTRAN "Build FORTRAN bindings" ON)
# option(BAOBZI_BUILD_AVX "Build AVX support" ON)
# option(BAOBZI_BUILD_AVX2 "Build AVX2 support" ON)
# option(BAOBZI_BUILD_AVX512 "Build AVX512 support" ON)


file(GLOB LIB_SOURCES_GENERIC "src/*0.cpp")
add_library(baobzi_generic OBJECT ${LIB_SOURCES_GENERIC})
target_include_directories(baobzi_generic PUBLIC ${BAOBZI_INCLUDES})
target_compile_options(baobzi_generic PRIVATE -fopenmp)
set_property(TARGET baobzi_generic PROPERTY POSITION_INDEPENDENT_CODE ON)

file(GLOB LIB_SOURCES_AVX "src/*1.cpp")
add_library(baobzi_avx OBJECT ${LIB_SOURCES_AVX})
target_include_directories(baobzi_avx PUBLIC ${BAOBZI_INCLUDES})
target_compile_options(baobzi_avx PRIVATE -mavx -fopenmp)
set_property(TARGET baobzi_avx PROPERTY POSITION_INDEPENDENT_CODE ON)

file(GLOB LIB_SOURCES_AVX2 "src/*2.cpp")
add_library(baobzi_avx2 OBJECT ${LIB_SOURCES_AVX2})
target_include_directories(baobzi_avx2 PUBLIC ${BAOBZI_INCLUDES})
target_compile_options(baobzi_avx2 PRIVATE -mavx2 -mfma -fopenmp)
set_property(TARGET baobzi_avx2 PROPERTY POSITION_INDEPENDENT_CODE ON)

file(GLOB LIB_SOURCES_AVX512 "src/*3.cpp")
add_library(baobzi_avx512 OBJECT ${LIB_SOURCES_AVX512} )
target_include_directories(baobzi_avx512 PUBLIC ${BAOBZI_INCLUDES})
target_compile_options(baobzi_avx512 PRIVATE -mavx512f -mfma -fopenmp)
set_property(TARGET baobzi_avx512 PROPERTY POSITION_INDEPENDENT_CODE ON)

set(BAOBZI_BINDING_SOURCES src/baobzi.cpp)

if (BAOBZI_BUILD_FORTRAN)
  list(APPEND BAOBZI_BINDING_SOURCES src/baobzi.f90)
endif()

if (BAOBZI_BUILD_SHARED)
  add_library(baobzi SHARED ${BAOBZI_BINDING_SOURCES})
  target_include_directories(baobzi PUBLIC ${BAOBZI_INCLUDES})
  target_link_libraries(baobzi PRIVATE baobzi_generic baobzi_avx baobzi_avx2 baobzi_avx512 OpenMP::OpenMP_CXX)
  list(APPEND INSTALL_TARGETS baobzi)
endif()

if (BAOBZI_BUILD_STATIC)
  add_library(baobzi_static STATIC ${BAOBZI_BINDING_SOURCES})
  target_link_libraries(baobzi_static PRIVATE baobzi_generic baobzi_avx baobzi_avx2 baobzi_avx512 OpenMP::OpenMP_CXX)
  set_target_properties(baobzi_static PROPERTIES OUTPUT_NAME baobzi)
  list(APPEND INSTALL_TARGETS baobzi_static)
endif()

if (${BAOBZI_BUILD_SHARED} AND ${BAOBZI_BUILD_EXAMPLES})
  set(EXAMPLE_SOURCE_CPP "examples/c++/baobzi_timing.cpp")
  add_executable(baobzi_timing_cpp ${EXAMPLE_SOURCE_CPP})
  target_include_directories(baobzi_timing_cpp PUBLIC ${BAOBZI_INCLUDES})
  target_link_libraries(baobzi_timing_cpp PUBLIC OpenMP::OpenMP_CXX)
  target_compile_options(baobzi_timing_cpp PUBLIC -march=native)

  set(EXAMPLE_SOURCE_C "examples/C/baobzi_timing.c")
  add_executable(baobzi_timing_c ${EXAMPLE_SOURCE_C})
  target_include_directories(baobzi_timing_c PUBLIC ${BAOBZI_INCLUDES})
  target_link_libraries(baobzi_timing_c PUBLIC baobzi OpenMP::OpenMP_CXX m)
endif()

if (BAOBZI_BUILD_TESTS)
  add_subdirectory(extern/catch2)

  add_executable(test_c tests/test_c.cpp)
  target_link_libraries(test_c PUBLIC Catch2::Catch2WithMain baobzi OpenMP::OpenMP_CXX)

  list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/extern/catch2/extras)
  include(CTest)
  include(Catch)
  catch_discover_tests(test_c)
endif()

if (BAOBZI_BUILD_MATLAB)
  find_package(Matlab REQUIRED)
  matlab_add_mex(NAME baobzi_mex SRC src/matlab/baobzi_mex.cpp LINK_TO baobzi)
  target_include_directories(baobzi_mex PUBLIC ${BAOBZI_INCLUDES})
  install(FILES ${PROJECT_SOURCE_DIR}/src/matlab/baobzi.m
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/baobzi/matlab)
  install(TARGETS baobzi_mex DESTINATION ${CMAKE_INSTALL_PREFIX}/share/baobzi/matlab)
endif()

include(GNUInstallDirs)
install(TARGETS ${INSTALL_TARGETS})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES ${PROJECT_SOURCE_DIR}/src/julia/baobzi.jl
  DESTINATION ${CMAKE_INSTALL_PREFIX}/share/baobzi/julia)
install(FILES ${PROJECT_SOURCE_DIR}/src/python/baobzi.py
  DESTINATION ${CMAKE_INSTALL_PREFIX}/share/baobzi/python)
install(FILES ${PROJECT_SOURCE_DIR}/src/baobzi.f90
  ${CMAKE_CURRENT_BINARY_DIR}/baobzi.mod
  DESTINATION ${CMAKE_INSTALL_PREFIX}/share/baobzi/fortran)
