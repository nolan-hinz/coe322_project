cmake_minimum_required( VERSION 3.12 )
project( final_project
  DESCRIPTION "COE 322 Final Project, Written by Nolan Hinz and Ethan Harpuder (jh76769 and ehh589)"
  VERSION 1.0 )

add_executable( final_project main.cpp )
target_compile_features( final_project PRIVATE cxx_std_23 )

find_package( PkgConfig REQUIRED )
pkg_check_modules( OPTS REQUIRED cxxopts )
message( STATUS "Cxxopts includes: ${OPTS_INCLUDE_DIRS}" )

target_include_directories(
        final_project PUBLIC
        ${OPTS_INCLUDE_DIRS}
	)

install( TARGETS final_project DESTINATION . )
