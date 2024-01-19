# set library name variable
set(PRJ_LIB_NAME "shaders")

# create library
add_library(${PRJ_LIB_NAME} INTERFACE)

# shaders to be used, 
# must include "cmake/glsl_compiler.cmake" before calling
target_shader_sources(${PRJ_LIB_NAME}
	# simple shader vertex list set by shader
	simple_shader.frag 
	simple_shader.vert
	# basic shader vertex list set by application
	basic_shader.frag
	basic_shader.vert
)
