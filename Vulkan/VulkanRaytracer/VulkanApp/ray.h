#ifndef RAYH
#define RAYH
#include <glm/glm.hpp>

class ray
{
	public:
		ray() {}
		ray(const glm::vec3& a, const glm::vec3& b) { A = a; B = b; }
		glm::vec3 origin() const { return A; }
		glm::vec3 direction() const { return B; }
		glm::vec3 point_at_parameter(float t) const { return A + t * B; }

		glm::vec3 A;
		glm::vec3 B;
};

#endif