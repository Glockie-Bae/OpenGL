#pragma once

#include<glm/glm.hpp>

class Ray {
public:

	Ray()
		: m_origin(0.0f, 0.0f, 0.0f)
		, m_direction(0.0f, 0.0f, 0.0f)
	{}

	Ray(glm::vec3 origin, glm::vec3 direction)
		: m_origin(origin), m_direction(direction)
	{}

	Ray(const Ray& ray)
		: m_origin(ray.m_origin)
		, m_direction(ray.m_direction)
	{}

	
	inline glm::vec3 to(float _t) const { return glm::vec3(m_origin + m_direction * _t); }
	inline glm::vec3 GetOrigin() const { return m_origin; }
	inline glm::vec3 GetDirection() const { return m_direction; }

private:
	glm::vec3 m_origin;
	glm::vec3 m_direction;
};

float SphereHit(const Ray& ray, const float radius, const glm::vec3& SphereOrigin);

glm::vec4 lerp4(const Ray& sight);



