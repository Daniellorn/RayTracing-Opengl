# CMakeList.txt : CMake project for ProjektGrafika, include source and define
# project specific logic here.
#

set(SOURCE_DIR "Source")
set(IMGUI_PATH "${SOURCE_DIR}/vendor/ImGui/imgui")

set(SOURCES
	${SOURCE_DIR}/ProjektGrafika.cpp
	${SOURCE_DIR}/Shader.h
	${SOURCE_DIR}/Shader.cpp
	${SOURCE_DIR}/vendor/stb_image/stb_image.cpp
	${SOURCE_DIR}/Assert.h
	${SOURCE_DIR}/Camera.h
	${SOURCE_DIR}/Camera.cpp
	${SOURCE_DIR}/Scene.cpp 
	${SOURCE_DIR}/Scene.h
	${SOURCE_DIR}/Timer.h
)

set(ImGui_SOURCES
	${IMGUI_PATH}/imconfig.h
	${IMGUI_PATH}/imgui.cpp
	${IMGUI_PATH}/imgui.h
	${IMGUI_PATH}/imgui_demo.cpp
	${IMGUI_PATH}/imgui_draw.cpp
	${IMGUI_PATH}/imgui_internal.h
	${IMGUI_PATH}/imgui_tables.cpp
	${IMGUI_PATH}/imgui_widgets.cpp
	${IMGUI_PATH}/imstb_rectpack.h
	${IMGUI_PATH}/imstb_textedit.h
	${IMGUI_PATH}/imstb_truetype.h
	${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp
	${IMGUI_PATH}/backends/imgui_impl_opengl3.h
	${IMGUI_PATH}/backends/imgui_impl_opengl3_loader.h
	${IMGUI_PATH}/backends/imgui_impl_glfw.h
	${IMGUI_PATH}/backends/imgui_impl_glfw.cpp
	${IMGUI_PATH}/misc/debuggers/imgui.natvis
	${IMGUI_PATH}/misc/debuggers/imgui.natstepfilter
	${IMGUI_PATH}/misc/cpp/imgui_stdlib.cpp
	${IMGUI_PATH}/misc/cpp/imgui_stdlib.h
)

list(APPEND SOURCES ${ImGui_SOURCES})

add_executable(ProjektGrafika ${SOURCES})


target_link_libraries(ProjektGrafika glfw)
target_link_libraries(ProjektGrafika glad)
target_link_libraries(ProjektGrafika glm::glm)
target_include_directories(ProjektGrafika PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source/vendor/stb_image)
target_include_directories(ProjektGrafika PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source/vendor/ImGui/imgui)

target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/res/shaders/")

#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_directory
#    ${CMAKE_CURRENT_SOURCE_DIR}/res
#    $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
#)
