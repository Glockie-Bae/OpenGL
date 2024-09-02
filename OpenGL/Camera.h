#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include<iostream>

class Camera {
public:

	Camera();
	~Camera();
	Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up);

	glm::mat4 GetViewMatrix();
	glm::vec3 GetPos();


	// 摄像机位置移动
	void KeyboardMoveCamera(GLFWwindow * window, float deltaTime);
	// 摄像机镜头移动
	void MouseMoveCameraView(float mouseXOffset, float mouseYOffset, GLboolean constrainPitch = true);
	// 摄像机缩放移动
	void MouseSrollCameraView(float yoffset);
	void updateCameraView();

	float GetFOV() const ;
	void SetFOV(float FOV);
	
private:
	glm::vec3 m_CameraPos;
	glm::vec3 m_CameraFront;
	glm::vec3 m_CameraUp;
	glm::vec3 m_CameraRight;
	glm::vec3 m_CameraWorldUp;

	float m_Speed = 0.0f;
	float m_MouseSensitivity = 0.0f;
	float m_Pitch = 0.0f;
	float m_Yaw = -90.0f;
	float m_Zoom = 45.0f;

	
};