cmake_minimum_required(VERSION 3.20)
project(passes-assignment)

set(LT_LLVM_INSTALL_DIR "" CACHE PATH "LLVM installation directory")

set(LT_LLVM_INCLUDE_DIR "${LT_LLVM_INSTALL_DIR}/include/llvm")
if(NOT EXISTS "${LT_LLVM_INCLUDE_DIR}")
message(FATAL_ERROR
  " LT_LLVM_INSTALL_DIR (${LT_LLVM_INCLUDE_DIR}) is invalid.")
endif()

set(LT_VALID_INSTALLATION FALSE)

if(EXISTS "${LT_LLVM_INSTALL_DIR}/lib/cmake/llvm/LLVMConfig.cmake")
  set(LT_VALID_INSTALLATION TRUE)
endif()

if(EXISTS "${LT_LLVM_INSTALL_DIR}/lib64/cmake/llvm/LLVMConfig.cmake")
  set(LT_VALID_INSTALLATION TRUE)
endif()

if(NOT ${LT_VALID_INSTALLATION})
  message(FATAL_ERROR
    "LLVM installation directory, (${LT_LLVM_INSTALL_DIR}), is invalid. Couldn't
    find LLVMConfig.cmake.")
endif()

list(APPEND CMAKE_PREFIX_PATH "${LT_LLVM_INSTALL_DIR}/lib/cmake/llvm/")

find_package(LLVM CONFIG)

if("${LLVM_VERSION_MAJOR}" VERSION_LESS 17)
  message(FATAL_ERROR "Found LLVM ${LLVM_VERSION_MAJOR}, but need LLVM 17 or above")
endif()

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LT_LLVM_INSTALL_DIR}")

message("LLVM STATUS:
  Definitions ${LLVM_DEFINITIONS}
  Includes    ${LLVM_INCLUDE_DIRS}
  Libraries   ${LLVM_LIBRARY_DIRS}
  Targets     ${LLVM_TARGETS_TO_BUILD}"
)

include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

set(CMAKE_CXX_STANDARD 17 CACHE STRING "")

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE
      STRING "Build type (default Debug):" FORCE)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall\
  -fdiagnostics-color=always")

if(NOT LLVM_ENABLE_RTTI)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-fvisibility-inlines-hidden" SUPPORTS_FVISIBILITY_INLINES_HIDDEN_FLAG)
if (${SUPPORTS_FVISIBILITY_INLINES_HIDDEN_FLAG} EQUAL "1")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

add_subdirectory(HelloWorld)