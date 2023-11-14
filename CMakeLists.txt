cmake_minimum_required(VERSION 3.16)
project(loxplusplus VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
        lox++
        src/main.cpp
        src/Scanner.cpp
        src/Scanner.h
        src/Token.cpp
        src/Token.h
        src/TokenType.cpp
        src/TokenType.h
        src/Value.cpp
        src/Value.h
        src/CompilationError.cpp
        src/CompilationError.h
)