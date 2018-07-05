#ifndef SPHEREH
#define SPHEREH

#include "hitable.h"
#include <glm/gtc/constants.hpp>

void get_sphere_uv(const glm::vec3& p, float& u, float& v)
{
	float phi = atan2(p.z, p.x);
	float theta = asin(p.y);
	u = 1 - (phi + glm::pi<float>()) / (2.f * glm::pi<float>());
	v = (theta + glm::pi<float>() / 2.f) / glm::pi<float>();
}

class sphere : public hitable
{
	public:
		sphere() {}
		sphere(glm::vec3 cen, float r, material *mat) : center(cen), radius(r), mat_ptr(mat) {}
		virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
		virtual bool bounding_box(float t0, float t1, aabb& box) const;

		glm::vec3 center;
		float radius;
		material *mat_ptr;
};

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	glm::vec3 oc = r.origin() - center;
	float a = glm::dot(r.direction(), r.direction());
	float b = 2.0f * glm::dot(oc, r.direction());
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
			get_sphere_uv((rec.p - center) / radius, rec.u, rec.v);
			return true;
		}
		temp = (-b + sqrtDiscriminant) * inv2a;
		if (temp < t_max && temp > t_min)
		{
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center) / radius;
			rec.mat_ptr = mat_ptr;
			get_sphere_uv((rec.p - center) / radius, rec.u, rec.v);
			return true;
		}
	}

	return false;
}

bool sphere::bounding_box(float t0, float t1, aabb& box) const
{
	box = aabb(center - glm::vec3(radius, radius, radius), center + glm::vec3(radius, radius, radius));
	return true;
}

class moving_sphere : public hitable
{
	public:
		moving_sphere() {}
		moving_sphere(glm::vec3 cen0, glm::vec3 cen1, float t0, float t1, float r, material *m) : center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r), mat_ptr(m) {}
		virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
		virtual bool bounding_box(float t0, float t1, aabb& box) const;

		glm::vec3 center(float time) const;
		glm::vec3 center0, center1;
		float time0, time1;
		float radius;
		material *mat_ptr;
};

glm::vec3 moving_sphere::center(float time) const
{
	return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}

bool moving_sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	glm::vec3 oc = r.origin() - center(r.time());
	float a = glm::dot(r.direction(), r.direction());
	float b = 2.0f * glm::dot(oc, r.direction());
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
			rec.normal = (rec.p - center(r.time())) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}
		temp = (-b + sqrtDiscriminant) * inv2a;
		if (temp < t_max && temp > t_min)
		{
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center(r.time())) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}

	return false;
}

bool moving_sphere::bounding_box(float t0, float t1, aabb& box) const
{
	aabb aabbT0 = aabb(center(t0) - glm::vec3(radius, radius, radius), center(t0) + glm::vec3(radius, radius, radius));
	aabb aabbT1 = aabb(center(t1) - glm::vec3(radius, radius, radius), center(t1) + glm::vec3(radius, radius, radius));
	box = surrounding_box(aabbT0, aabbT1);
	return true;
}

#endif
