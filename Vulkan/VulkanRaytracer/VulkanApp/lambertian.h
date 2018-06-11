#ifndef LAMBERTIANH
#define LAMBERTIANH

#include <random>

#include "material.h"

class lambertian : public material
{
	public:
		lambertian(const glm::vec3& a) : albedo(a) {}
		virtual bool scatter(const ray& r_in, const hit_record& rec, glm::vec3& attenuation, ray& scattered) const
		{
			glm::vec3 target = rec.p + rec.normal + random_in_unit_sphere();
			scattered = ray(rec.p, target - rec.p);
			attenuation = albedo;
			return true;
		}

		glm::vec3 albedo;
};

#endif // !LAMBERTIANH

