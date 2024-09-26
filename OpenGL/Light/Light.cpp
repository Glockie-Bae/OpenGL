#include "Light.h"

Light::Light()
{
}

Light::Light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
{
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
}

DirLight::DirLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 direction)
	: Light(ambient, diffuse, specular)
{
	this->direction = direction;
}

PointLight::PointLight()
{
	this->ambient = glm::vec3(0.2f);
	this->diffuse = glm::vec3(0.5f);
	this->specular = glm::vec3(1.0f);
	this->position = glm::vec3(0.0f, 0.0f, 0.0f);
	this->constant = 1.0f;
	this->linear = 0.09f;
	this->quadratic = 0.032;
}

PointLight::PointLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 position, float constant, float linear, float quadratic)
{
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->position = position;
	this->constant = constant;
	this->linear = linear;
	this->quadratic = quadratic;
}

PointLight::PointLight(glm::vec3 position, glm::vec3 color)
{
	this->position = position;
	this->color = color;

	this->ambient = glm::vec3(0.0f);
	this->diffuse = glm::vec3(0.0f);
	this->specular = glm::vec3(0.0f);
	this->constant = 0.0f;
	this->linear = 0.0f;
	this->quadratic = 0.0f;
}

SpotLight::SpotLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 position, glm::vec3 direction, float cutOff, float outerCutOff, float constant, float linear, float quadratic)
{
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->position = position;
	this->direction = direction;
	this->cutOff = glm::cos(glm::radians(cutOff));
	this->outerCutOff = glm::cos(glm::radians(outerCutOff));
	this->constant = constant;
	this->linear = linear;
	this->quadratic = quadratic;
}

void SpotLight::Update(glm::vec3 position, glm::vec3 direction)
{
	this->position = position;
	this->direction = direction;
}
