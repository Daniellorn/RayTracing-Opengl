#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

class Camera
{
public:
	Camera(GLFWwindow* window, float FOV, float nearClip, float farClip, int width, int height);
	~Camera() = default;

	const glm::mat4& GetProjection() const { return m_Projection; }
	const glm::mat4& GetView() const { return m_View; }
	const glm::mat4& GetInverseView() const { return m_InverseView; }
	const glm::mat4& GetInverseProjection() const { return m_InverseProjection; }

	const glm::vec3& GetPosition() const { return m_Position; }
	const glm::vec3& GetDirection() const { return m_ForwardDirection; }
	 
	bool OnUpdate(float ts); 

private:

	void RecalculateView();
	void CalculateProjection();

private:
	GLFWwindow* m_Window;

	glm::mat4 m_Projection{ 1.0f };
	glm::mat4 m_View{ 1.0f };
	glm::mat4 m_InverseProjection{ 1.0f };
	glm::mat4 m_InverseView{ 1.0f };

	float m_FOV = 45.0f;
	float m_NearClip = 0.1f;
	float m_FarClip = 100.0f;

	float m_Sensitivity = 0.002f;

	glm::vec3 m_Position{ 0.0f, 0.0f, 0.0f };
	glm::vec3 m_ForwardDirection{ 0.0f, 0.0f, 0.0f };

	glm::dvec2 m_LastMousePosition{ 0.0, 0.0 };

	int m_Width, m_Height;
};