cmake_minimum_required ( VERSION 3.21 FATAL_ERROR )

project (
	fHash
	VERSION     0.1.0
	DESCRIPTION "A FALK library module for hashing."
	LANGUAGES   CXX
)

set          ( CMAKE_POSITION_INDEPENDENT_CODE     ON ) # -fPIC
set          ( CMAKE_INTERPROCEDURAL_OPTIMIZATION  ON )
cmake_policy ( SET                  CMP0069       NEW )
set          ( CMAKE_POLICY_DEFAULT_CMP0069       NEW )

set ( HEADERS core.h info.h )

add_library                ( ${PROJECT_NAME} INTERFACE )
target_sources             ( ${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/${HEADERS} )
target_compile_features    ( ${PROJECT_NAME} INTERFACE cxx_std_23 )
target_include_directories ( ${PROJECT_NAME} INTERFACE include/ )

configure_file ( # for exposing info to C++ (e.g. project version)
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/info.h.in
	${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/info.h
)

# TODO: include guard
