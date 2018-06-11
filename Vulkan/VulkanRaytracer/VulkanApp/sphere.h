#ifndef SPHEREH
#define SPHEREH

#include "hitable.h"

class sphere : public hitable
{
	public:
		sphere() {}
		sphere(glm::vec3 cen, float r, material *mat) : center(cen), radius(r), mat_ptr(mat) {}
		virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
		glm::vec3 center;
		float radius;
		material *mat_ptr;
};

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	glm::vec3 oc = r.origin() - center;
	float a = glm::dot(r.direction(), r.direction());
	float b = 2.0 * glm::dot(oc, r.direction());
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - 4 * a * c;
	float sqrtDiscriminant = sqrt(discriminant);
	float inv2a = 1 / (2 * a);

	if (discriminant > 0)
	{
		float temp = (-b - sqrtDiscriminant) * inv2a;
		if (temp < t_max && temp > t_min)
		{
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}
		temp = (-b + sqrtDiscriminant) * inv2a;
		if (temp < t_max && temp > t_min)
		{
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}

	return false;
}

#endif
