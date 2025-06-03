# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.28)
include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

project ("ProjektGrafika")


# glfw
find_package(glfw3 3.4 QUIET)
if (NOT glfw3_FOUND)
	FetchContent_Declare(
		glfw3
		DOWNLOAD_EXTRACT_TIMESTAMP OFF
		URL https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip
	)
	FetchContent_GetProperties(glfw3)
	if (NOT glfw3_POPULATED)
		set (FETCHCONTENT_QUIET NO)
		FetchContent_Populate(glfw3)
		add_subdirectory(${glfw3_SOURCE_DIR} ${glfw3_BINARY_DIR})
	endif()
endif()

# opengl
find_package(OpenGL REQUIRED)

# glad
FetchContent_Declare(
	glad 
	DOWNLOAD_EXTRACT_TIMESTAMP OFF
	URL https://github.com/Dav1dde/glad/archive/refs/tags/v2.0.8.zip
)

FetchContent_GetProperties(glad)
if (NOT glad_POPULATED)
	set(FETCHCONTENT_QUIET NO)
	FetchContent_MakeAvailable(glad)

	add_subdirectory("${glad_SOURCE_DIR}/cmake" glad_cmake)
	glad_add_library(glad REPRODUCIBLE EXCLUDE_FROM_ALL LOADER API gl:core=4.6)
endif()

set_target_properties(glad PROPERTIES FOLDER "Dependencies")

# glm
FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
)
FetchContent_MakeAvailable(glm)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


# Include sub-projects.
add_subdirectory ("ProjektGrafika")
