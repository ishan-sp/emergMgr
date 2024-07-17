# Specify the minimum version of CMake required
cmake_minimum_required(VERSION 3.15)

# Set the name of the project
project(rvceERMS)

# Add an executable target with the given name and source file
add_executable(myexample HttpServer.cpp Application.cpp main.cpp)
