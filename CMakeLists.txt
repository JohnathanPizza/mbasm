cmake_minimum_required(VERSION 3.19)

project(mbasm)

include_directories(${CMAKE_SOURCE_DIR}/inc)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_executable(mbasm
	${CMAKE_SOURCE_DIR}/src/main.c
	${CMAKE_SOURCE_DIR}/src/ins.c
	${CMAKE_SOURCE_DIR}/src/command.c
	${CMAKE_SOURCE_DIR}/src/utility.c
	${CMAKE_SOURCE_DIR}/src/list.c
	${CMAKE_SOURCE_DIR}/src/error.c
)
