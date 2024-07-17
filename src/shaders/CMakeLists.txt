# include logic to build glsl files
include(${CMAKE_MODULE_PATH}/shader_compiler.cmake)

# set library name variable
set(PRJ_SHDR_LIB_NAME "shaders")

# create library
add_library(${PRJ_SHDR_LIB_NAME} INTERFACE)

# shaders to be used, 
# must include "cmake/shader_compiler.cmake" before calling
target_shader_sources(${PRJ_SHDR_LIB_NAME}
	GLSL
		basic_compute.comp
		push_const_compute.comp
		basic_triangle.vert
		basic_triangle.frag
	HLSL
		basic_compute.hlsl : cs_6_4
		push_const_compute.hlsl : cs_6_4
		basic_triangle.vs.hlsl : vs_6_4
		basic_triangle.ps.hlsl : ps_6_4
)
