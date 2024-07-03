# Compile GLSL and HLSL files into SPIR-V files
# Depends on glslc and dxc installed by LunarG SDK

cmake_minimum_required(VERSION 3.29.0 FATAL_ERROR)

# exit if called multiple times.
if(ShaderCompiler_FOUND)
	return()
endif()

# include cmake files for each shader file type
include(${PROJECT_SOURCE_DIR}/cmake/glsl_compiler.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/hlsl_compiler.cmake)

# define the shader source function
function(target_shader_sources TARGET)
	cmake_parse_arguments(arg "" "" "HLSL;GLSL" ${ARGN})  # Parse the arguments into GLSL and HLSL groups

	list(LENGTH arg_GLSL glsl_count) # Count the glsl files
	# if count is greater than 0, hand off the glsl_compiler.cmake module 
	if(glsl_count GREATER 0)
		target_glsl_sources(${TARGET} ${arg_GLSL})
	endif()

	list(LENGTH arg_HLSL hlsl_count) # Count the hlsl files
	# if count is greater than 0, hand off the hlsl_compiler.cmake module
	if(hlsl_count GREATER 0)
		target_hlsl_sources(${TARGET} ${arg_HLSL})
	endif()
endfunction()

if (NOT ShaderCompiler_FOUND)
	# usage message
	message("Shader Compiler found.
	Usage: 
		target_shader_sources(<target>
			[GLSL
				<glsl_shader_file>
				...
			]
			[HLSL
				<hlsl_shader_file> : <hlsl_shader_profile>
				...
			]
		)

	Output:
		${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${shader_file_fldr}/<shader_file>.spv
		...
	")
endif()

# add Cache variable to check if find_package was already called once
set(ShaderCompiler_FOUND true)
