#pragma once

#include <glm/glm.hpp>

struct Light {

    // 光源发射的光线
    // (0.0f, 0.0f, 1.0f) 表示光只发射蓝色光线
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

