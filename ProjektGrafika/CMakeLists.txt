# CMakeList.txt : CMake project for ProjektGrafika, include source and define
# project specific logic here.
#

set(SOURCE_DIR "Source")

set(SOURCES
	Source/ProjektGrafika.cpp
	Source/Shader.h
	Source/Shader.cpp
)


add_executable(ProjektGrafika ${SOURCES})


target_link_libraries(ProjektGrafika glfw)
target_link_libraries(ProjektGrafika glad)

