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
