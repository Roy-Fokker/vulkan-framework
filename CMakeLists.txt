cmake_minimum_required(VERSION 3.28)

# project and language to be used
project(vulkan-framework
	LANGUAGES CXX
	HOMEPAGE_URL "https://roy-fokker.github.io/vulkan-framework/"
	DESCRIPTION "C++23 based Project Template.")

# Enable scan for dependencies
set(CXX_SCAN_FOR_MODULES ON)
# disable C++ extensions
set(CMAKE_CXX_EXTENSIONS OFF)
# force C++ version to 23
set(CMAKE_CXX_STANDARD 23)
# standard is required
set(CXX_STANDARD_REQUIRED ON)

# include logic to build glsl files
include(cmake/glsl_compiler.cmake)

# shader source folder
add_subdirectory(src/shaders)

# C++ Standard library precompile source folder
add_subdirectory(src/std23)

# Vulkan framework library source folder
add_subdirectory(src/vulkan-framework)

# executable source folder
add_subdirectory(src/vfw-application)