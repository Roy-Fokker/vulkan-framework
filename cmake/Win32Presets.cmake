cmake_minimum_required(VERSION 3.29.0 FATAL_ERROR)

# Set Properties for Windows specific configurations
LIST(APPEND platform_definitions
	_CRT_SECURE_NO_WARNINGS     # Don't want MSVC's CRT warnings
	NOMINMAX                    # Don't want Window.h to define min and max macros
	WIN32_LEAN_AND_MEAN         # Windows.h should only include the basics
)

# Set Compiler flags for MSVC
# Not used atm, as preset set same options for msvc
LIST(APPEND compiler_options
	/utf-8                 # Set source and execution character sets to UTF-8
	/EHsc                  # Enable standard C++ stack unwinding, assume C code doesn't throw exceptions
	/W4                    # Enable all the warnings
	/permissive-           # Set standards conformance mode
	/Zc:__cplusplus        # Enable the __cplusplus macro to report the supported standard
	/Zc:preprocessor       # Use the new conforming preprocessor
	/Zc:throwingNew        # Assume operator new throws on failure
	/Zc:inline             # Remove unreferenced functions or data if they're COMDAT or have internal linkage only
	/Zc:externConstexpr    # Enable external linkage for constexpr variables
	/Zc:templateScope      # Enforce Standard C++ template parameter shadowing rules
	/Zc:checkGwOdr         # Enforce Standard C++ ODR violations under /Gw
	/Zc:enumTypes          # Enable Standard C++ rules for enum type deduction
)

