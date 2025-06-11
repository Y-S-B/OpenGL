cmake_minimum_required(VERSION 3.10)
project(TicTacToe)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# For Windows, we need to link against some additional libraries
if(WIN32)
    set(EXTRA_LIBS opengl32)
endif()

# Include glad.c so it actually gets compiled
add_executable(tictactoe
    src/main.cpp
    src/glad.c
)

# Make sure we tell CMake about BOTH include/glad and include/GLFW
target_include_directories(tictactoe PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Link libraries: 
#  1) The static GLFW library in /lib 
#  2) OpenGL libraries
target_link_libraries(tictactoe PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/libglfw3.a"
    ${EXTRA_LIBS}

)

