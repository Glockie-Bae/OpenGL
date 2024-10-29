#pragma once

#include"Sphere.h"
#include<vector>


class IntersectManager : public Intersect {
public:
	IntersectManager() {}

	void AddSphere(glm::vec3 origin, float r);

	virtual bool hit(const Ray& sight, float t_min, float t_max, HitInfo& hitInfo) const override;

	void DeleteObject();

private:

	std::vector<Intersect*> m_intersects;

};

glm::vec4 lerp5(const Ray& ray, IntersectManager& intersectManager);
