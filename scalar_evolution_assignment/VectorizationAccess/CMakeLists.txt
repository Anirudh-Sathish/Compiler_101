cmake_minimum_required(VERSION 3.20)
project(VectorizationAccess_Implementation-VectorizationAccess)


set(LT_LLVM_INSTALL_DIR "" CACHE PATH "LLVM installation directory")


list(APPEND CMAKE_PREFIX_PATH "${LT_LLVM_INSTALL_DIR}/lib/cmake/llvm/")

find_package(LLVM CONFIG)
if("${LLVM_VERSION_MAJOR}" VERSION_LESS 17)
  message(FATAL_ERROR "Found LLVM ${LLVM_VERSION_MAJOR}, but need LLVM 17 or above")
endif()

include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 17 CACHE STRING "")

if(NOT LLVM_ENABLE_RTTI)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

add_library(VectorizationAccess SHARED VectorizationAccess.cpp)

target_link_libraries(VectorizationAccess
  "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>")