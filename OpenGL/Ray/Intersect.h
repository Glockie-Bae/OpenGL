#pragma once
#include"Ray.h"

struct HitInfo {
	float _t;   // ray的系数t
	glm::vec3 _p; // ray与物体相交点
	glm::vec3 _n; // 相交点物体法线
};


class Intersect
{
public:
	Intersect() {}

	virtual bool hit(const Ray& sight, float t_min, float t_max, HitInfo& hitInfo) const = 0;

	virtual ~Intersect() {};


	constexpr static float inf() { return 0x3f3f3f3f; }
private:
	HitInfo m_hitInfo;
};


