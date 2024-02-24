# Compile hlsl files into SPIR-V files
# depends on dxc installed by LunarG SDK

# Usage: target_shader_sources(<target> [<file> ...])
function(target_shader_sources TARGET)
	find_package(Vulkan REQUIRED)
	if (NOT TARGET Vulkan::dxc)
		message(FATAL_ERROR "[Error]: Could not find dxc.")
	endif()
endfunction()
