#include "Sphere.h"

bool Sphere::hit(const Ray& sight, float t_min, float t_max, HitInfo& hitInfo) const
{

	/*
		struct HitInfo {
		float _t;   // ray的系数t
		glm::vec3 _p; // ray与物体相交点
		glm::vec3 _n; // 相交点物体法线
		};
	*/

	float a = glm::dot(sight.GetDirection(), sight.GetDirection());
	float b = 2.0f * glm::dot(sight.GetDirection(), sight.GetOrigin() - m_origin);
	float c = glm::dot(sight.GetOrigin() - m_origin, sight.GetOrigin() - m_origin) - m_radius * m_radius;

	float discriminant = b * b - 4.0f * a * c;
	if(discriminant > 0){
		float t = (-b - sqrt(discriminant)) / (2.0f * a);

		// 如果t在t_min和t_max之间
		// 相交于第一个点
		if (t<t_max && t>t_min) {
			hitInfo._t = t;
			hitInfo._p = sight.to(hitInfo._t);
			hitInfo._n = glm::normalize(hitInfo._p - m_origin);
			return true;
		}

		// 继续判断后续第二个点， 可能存在折射等需要继续判断第二个点
		t = (-b + sqrt(discriminant)) / (2.0f * a);

		if (t<t_max && t>t_min) {
			hitInfo._t = t;
			hitInfo._p = sight.to(hitInfo._t);
			hitInfo._n = glm::normalize(hitInfo._p - m_origin);
			return true;
		}

		
	}
	return false;


}
