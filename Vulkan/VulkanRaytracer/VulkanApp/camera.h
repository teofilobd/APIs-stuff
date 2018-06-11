#ifndef CAMERAH
#define CAMERAH

#include <glm/gtc/constants.hpp>

#include "ray.h"

glm::vec3 random_in_unit_disk()
{
	glm::vec3 p;
	do
	{
		p = 2.0f * glm::vec3(drand48(), drand48(), 0) - glm::vec3(1, 1, 0);
	} while (glm::dot(p, p) >= 1.0);

	return p;
}

class camera
{
	public:
		camera(glm::vec3 lookfrom, glm::vec3 lookat, glm::vec3 vup, float vfov, float aspect, float aperture, float focus_dist) // vfov is top to bottom in degrees
		{
			lens_radius = aperture * 0.5f;
			float theta = glm::radians(vfov);
			float half_height = glm::tan(theta * 0.5f);
			float half_width = aspect * half_height;

			origin = lookfrom;
			w = glm::normalize(lookfrom - lookat);
			u = glm::normalize(glm::cross(vup, w));
			v = glm::cross(w, u);

			glm::mat3x3 lookMat(u, v, w);

			lower_left_corner = origin - focus_dist * lookMat * glm::vec3(half_width, half_height, 1);
			horizontal = 2.f * half_width * focus_dist * u;
			vertical = 2.f * half_height * focus_dist * v;
		}
		ray get_ray(float s, float t) 
		{
			glm::vec3 rd = lens_radius * random_in_unit_disk();
			glm::vec3 offset = u * rd.x + v * rd.y;
			return ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset); 
		}

		glm::vec3 u, v, w;
		glm::vec3 origin;
		glm::vec3 lower_left_corner;
		glm::vec3 horizontal;
		glm::vec3 vertical;
		float lens_radius;
};

#endif // !CAMERAH
