# Compile HLSL shaders into SPIR-V files
# depends on dxc installed by LunarG SDK

# Usage: target_shader_sources(<target> [[<file> : <profile>]...])
function(target_hlsl_sources TARGET)
	set (VULKAN_COMPONENTS "dxc" "dxc_exe")
	find_package(Vulkan REQUIRED COMPONENTS ${VULKAN_COMPONENTS})

	if (NOT TARGET Vulkan::dxc_exe)
		message(FATAL_ERROR "[Error]: Could not find dxc")
	endif()

	message("Found DXC executable: ${Vulkan_dxc_EXECUTABLE}")
	set(dxc_exe ${Vulkan_dxc_EXECUTABLE})

	# figure out how many files we have to configure given the pattern
	list(LENGTH ARGN count_HLSL)
	math(EXPR count_HLSL "${count_HLSL} / 3")

	# # List of profile names supported by LunarG supplied dxc
	# set(valid_profiles 
	# 	ps_6_0 ps_6_1 ps_6_2 ps_6_3 ps_6_4 ps_6_5 ps_6_6 ps_6_7 ps_6_8 
	# 	vs_6_0 vs_6_1 vs_6_2 vs_6_3 vs_6_4 vs_6_5 vs_6_6 vs_6_7 vs_6_8 
	# 	gs_6_0 gs_6_1 gs_6_2 gs_6_3 gs_6_4 gs_6_5 gs_6_6 gs_6_7 gs_6_8 
	# 	hs_6_0 hs_6_1 hs_6_2 hs_6_3 hs_6_4 hs_6_5 hs_6_6 hs_6_7 hs_6_8 
	# 	ds_6_0 ds_6_1 ds_6_2 ds_6_3 ds_6_4 ds_6_5 ds_6_6 ds_6_7 ds_6_8 
	# 	cs_6_0 cs_6_1 cs_6_2 cs_6_3 cs_6_4 cs_6_5 cs_6_6 cs_6_7 cs_6_8 
	# 	lib_6_1 lib_6_2 lib_6_3 lib_6_4 lib_6_5 lib_6_6 lib_6_7 lib_6_8 
	# 	ms_6_5 ms_6_6 ms_6_7 ms_6_8 
	# 	as_6_5 as_6_6 as_6_7 as_6_8
	# )

	# Loop through all the pairs for filename:profile provided
	foreach(i RANGE 1 ${count_HLSL})
		math(EXPR fni "(${i}-1)*3")             # filename index
		math(EXPR pfi "${fni}+2")                # profile index
		list(GET arg_HLSL ${fni} hlsl_filename)
		list(GET arg_HLSL ${pfi} hlsl_profile)

		# # check the profile name matches the above list.
		# list(FIND valid_profiles ${hlsl_profile} found_profile)
		# if(found_profile EQUAL -1)
		# 	message(FATAL_ERROR "Profile ${hlsl_profile} for ${hlsl_filename} is not recognized.")
		# endif()

		# get the absolute path of current source file
		file(REAL_PATH ${hlsl_filename} source_abs)

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
			message(FATAL_ERROR "Cannot file shader file: ${source_abs}")
		endif()

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
