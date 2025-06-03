#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>

class Camera
{
public:
	Camera(GLFWwindow* window, int width, int height);
	~Camera() = default;

	glm::vec3 GetCameraPosition() const;
	glm::vec3 GetCameraRotation() const;
	glm::vec3 GetCameraUp() const;
	glm::vec3 GetCameraFront() const;
	glm::vec3 GetCameraRight() const;

	float GetCameraSpeed() const;

	void Inputs(float deltaTime);


private:

	glm::vec3 m_CameraPosition{ 0.0f, 0.0f, 3.0f };
	glm::vec3 m_CameraRotation{ 0.0f, 0.0f, 0.0f };
	glm::vec3 m_CameraUp{ 0.0f, 1.0f, 0.0f };
	glm::vec3 m_CameraFront{ 0.0f, 0.0f, -1.0f };
	glm::vec3 m_CameraRight{ 0.0f, 0.0f, 0.0f };

	GLFWwindow* m_Window;
	
	int m_Width;
	int m_Height;

	float m_Speed = 100000.0f;
	float m_Sensitivity = 100.0f;

	bool m_Click = true;
};