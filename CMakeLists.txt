#pragma once
cmake_minimum_required ( VERSION 3.21 FATAL_ERROR ) # TODO: expand version range

project (
	fHash
	VERSION     0.1
	DESCRIPTION "A FALK library module for hashing."
	LANGUAGES   CXX
)

configure_file ( # for exposing info to C++ (e.g. project version)
	"${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/info.hh.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/info.hh"
)

set          ( CMAKE_POSITION_INDEPENDENT_CODE     ON ) # -fPIC
set          ( CMAKE_INTERPROCEDURAL_OPTIMIZATION  ON )
cmake_policy ( SET                  CMP0069       NEW )
set          ( CMAKE_POLICY_DEFAULT_CMP0069       NEW )

add_library                ( ${PROJECT_NAME} INTERFACE )
target_compile_features    ( ${PROJECT_NAME} INTERFACE cxx_std_20 )
target_include_directories ( ${PROJECT_NAME} INTERFACE "include/" )
add_subdirectory           ( "include/${PROJECT_NAME}/" )
