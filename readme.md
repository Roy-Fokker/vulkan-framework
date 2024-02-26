# Vulkan Framework for toy projects
based on C++23 Project Template

## Uses
- vcpkg for package management
- ninja for build engine
- cmake with cmakepresets for build generator
- clang-format and clang-tidy, for basic formatting and static analysis
- C++ Modules, where possible

## to be figured out
- enabling standard module
- currently it's win32 only


## project dependencies installed via vcpkg
- vulkan
- glm

## debugging and executing 
- should be run from .\bin directory.
- can be configured via launch.json and settings.json
  launch.json
  ```json
	"program": "${command:cmake.launchTargetPath}",
	"cwd": "${workspaceFolder}/bin",
	"environment": [
		{
			"name": "PATH",
			"value": "${env:PATH}:${command:cmake.getLaunchTargetDirectory}"
		}
	],
  ```
  settings.json
  ```json 
	"cmake.debugConfig": {
		"cwd": "${workspaceFolder}/bin",
		"console": "integratedTerminal"
	}
  ```
- MS CMake-Tools has new mechanism to do above. Documentation is bit obtuse, so need to parse it.