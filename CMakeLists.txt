cmake_minimum_required(VERSION 3.25)

project(pet LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
  add_definitions(-DNDEBUG)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-overflow=5 -Wwrite-strings -Wcast-qual -Wswitch-default -Wconversion -Wunreachable-code -Wno-unused-variable -Wno-unused-function -Wno-unused-local-typedefs")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Og -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_library(pet-lib STATIC
	src/toolkit/StringPool.cpp
	src/toolkit/StringUtils.cpp

	src/pet/parser/ConstantFolder.cpp
	src/pet/parser/Lexer.cpp
	src/pet/parser/Parser.cpp
	src/pet/parser/Token.cpp

	src/pet/runtime/Globals.cpp
	src/pet/runtime/Interpreter.cpp

	src/pet/runtime/Array.cpp
	src/pet/runtime/Dictionary.cpp
	src/pet/runtime/Value.cpp
	
	src/pet/Expression.cpp
	src/pet/Location.cpp
	src/pet/Script.cpp
	src/pet/Statement.cpp)

target_include_directories(pet-lib PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src
	thirdparty/magic_enum/include)

add_executable(pet src/main.cpp)
target_link_libraries(pet PRIVATE pet-lib)

install(TARGETS pet DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)
