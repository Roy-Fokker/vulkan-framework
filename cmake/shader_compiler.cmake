# Compile GLSL and HLSL files into SPIR-V files
# Depends on glslc and dxc installed by LunarG SDK

# Usage: 
# target_shader_sources(<target>
# 	[GLSL
# 		<glsl_shader_file>
# 		...
# 	]
# 	[HLSL
# 		<hlsl_shader_file> : <hlsl_shader_profile>
# 		...
# 	]
# )

cmake_minimum_required(VERSION 3.29.0 FATAL_ERROR)

include(cmake/glsl_compiler.cmake)
include(cmake/hlsl_compiler.cmake)

function(target_shader_sources TARGET)
	cmake_parse_arguments(arg "" "" "HLSL;GLSL" ${ARGN})

	list(LENGTH arg_GLSL glsl_count)
	if(glsl_count GREATER 0)
		target_glsl_sources(${TARGET} ${arg_GLSL})
	endif()

	list(LENGTH arg_HLSL hlsl_count)
	if(hlsl_count GREATER 0)
		target_hlsl_sources(${TARGET} ${arg_HLSL})
	endif()
	
endfunction()
