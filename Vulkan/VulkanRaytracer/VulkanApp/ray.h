#ifndef RAYH
#define RAYH
#include <glm/glm.hpp>

class ray
{
	public:
		ray() {}
		ray(const glm::vec3& a, const glm::vec3& b, float ti = 0.0f) { A = a; B = b; _time = ti; }
		glm::vec3 origin() const { return A; }
		glm::vec3 direction() const { return B; }
		float time() const { return _time; }
		glm::vec3 point_at_parameter(float t) const { return A + t * B; }

		glm::vec3 A;
		glm::vec3 B;
		float _time;
};	

#endif