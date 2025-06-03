#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Camera.h"

#include <iostream>

Camera::Camera(GLFWwindow* window, int width, int height)
	: m_Height(height), m_Width(width), m_Window(window)
{
}

glm::vec3 Camera::GetCameraPosition() const
{
	return m_CameraPosition;
}

glm::vec3 Camera::GetCameraRotation() const
{
	return m_CameraRotation;
}

glm::vec3 Camera::GetCameraUp() const
{
	return m_CameraUp;
}

glm::vec3 Camera::GetCameraFront() const
{
	return m_CameraFront;
}

glm::vec3 Camera::GetCameraRight() const
{
	return m_CameraRight;
}

float Camera::GetCameraSpeed() const
{
	return m_Speed;
}

void Camera::Inputs(float deltaTime)
{
	if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
	{
		m_CameraPosition += m_Speed * m_CameraFront * deltaTime;
		std::cout << deltaTime << std::endl;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
	{
		m_CameraPosition -= m_Speed * m_CameraFront * deltaTime;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
	{
		m_CameraPosition += m_Speed * -glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * deltaTime;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
	{
		m_CameraPosition += m_Speed * glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * deltaTime;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		m_CameraPosition += m_Speed * m_CameraUp * deltaTime;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		m_CameraPosition += m_Speed * -m_CameraUp * deltaTime;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		m_Speed = 0.4f;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		m_Speed = 0.1f;
	}


	if (glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		if (m_Click)
		{
			glfwSetCursorPos(m_Window, (m_Width / 2), (m_Height / 2));
			m_Click = false;
		}

		double mouseX;
		double mouseY;

		glfwGetCursorPos(m_Window, &mouseX, &mouseY);

		float deltaX = (float)(mouseX - m_Width / 2) / m_Width;
		float deltaY = (float)(mouseY - m_Height / 2) / m_Height;

		m_CameraRotation.y += deltaX * m_Sensitivity; //rotacja w x
		m_CameraRotation.x -= deltaY * m_Sensitivity; // rotacaj w y

		if (m_CameraRotation.x > 89.0f)
			m_CameraRotation.x = 89.0f;
		if (m_CameraRotation.x < -89.0f)
			m_CameraRotation.x = -89.0f;

		if (m_CameraRotation.y > 360.0f) m_CameraRotation.y -= 360.0f;
		if (m_CameraRotation.y < 0.0f)   m_CameraRotation.y += 360.0f;


		glm::vec3 direction;
		direction.x = cos(glm::radians(m_CameraRotation.x)) * cos(glm::radians(m_CameraRotation.y));
		direction.y = sin(glm::radians(m_CameraRotation.x));
		direction.z = cos(glm::radians(m_CameraRotation.x)) * sin(glm::radians(m_CameraRotation.y));
		m_CameraFront = glm::normalize(direction);

		glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

		m_CameraRight = glm::normalize(glm::cross(m_CameraFront, worldUp));
		m_CameraUp = glm::normalize(glm::cross(m_CameraRight, m_CameraFront));


		glfwSetCursorPos(m_Window, m_Width / 2, m_Height / 2);
	}
	else
	{
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		m_Click = true;
	}

	
}
