#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include<iostream>
#include"Ray/Ray.h"

static glm::mat4 captureViews[] =
{
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
};

static glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

class Camera {
public:

	Camera();
	~Camera();
	Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up);

	glm::mat4 GetViewMatrix();
	glm::vec3 GetPos();


	// �����λ���ƶ�
	void KeyboardMoveCamera(GLFWwindow * window, float deltaTime);
	// �������ͷ�ƶ�
	void MouseMoveCameraView(double xposIn, double yposIn, GLboolean constrainPitch = true);
	// ����������ƶ�
	void MouseSrollCameraView(float yoffset);
	void updateCameraView();

	float GetFOV() const ;
	glm::vec3 GetFront() const;
	void SetFOV(float FOV);
	
	void SetFirstMouse(bool IsTrue);

	inline const Ray GetRay(const float u, const float v) const {
		return Ray(m_eye, m_start + u * m_horizontal + v * m_vertical);
	}

	inline const Ray GetRay(const glm::vec2& para) const {
		return Ray(m_eye, m_start + para.x * m_horizontal + para.y * m_vertical);
	}

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

	float m_LastX = 0;
	float m_LastY = 0;

	bool m_FirstMouse = true;

	glm::vec3 m_eye;
	glm::vec3 m_start;
	glm::vec3 m_horizontal;
	glm::vec3 m_vertical;


};