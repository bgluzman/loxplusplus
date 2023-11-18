cmake_minimum_required(VERSION 3.16)
project(loxplusplus VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
add_definitions(${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(llvm_libs support core)

add_executable(
        lox++
        src/main.cpp
        src/Scanner.cpp
        src/Scanner.h
        src/Token.cpp
        src/Token.h
        src/TokenType.cpp
        src/TokenType.h
        src/Literal.cpp
        src/Literal.h
        src/CompilationError.cpp
        src/CompilationError.h
        src/Ast.cpp
        src/Ast.h
        src/Parser.cpp
        src/Parser.h
)
target_include_directories(lox++ PRIVATE ${LLVM_INCLUDE_DIRS})
target_link_libraries(lox++ PRIVATE ${llvm_libs})