#include "Sphere.h"

bool Sphere::hit(const Ray& sight, float t_min, float t_max, HitInfo& hitInfo) const
{

	/*
		struct HitInfo {
		float _t;   // ray��ϵ��t
		glm::vec3 _p; // ray�������ཻ��
		glm::vec3 _n; // �ཻ�����巨��
		};
	*/

	float a = glm::dot(sight.GetDirection(), sight.GetDirection());
	float b = 2.0f * glm::dot(sight.GetDirection(), sight.GetOrigin() - m_origin);
	float c = glm::dot(sight.GetOrigin() - m_origin, sight.GetOrigin() - m_origin) - m_radius * m_radius;

	float discriminant = b * b - 4.0f * a * c;
	if(discriminant > 0){
		float t = (-b - sqrt(discriminant)) / (2.0f * a);

		// ���t��t_min��t_max֮��
		// �ཻ�ڵ�һ����
		if (t<t_max && t>t_min) {
			hitInfo._t = t;
			hitInfo._p = sight.to(hitInfo._t);
			hitInfo._n = glm::normalize(hitInfo._p - m_origin);
			return true;
		}

		// �����жϺ����ڶ����㣬 ���ܴ����������Ҫ�����жϵڶ�����
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
