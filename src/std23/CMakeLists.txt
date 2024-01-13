# set library name variable
set(PRJ_LIB_NAME "std23")

# create library
add_library(${PRJ_LIB_NAME} INTERFACE)

# set standard to C++ 23
target_compile_features(${PRJ_LIB_NAME} INTERFACE cxx_std_23)

# set preprocessor defines
target_compile_definitions(${PRJ_LIB_NAME}
	INTERFACE 
		UNICODE _UNICODE            # Tell compiler we are using UNICODE
		_CRT_SECURE_NO_WARNINGS     # Don't want MSVC's CRT warnings
		NOMINMAX                    # Don't want Window.h to define min and max macros
		WIN32_LEAN_AND_MEAN         # Windows.h should only include the basics
)

# ensure cmake knows which headers to include
target_include_directories(${PRJ_LIB_NAME}
	INTERFACE .
)

# set precompile headers interface for std/os stuff
# gets imported silently in caller target
target_precompile_headers(${PRJ_LIB_NAME}
	INTERFACE
		std.hpp
)
