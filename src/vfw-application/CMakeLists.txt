# find vcpkg installed library
find_package(Vulkan REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

# set application name variable
set(PRJ_EXE_NAME "vfw-application")

# create executable
add_executable(${PRJ_EXE_NAME})

# ensure it's Standard C++ 20
target_compile_features(${PRJ_EXE_NAME} PRIVATE cxx_std_23)

# set preprocessor defines
target_compile_definitions(${PRJ_EXE_NAME}
	PRIVATE 
		UNICODE _UNICODE            # Tell compiler we are using UNICODE
		_CRT_SECURE_NO_WARNINGS     # Don't want MSVC's CRT warnings
		NOMINMAX                    # Don't want Window.h to define min and max macros
		WIN32_LEAN_AND_MEAN         # Windows.h should only include the basics
		VULKAN_HPP_NO_CONSTRUCTORS  # Vulkan.hpp should not define constructors for structs
		VK_USE_PLATFORM_WIN32_KHR   # On Windows, Vulkan needs this.
		GLM_FORCE_DEPTH_ZERO_TO_ONE # GLM clip space should be in Z-axis to 0 to 1
		GLM_FORCE_LEFT_HANDED       # GLM should use left-handed coordinates, +z goes into screen
		GLM_FORCE_RADIANS           # GLM should always use radians not degrees.
)

# source files for this application
target_sources(${PRJ_EXE_NAME}
	PRIVATE
		main.cpp

	# magic encantations for C++ Modules Support in CMake
	PRIVATE FILE_SET app_modules TYPE CXX_MODULES FILES
		clock.ixx
		input.ixx
		window.ixx
		application.ixx
)

# libraries used by this application
target_link_libraries(${PRJ_EXE_NAME}
	PRIVATE
		std23             # precompiled header for C++ 23 Standard Libary
		vulkan-framework  # our vulkan framework
		shaders           # glsl/hlsl shaders to use
		Vulkan::Vulkan    # Vulkan SDK
		glm::glm          # glm library
		fmt::fmt          # fmt library
)

# precompiled headers 
target_precompile_headers(${PRJ_EXE_NAME}
	REUSE_FROM
		vulkan-framework  # reuse the PCH file from this project
)