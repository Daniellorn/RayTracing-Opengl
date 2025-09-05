#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <filesystem>


#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Scene.h"
#include "Timer.h"
#include "Framebuffer.h" 
#include "UBO.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);

struct RenderSpecification
{

};

class Application
{
public:

	Application();
	~Application();

	void Init();
	void Run();

private:

	
	int InitWindow();


private:
	
	GLFWwindow* m_Window = nullptr;
	Camera m_Camera;
	Framebuffer m_FB;
	Shader m_ComputeShader;
	Scene m_Scene;
	UBO<Sphere> SphereUBO;
	UBO<Material> MaterialUBO;

	ImGuiContext* m_ImGuiContext = nullptr;

	int m_Width = 1280;
	int m_Height = 720;

};