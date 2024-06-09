# Compile glsl files into SPIR-V files
# depends on glslc installed by LunarG SDK

# Usage: target_glsl_sources(<target> [<file> ...])
function (target_glsl_sources TARGET)
	find_package(Vulkan REQUIRED)
	if (NOT TARGET Vulkan::glslc)
		message(FATAL_ERROR "[Error]: Could not find glslc.")
	endif()

	foreach(source IN LISTS ARGN)
		# get the absolute path of current source file
		file(REAL_PATH ${source} source_abs)

		# get only the filename from absolute path
		cmake_path(GET source_abs FILENAME basename)

		# get only the parent directory of the file from absolute path
		cmake_path(GET source_abs PARENT_PATH source_fldr)
		get_filename_component(source_fldr "${source_fldr}" NAME)
		
		# append source shader's parent directory name to output directory path
		set(shader_dir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${source_fldr})

		# full path to compiled output 
		set(output ${shader_dir}/${basename}.spv)
		
		if(NOT EXISTS ${source_abs})
			message(FATAL_ERROR "Cannot file shader file: ${source}")
		endif()

		# call vulkan sdk's glslc compiler with source and output arguments.
		add_custom_command(
			OUTPUT ${output}
			COMMAND ${CMAKE_COMMAND} -E make_directory ${shader_dir}
			COMMAND Vulkan::glslc ${source_abs} -o ${output}
			DEPENDS ${source_abs}
			COMMENT "Compiling SPIRV: ${source} -> ${output}"
			VERBATIM
		)

		# make a new variable to hold dependency target name
		set(shader_target "${TARGET}_${basename}")
		# add custom target using new variable bound to output file of glslc step
		add_custom_target("${shader_target}"
		                  DEPENDS "${output}")
		# add compilation of this shader as dependency of the target
		add_dependencies("${TARGET}" "${shader_target}")
	endforeach()

endfunction()