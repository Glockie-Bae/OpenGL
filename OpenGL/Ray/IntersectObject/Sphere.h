#pragma once

#include"Ray/Intersect.h"

class Sphere : public Intersect {
public:
	Sphere(glm::vec3 heart, float r)
		:m_origin(heart), m_radius(r) {}



	virtual bool hit(const Ray& sight, float t_min, float t_max, HitInfo& hitInfo) const override;


	inline glm::vec3 GetOrigin() const { return m_origin; }
	inline float GetRadius() const { return m_radius; }

private:
	glm::vec3 m_origin;
	float m_radius;
};