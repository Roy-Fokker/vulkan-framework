# set library name variable
set(PRJ_SHDR_LIB_NAME "shaders")

# create library
add_library(${PRJ_SHDR_LIB_NAME} INTERFACE)

# shaders to be used, 
# must include "cmake/shader_compiler.cmake" before calling
target_shader_sources(${PRJ_SHDR_LIB_NAME}
	GLSL
		# simple shader vertex list set by shader
		simple_shader.frag 
		simple_shader.vert
		# basic shader vertex list set by application
		basic_shader.frag
		basic_shader.vert
		basic_pc_shader.frag
		basic_pc_shader.vert
	HLSL
		basic_compute.hlsl : cs_6_4
)
