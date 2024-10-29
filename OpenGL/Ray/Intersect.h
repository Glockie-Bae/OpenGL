#pragma once
#include"Ray.h"

struct HitInfo {
	float _t;   // ray��ϵ��t
	glm::vec3 _p; // ray�������ཻ��
	glm::vec3 _n; // �ཻ�����巨��
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


