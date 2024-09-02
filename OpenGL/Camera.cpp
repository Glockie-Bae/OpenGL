#include "Camera.h"



Camera::Camera()
{
	m_CameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	m_CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	m_CameraWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	m_CameraRight = glm::normalize(glm::cross(m_CameraFront, m_CameraWorldUp));
	m_CameraUp = glm::normalize(glm::cross(m_CameraRight, m_CameraFront));
}

Camera::~Camera()
{
}

Camera::Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up)
{
	m_CameraPos = pos;
	m_CameraFront = front;
	m_CameraWorldUp = up;

	m_CameraRight = glm::normalize(glm::cross(m_CameraFront, m_CameraWorldUp));
	m_CameraUp = glm::normalize(glm::cross(m_CameraRight, m_CameraFront));
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(m_CameraPos, m_CameraPos + m_CameraFront, m_CameraUp);
}

glm::vec3 Camera::GetPos()
{
	return m_CameraPos;
}

void Camera::KeyboardMoveCamera(GLFWwindow* window, float deltaTime)
{
	m_Speed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		m_CameraPos += m_Speed * m_CameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		m_CameraPos -= m_Speed * m_CameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		m_CameraPos -= m_Speed * m_CameraRight;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		m_CameraPos += m_Speed * m_CameraRight;
	}
}



void Camera::MouseMoveCameraView(float mouseXOffset, float mouseYOffset, GLboolean constrainPitch)
{
	
	m_MouseSensitivity = 0.1f;
	mouseXOffset *= m_MouseSensitivity;
	mouseYOffset *= m_MouseSensitivity;

	

	m_Yaw += mouseXOffset;
	m_Pitch += mouseYOffset;


	if (constrainPitch) {
		if(m_Pitch>89.0f)
			m_Pitch = 89.0f;
		if (m_Pitch < -89.0f)
			m_Pitch = -89.0f;
	}

	updateCameraView();
}

void Camera::MouseSrollCameraView(float yoffset)
{
	m_Zoom -= (float)yoffset;
	if (m_Zoom < 1.0f)
		m_Zoom = 1.0f;
	if (m_Zoom > 45.0f)
		m_Zoom = 45.0f;
}

void Camera::updateCameraView()
{
	glm::vec3 front;
	front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	front.y = sin(glm::radians(m_Pitch));
	front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	m_CameraFront = glm::normalize(front);
	m_CameraRight = glm::normalize(glm::cross(m_CameraFront, m_CameraWorldUp));
	m_CameraUp = glm::normalize(glm::cross(m_CameraRight, m_CameraFront));

}

float Camera::GetFOV() const
{
	return m_Zoom;
}

void Camera::SetFOV(float FOV)
{
	m_Zoom = FOV;
}

