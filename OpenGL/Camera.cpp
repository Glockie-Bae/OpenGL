#include "Camera.h"


Camera::Camera()
{
	m_CameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
	m_CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	m_CameraWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	m_CameraRight = glm::normalize(glm::cross(m_CameraFront, m_CameraWorldUp));
	m_CameraUp = glm::normalize(glm::cross(m_CameraRight, m_CameraFront));

	m_eye = glm::vec3(0.0f, 0.0f, 0.0f);
	m_horizontal = glm::vec3(4.0f, 0.0f, 0.0f);
	m_vertical = glm::vec3(0.0f, 2.0f, 0.0f);
	m_start = glm::vec3(-2.0f, -1.0f, -1.0f);
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

	m_eye = glm::vec3(0.0f, 0.0f, 0.0f);
	m_horizontal = glm::vec3(4.0f, 0.0f, 0.0f);
	m_vertical = glm::vec3(0.0f, 2.0f, 0.0f);
	m_start = glm::vec3(-2.0f, -1.0f, -1.0f);
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



void Camera::MouseMoveCameraView(double xposIn , double yposIn, GLboolean constrainPitch)
{
	
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (m_FirstMouse) {
		m_LastX = xpos;
		m_LastY = ypos;
		m_FirstMouse = false;
	}

	float xoffset = xpos - m_LastX;
	float yoffset = m_LastY - ypos; // reversed since y-coordinates go from bottom to top


	m_LastX = xpos;
	m_LastY = ypos;
	
	m_MouseSensitivity = 0.1f;
	xoffset *= m_MouseSensitivity;
	yoffset *= m_MouseSensitivity;

	

	m_Yaw += xoffset;
	m_Pitch += yoffset;


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

glm::vec3 Camera::GetFront() const
{
	return m_CameraFront;
}

void Camera::SetFOV(float FOV)
{
	m_Zoom = FOV;
}

void Camera::SetFirstMouse(bool IsTrue)
{
	m_FirstMouse = IsTrue;
}


