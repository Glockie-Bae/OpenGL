#pragma once

#include <glm/glm.hpp>

struct Light {

    // ��Դ����Ĺ���
    // (0.0f, 0.0f, 1.0f) ��ʾ��ֻ������ɫ����
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    Light();
    Light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
};

struct DirLight : public Light {
	glm::vec3 direction;
	DirLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 direction);
};

struct PointLight : public Light {
    glm::vec3 position;

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

	PointLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 position, float constant, float linear, float quadratic);
};

