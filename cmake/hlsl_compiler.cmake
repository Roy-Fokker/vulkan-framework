# Compile HLSL shaders into SPIR-V files
# depends on dxc installed by LunarG SDK

# Usage: target_shader_sources(<target> [[<file> : <profile>]...])
function(target_hlsl_sources TARGET)
	# Componenets we need CMake to ensure exist
	set (VULKAN_COMPONENTS "dxc" "dxc_exe")
	# look for above components in Vulkan SDK
	find_package(Vulkan REQUIRED COMPONENTS ${VULKAN_COMPONENTS})

	if (NOT TARGET Vulkan::dxc_exe)
		message(FATAL_ERROR "[Error]: Could not find dxc")
	endif()

	# figure out how many files we have to configure given the pattern
	list(LENGTH ARGN count_HLSL)
	math(EXPR count_HLSL "${count_HLSL} / 3")

	# Loop through all the pairs for filename:profile provided
	foreach(i RANGE 1 ${count_HLSL})
		math(EXPR fni "(${i}-1)*3")             # filename index
		math(EXPR pfi "${fni}+2")                # profile index
		list(GET arg_HLSL ${fni} hlsl_filename)
		list(GET arg_HLSL ${pfi} hlsl_profile)

		# get the absolute path of current source file
		file(REAL_PATH ${hlsl_filename} source_abs)

		if(NOT EXISTS ${source_abs})
			message(FATAL_ERROR "Cannot file shader file: ${source_abs}")
		endif()

		# get only the filename from absolute path
		cmake_path(GET source_abs FILENAME basename)

		# get only the parent directory of the file from absolute path
		cmake_path(GET source_abs PARENT_PATH source_fldr)
		get_filename_component(source_fldr "${source_fldr}" NAME)
		
		# append source shader's parent directory name to output directory path
		set(shader_dir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${source_fldr})

		# full path to compiled output 
		set(output ${shader_dir}/${basename}.spv)

		# call vulkan sdk's dxc compiler with source and output arguments.
		add_custom_command(
			OUTPUT ${output}
			COMMAND ${CMAKE_COMMAND} -E make_directory ${shader_dir}
			COMMAND Vulkan::dxc_exe -spirv -E main -Fo ${output} -T ${hlsl_profile} ${source_abs}
			DEPENDS ${source_abs}
			COMMENT "Compiling SPIRV: ${source} -> ${output}"
			VERBATIM
		)

		# make a new variable to hold dependency target name
		set(shader_target "${TARGET}_${basename}")
		# add custom target using new variable bound to output file of dxc step
		add_custom_target("${shader_target}"
		                  DEPENDS "${output}")
		# add compilation of this shader as dependency of the target
		add_dependencies("${TARGET}" "${shader_target}")

	endforeach()
endfunction()
