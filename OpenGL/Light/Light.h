#pragma once

#include <glm/glm.hpp>

struct Light {

    // ��Դ����Ĺ���
    // (0.0f, 0.0f, 1.0f) ��ʾ��ֻ������ɫ����
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    Light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
};

struct DirectLight : public Light {
	glm::vec3 direction;
	DirectLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 direction);
};

struct PointLight : public Light {
	glm::vec3 position;
	float constant;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutOff;
	PointLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 position, float constant, float linear, float quadratic);
};