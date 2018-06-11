#ifndef DIELECTRICH
#define DIELECTRICH

#include "material.h"

bool refract(const glm::vec3& v, const glm::vec3& n, float ni_over_nt, glm::vec3& refracted)
{
	glm::vec3 uv = glm::normalize(v);
	float dt = glm::dot(uv, n);
	float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt);
	if (discriminant > 0)
	{
		refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
		return true;
	}
	
	return false;
}

float schlick(float cosine, float ref_idx)
{
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	return glm::lerp(pow(1 - cosine, 5), 1.f, r0 * r0);
}

class dielectric : public material
{
public:
	dielectric(float ri) : ref_idx(ri) {}
	virtual bool scatter(const ray& r_in, const hit_record& rec, glm::vec3& attenuation, ray& scattered) const
	{
		glm::vec3 outward_normal;
		glm::vec3 reflected = reflect(r_in.direction(), rec.normal);
		float ni_over_nt;
		attenuation = glm::vec3(1.0, 1.0, 1.0);
		glm::vec3 refracted;
		float reflected_prob;
		
		float cosine = glm::dot(r_in.direction(), rec.normal) / glm::length(r_in.direction());

		if (glm::dot(r_in.direction(), rec.normal) > 0)
		{
			outward_normal = -rec.normal;
			ni_over_nt = ref_idx;
			cosine = ref_idx * cosine;
		}
		else
		{
			outward_normal = rec.normal;
			ni_over_nt = 1.0 / ref_idx;
			cosine = -cosine;
		}

		if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
		{
			reflected_prob = schlick(cosine, ref_idx);
		} 
		else
		{
			reflected_prob = 1;
		}

		if(drand48() < reflected_prob)
		{
			scattered = ray(rec.p, reflected);;
		} 
		else
		{
			scattered = ray(rec.p, refracted);
		}
		return true;
	}

	float ref_idx;
};

#endif // !DIELECTRICH

