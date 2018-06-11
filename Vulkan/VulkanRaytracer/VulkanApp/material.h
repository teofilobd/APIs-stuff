#ifndef MATERIALH
#define MATERIALH

#include "ray.h"
#include "hitable.h"

glm::vec3 random_in_unit_sphere()
{
	glm::vec3 p;
	do
	{
		p = 2.0f * glm::vec3(drand48(), drand48(), drand48()) - glm::vec3(1, 1, 1);
	} while (glm::dot(p, p) >= 1.0);
	return p;
}

class material
{
	public:
		virtual bool scatter(const ray& r_in, const hit_record& rec, glm::vec3& attenuation, ray& scattered) const = 0;
};

#endif