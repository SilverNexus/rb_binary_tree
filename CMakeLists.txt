cmake_minimum_required(VERSION 2.8)
project(rbtree)

# Set flags to C++98
IF(UNIX)
    add_compile_options(-Wall -Wextra -std=c++98)
ELSEIF(MSVC)
    # I think it will handle C++98 as-is.
    # Also, all warnings gives warnings from provided header files, and buries relevant errors.
    # Instead, use the next step down of warnings.
    add_compile_options(/W3)
ENDIF()
#Otherwise, we assume defaults use C++98 or newer.

add_executable(rbtree_app main.cpp node.h rbtree.h)
