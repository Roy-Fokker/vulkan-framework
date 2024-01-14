# find vcpkg installed library
# find_package(fmt CONFIG REQUIRED)

# set application name variable
set(PRJ_EXE_NAME "vfw-application")

# create executable
add_executable(${PRJ_EXE_NAME})

# ensure it's Standard C++ 20
target_compile_features(${PRJ_EXE_NAME} PRIVATE cxx_std_23)

# set preprocessor defines
target_compile_definitions(${PRJ_EXE_NAME}
	PRIVATE 
		UNICODE _UNICODE
		_CRT_SECURE_NO_WARNINGS
		NOMINMAX 
		WIN32_LEAN_AND_MEAN
)

# source files for this application
target_sources(${PRJ_EXE_NAME}
	PRIVATE
		main.cpp

	PRIVATE FILE_SET app_modules TYPE CXX_MODULES FILES
		input.ixx
		window.ixx
		application.ixx)

# libraries used by this application
target_link_libraries(${PRJ_EXE_NAME}
	PRIVATE
		std23
		vulkan-framework)