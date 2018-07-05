#ifndef MATERIALH
#define MATERIALH

#include "ray.h"
#include "hitable.h"
#include "texture.h"

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
		virtual glm::vec3 emitted(float u, float v, const glm::vec3& p) const
		{
			return glm::vec3(0, 0, 0);
		}
};

class lambertian : public material
{
public:
	lambertian(texture *a) : albedo(a) {}
	virtual bool scatter(const ray& r_in, const hit_record& rec, glm::vec3& attenuation, ray& scattered) const
	{
		glm::vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		scattered = ray(rec.p, target - rec.p, r_in.time());
		attenuation = albedo->value(rec.u, rec.v, rec.p);
		return true;
	}

	texture *albedo;
};

class metal : public material
{
public:
	metal(const glm::vec3& a, float f) : albedo(a) { if (f < 1) fuzz = f; else fuzz = 1; }
	virtual bool scatter(const ray& r_in, const hit_record& rec, glm::vec3& attenuation, ray& scattered) const
	{
		glm::vec3 reflected = reflect(glm::normalize(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
		attenuation = albedo;
		return (glm::dot(scattered.direction(), rec.normal) > 0);
	}

	glm::vec3 albedo;
	float fuzz;
};

bool refract(const glm::vec3& v, const glm::vec3& n, float ni_over_nt, glm::vec3& refracted)
{
	glm::vec3 uv = glm::normalize(v);
	float dt = glm::dot(uv, n);
	float discriminant = 1.0f - ni_over_nt * ni_over_nt * (1.0f - dt * dt);
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
			ni_over_nt = 1.0f / ref_idx;
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

		if (drand48() < reflected_prob)
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

class diffuse_light : public material
{
public:
	diffuse_light(texture *a) : emit(a) {}
	virtual bool scatter(const ray& r_in, const hit_record& rec, glm::vec3& attenuation, ray& scattered) const { return false; }
	virtual glm::vec3 emitted(float u, float v, const glm::vec3& p) const
	{
		return emit->value(u, v, p);
	}
	texture *emit;
};

class isotropic : public material
{
	public:
		isotropic(texture *a) : albedo(a) {}
		virtual bool scatter(const ray& r_in, const hit_record& rec, glm::vec3& attenuation, ray& scattered) const
		{
			scattered = ray(rec.p, random_in_unit_sphere());
			attenuation = albedo->value(rec.u, rec.v, rec.p);
			return true;
		}
		texture *albedo;
};

#endif