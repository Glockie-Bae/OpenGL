#include "Ray.h"

float SphereHit(const Ray& sight, const float radius, const glm::vec3& SphereOrigin)
{
    float a = glm::dot(sight.GetDirection(), sight.GetDirection());
	float b = 2.0f * glm::dot(sight.GetDirection(), sight.GetOrigin() - SphereOrigin);
	float c = glm::dot(sight.GetOrigin() - SphereOrigin, sight.GetOrigin() - SphereOrigin) - radius * radius;

	float discriminant = b * b - 4.0f * a * c;
	if (discriminant < 0)
		return -1;
	else
		return (-b - sqrt(discriminant)) / (2.0f * a);

}

glm::vec4 lerp4(const Ray& sight)
{
	glm::vec3 SphereOrigin(0.0f, 0.0f, -1.0f);

	float _t = SphereHit(sight, 0.5f, SphereOrigin);
	if (_t > 0.0f) {
		glm::vec3 normal = glm::normalize(sight.to(_t) - SphereOrigin);
		glm::vec3 color = glm::vec3(0.5f * (normal.x + 1.0f), 0.5f * (normal.y + 1.0f), 0.5f * (normal.z + 1.0f));
		return glm::vec4(color, 1.0f);
	}


	glm::vec3 unit = glm::normalize(sight.GetDirection());
	float precision_t = 0.5 * (unit.y + 1.0f);
	return glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) * (1.0f - precision_t) + glm::vec4(0.5f, 0.7f, 1.0f, 1.0f) * precision_t;
}

