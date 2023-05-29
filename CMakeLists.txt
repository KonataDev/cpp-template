cmake_minimum_required(VERSION 3.24)
#
# Project details
#
project(
  cpp_template
  VERSION 0.0.1
  LANGUAGES CXX)

#
# Model project dependencies
#
find_package(stdsharp CONFIG REQUIRED)

include(stdsharp/Utils)

config_lib(${PROJECT_NAME} INTERFACE STD 23)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_compile_options(
  ${PROJECT_NAME} INTERFACE $<$<CXX_COMPILER_ID:MSVC>: /external:anglebrackets>
                            $<$<CXX_COMPILER_ID:Clang>: -fexperimental-library>)

target_link_options(${PROJECT_NAME} INTERFACE $<$<CXX_COMPILER_ID:Clang>:
                    -lc++experimental>)

target_link_libraries(${PROJECT_NAME} INTERFACE stdsharp::stdsharp)

verbose_message("Successfully added all dependencies and linked against them.")

target_install(
  ${PROJECT_NAME}
  NAMESPACE
  ${PROJECT_NAME}
  COMPATIBILITY
  SameMajorVersion
  DEPENDENCIES
  stdsharp)

get_directory_property(is_subproject PARENT_DIRECTORY)

if(NOT is_subproject)
  set(is_standalone YES)
else()
  set(is_standalone NO)
endif()

include(CMakeDependentOption)

cmake_dependent_option(${PROJECT_NAME}_BUILD_TEST "Whether to build test" ON
                       "${is_standalone}" OFF)
cmake_dependent_option(
  ${PROJECT_NAME}_INCLUDE_AS_SYSTEM
  "Whether to include ${PROJECT_NAME} headers as system headers" OFF
  "${is_standalone}" ON)

if(${PROJECT_NAME}_INCLUDE_AS_SYSTEM)
  target_include_as_system(${PROJECT_NAME} INTERFACE)
endif()

if(${PROJECT_NAME}_BUILD_TEST)
  include(CTest)
  add_subdirectory(tests)
endif()
