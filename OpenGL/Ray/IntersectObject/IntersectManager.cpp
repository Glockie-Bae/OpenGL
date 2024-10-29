#include "IntersectManager.h"

void IntersectManager::AddSphere(glm::vec3 origin, float r)
{
	Sphere* s = new Sphere(origin, r);
    m_intersects.push_back(s);
}

bool IntersectManager::hit(const Ray& sight, float t_min, float t_max, HitInfo& hitInfo) const
{
    HitInfo temp_info;
    bool hitSomething = false;
    float far = t_max; // 一开始看无限远
    int size = m_intersects.size();
    for (int i = 0; i < size; i++) {
        if (m_intersects[i]->hit(sight, t_min, far, temp_info)) {
            hitSomething = true;
            far = temp_info._t;
			hitInfo = temp_info;
        }
    }

    return hitSomething;
}

void IntersectManager::DeleteObject()
{
    for (auto object : m_intersects) {
        if(object)
			delete object;
    }
}

glm::vec4 lerp5(const Ray& ray, IntersectManager& intersectManager)
{
    HitInfo rec;
    glm::vec3 color;
    if (intersectManager.hit(ray, 0.0f, Intersect::inf(), rec)) {
        // 如果击中物体
        color = 0.5f * glm::vec3(rec._n.x + 1.0f, rec._n.y + 1.0f, rec._n.z + 1.0f);
        return glm::vec4(color, 1.0f);
    }
    else {
        // 没有击中物体
		glm::vec3 unit_direction = glm::normalize(ray.GetDirection());
		float t = 0.5f * (unit_direction.y + 1.0f);
		color = (1.0f - t) * glm::vec3(1.0f, 1.0f, 1.0f) + t * glm::vec3(0.5f, 0.7f, 1.0f);
		return glm::vec4(color, 1.0f);
    }
}


