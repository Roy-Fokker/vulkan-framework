# find vcpkg installed library
find_package(Vulkan REQUIRED)
find_package(glm CONFIG REQUIRED)

# set library name variable
set(PRJ_LIB_NAME "vulkan-framework")

# create library
add_library(${PRJ_LIB_NAME})

# set standard to C++ 23
target_compile_features(${PRJ_LIB_NAME} PUBLIC cxx_std_23)

# set preprocessor defines
target_compile_definitions(${PRJ_LIB_NAME}
	PUBLIC 
		UNICODE _UNICODE            # Tell compiler we are using UNICODE
		_CRT_SECURE_NO_WARNINGS     # Don't want MSVC's CRT warnings
		NOMINMAX                    # Don't want Window.h to define min and max macros
		WIN32_LEAN_AND_MEAN         # Windows.h should only include the basics
		VULKAN_HPP_NO_CONSTRUCTORS  # Vulkan.hpp should not define constructors for structs
		VK_USE_PLATFORM_WIN32_KHR   # On Windows, Vulkan needs this.
		GLM_FORCE_DEPTH_ZERO_TO_ONE # GLM clip space should be in Z-axis to 0 to 1
		GLM_FORCE_LEFT_HANDED       # GLM should use left-handed coordinates, +z goes into screen
)

# precompile headers for this library
target_precompile_headers(${PRJ_LIB_NAME} 
	PUBLIC
		pch.hpp
)

# source files for this app
target_sources(${PRJ_LIB_NAME}
	PUBLIC FILE_SET vfw_modules TYPE CXX_MODULES FILES
		vulkan-framework.ixx
		renderer.ixx
		instance.ixx
		device.ixx
		swap_chain.ixx
		pipeline.ixx
)

# libraries used by vke
target_link_libraries(${PRJ_LIB_NAME}
	PRIVATE
		std23
		Vulkan::Vulkan
		glm::glm
)
