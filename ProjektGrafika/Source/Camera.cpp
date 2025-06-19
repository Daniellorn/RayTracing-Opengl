#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Camera.h"

Camera::Camera(GLFWwindow* window, float FOV, float nearClip, float farClip, int width, int height)
	: m_Window(window), m_FOV(FOV), m_NearClip(nearClip), m_FarClip(farClip), m_Width(width), m_Height(height)
{
	m_ForwardDirection = glm::vec3(0.0f, 0.0f, -1.0f);
	m_Position = glm::vec3(0.0f, 0.0f, 3.0f);

	CalculateProjection();
	RecalculateView();
}

bool Camera::OnUpdate(float ts)
{
	glm::dvec2 mousePosition;

	glfwGetCursorPos(m_Window, &mousePosition.x, &mousePosition.y);

	glm::dvec2 delta = (mousePosition - m_LastMousePosition) * (double)m_Sensitivity;
	m_LastMousePosition = mousePosition;

	if (glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
	{
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		return false;
	}

	glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	//glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	bool moved = false;

	constexpr glm::vec3 upDirection{ 0.0f, 1.0f, 0.0f };
	glm::vec3 rightDirection = glm::normalize(glm::cross(m_ForwardDirection, upDirection));

	float speed = 5.0f;

	if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
	{
		m_Position += m_ForwardDirection * speed * ts;
		moved = true;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
	{
		m_Position -= m_ForwardDirection * speed * ts;
		moved = true;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
	{
		m_Position -= rightDirection * speed * ts;
		moved = true;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
	{
		m_Position += rightDirection * speed * ts;
		moved = true;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		m_Position += upDirection * speed * ts;
		moved = true;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		m_Position -= upDirection * speed * ts;
		moved = true;
	}

	if (delta.x != 0.0 || delta.y != 0.0)
	{
		float pitchDelta = (float)delta.y * 0.4f; // zmiana y
		float yawDelta = (float)delta.x * 0.4f; //zmiana x

		glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, rightDirection),
			glm::angleAxis(-yawDelta, glm::vec3(0.0f, 1.0f, 0.0f))));
		
		m_ForwardDirection = glm::rotate(q, m_ForwardDirection);

		moved = true;
	}

	if (moved)
	{
		RecalculateView();
	}

	return moved;
}

void Camera::RecalculateView()
{
	m_View = glm::lookAt(m_Position, m_Position + m_ForwardDirection, glm::vec3(0, 1, 0));
	m_InverseView = glm::inverse(m_View);
}

void Camera::CalculateProjection()
{
	m_Projection = glm::perspectiveFov(glm::radians(m_FOV), (float)m_Width, (float)m_Height, m_NearClip, m_FarClip);
	m_InverseProjection = glm::inverse(m_Projection);
}
