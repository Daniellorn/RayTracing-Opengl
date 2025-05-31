# CMakeList.txt : CMake project for ProjektGrafika, include source and define
# project specific logic here.
#

set(SOURCE_DIR "Source")

set(SOURCES
	Source/ProjektGrafika.cpp
	Source/Shader.h
	Source/Shader.cpp
	Source/vendor/stb_image/stb_image.cpp
	Source/Assert.h
)


add_executable(ProjektGrafika ${SOURCES})


target_link_libraries(ProjektGrafika glfw)
target_link_libraries(ProjektGrafika glad)
target_include_directories(ProjektGrafika PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb_image)


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/res
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
)
