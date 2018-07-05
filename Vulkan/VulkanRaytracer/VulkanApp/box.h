#ifndef BOXH 
#define BOXH

class xy_rect : public hitable
{
public:
	xy_rect() {}
	xy_rect(float _x0, float _x1, float _y0, float _y1, float _k, material *mat) : 
		x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat) {}
	virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const
	{
		box = aabb(glm::vec3(x0, y0, k - 0.0001f), glm::vec3(x1, y1, k + 0.0001f));
		return true;
	}
	material *mp;
	float x0, x1, y0, y1, k;
};

bool xy_rect::hit(const ray& r, float t0, float t1, hit_record& rec) const
{
	float t = (k - r.origin().z) / r.direction().z;
	if (t < t0 || t > t1)
	{
		return false;
	}
	float x = r.origin().x + t * r.direction().x;
	float y = r.origin().y + t * r.direction().y;
	if (x < x0 || x > x1 || y < y0 || y > y1)
	{
		return false;
	}
	rec.u = (x - x0) / (x1 - x0);
	rec.v = (y - y0) / (y1 - y0);
	rec.t = t;
	rec.mat_ptr = mp;
	rec.p = r.point_at_parameter(t);
	rec.normal = glm::vec3(0, 0, 1);
	return true;
}

class xz_rect : public hitable
{
public:
	xz_rect() {}
	xz_rect(float _x0, float _x1, float _z0, float _z1, float _k, material *mat) :
		x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat) {}
	virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const
	{
		box = aabb(glm::vec3(x0, k - 0.0001f, z0), glm::vec3(x1, k + 0.0001f, z1));
		return true;
	}
	material *mp;
	float x0, x1, z0, z1, k;
};

bool xz_rect::hit(const ray& r, float t0, float t1, hit_record& rec) const
{
	float t = (k - r.origin().y) / r.direction().y;
	if (t < t0 || t > t1)
	{
		return false;
	}
	float x = r.origin().x + t * r.direction().x;
	float z = r.origin().z + t * r.direction().z;
	if (x < x0 || x > x1 || z < z0 || z > z1)
	{
		return false;
	}
	rec.u = (x - x0) / (x1 - x0);
	rec.v = (z - z0) / (z1 - z0);
	rec.t = t;
	rec.mat_ptr = mp;
	rec.p = r.point_at_parameter(t);
	rec.normal = glm::vec3(0, 1, 0);
	return true;
}

class yz_rect : public hitable
{
public:
	yz_rect() {}
	yz_rect(float _y0, float _y1, float _z0, float _z1, float _k, material *mat) :
		y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat) {}
	virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const
	{
		box = aabb(glm::vec3(k - 0.0001f, y0, z0), glm::vec3(k + 0.0001f, y1, z1));
		return true;
	}
	material *mp;
	float y0, y1, z0, z1, k;
};

bool yz_rect::hit(const ray& r, float t0, float t1, hit_record& rec) const
{
	float t = (k - r.origin().x) / r.direction().x;
	if (t < t0 || t > t1)
	{
		return false;
	}
	float y = r.origin().y + t * r.direction().y;
	float z = r.origin().z + t * r.direction().z;
	if (y < y0 || y > y1 || z < z0 || z > z1)
	{
		return false;
	}
	rec.u = (y - y0) / (y1 - y0);
	rec.v = (z - z0) / (z1 - z0);
	rec.t = t;
	rec.mat_ptr = mp;
	rec.p = r.point_at_parameter(t);
	rec.normal = glm::vec3(1, 0, 0);
	return true;
}

class flip_normals : public hitable
{
	public:
		flip_normals(hitable *p) : ptr(p) {}
		virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const
		{
			if (ptr->hit(r, tmin, tmax, rec))
			{
				rec.normal = -rec.normal;
				return true;
			}
			else
			{
				return false;
			}
		}
		virtual bool bounding_box(float t0, float t1, aabb& box) const
		{
			return ptr->bounding_box(t0, t1, box);
		}
		hitable *ptr;
};

class box : public hitable
{
	public:
		box() {}
		box(const glm::vec3& p0, const glm::vec3& p1, material *ptr);
		virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;
		virtual bool bounding_box(float t0, float t1, aabb& box) const
		{
			box = aabb(pmin, pmax);
			return true;
		}
		glm::vec3 pmin, pmax;
		hitable *list_ptr;
};

box::box(const glm::vec3& p0, const glm::vec3& p1, material *ptr)
{
	pmin = p0;
	pmax = p1;
	hitable **list = new hitable*[6];
	list[0] = new xy_rect(p0.x, p1.x, p0.y, p1.y, p1.z, ptr);
	list[1] = new flip_normals(new xy_rect(p0.x, p1.x, p0.y, p1.y, p0.z, ptr));
	list[2] = new xz_rect(p0.x, p1.x, p0.z, p1.z, p1.y, ptr);
	list[3] = new flip_normals(new xz_rect(p0.x, p1.x, p0.z, p1.z, p0.y, ptr));
	list[4] = new yz_rect(p0.y, p1.y, p0.z, p1.z, p1.x, ptr);
	list[5] = new flip_normals(new yz_rect(p0.y, p1.y, p0.z, p1.z, p0.x, ptr));
	list_ptr = new hitable_list(list, 6);
}

bool box::hit(const ray& r, float t0, float t1, hit_record& rec) const
{
	return list_ptr->hit(r, t0, t1, rec);
}

class translate : public hitable
{
	public:
		translate(hitable *p, const glm::vec3& displacement) : ptr(p), offset(displacement) {}
		virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
		virtual bool bounding_box(float t0, float t1, aabb& box) const;
		hitable *ptr;
		glm::vec3 offset;
};

bool translate::hit(const ray& r, float tmin, float tmax, hit_record& rec) const
{
	ray moved_r(r.origin() - offset, r.direction(), r.time());
	if (ptr->hit(moved_r, tmin, tmax, rec))
	{
		rec.p += offset;
		return true;
	}
	else
	{
		return false;
	}
}

bool translate::bounding_box(float t0, float t1, aabb& box) const
{
	if (ptr->bounding_box(t0, t1, box))
	{
		box = aabb(box.min() + offset, box.max() + offset);
		return true;
	}
	else
	{
		return false;
	}
}

class rotate_y : public hitable
{
	public:
		rotate_y(hitable *p, float angle);
		virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
		virtual bool bounding_box(float t0, float t1, aabb& box) const
		{
			box = bbox;
			return hasbox;
		}
		hitable *ptr;
		float sin_theta;
		float cos_theta;
		bool hasbox;
		aabb bbox;
};

rotate_y::rotate_y(hitable *p, float angle) : ptr(p)
{
	float radians = glm::radians(angle);
	sin_theta = sin(radians);
	cos_theta = cos(radians);
	hasbox = ptr->bounding_box(0, 1, bbox);
	glm::vec3 min(FLT_MAX, FLT_MAX, FLT_MAX);
	glm::vec3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			for (int k = 0; k < 2; ++k)
			{
				float x = i * bbox.max().x + (1 - i) * bbox.min().x;
				float y = j * bbox.max().y + (1 - j) * bbox.min().y;
				float z = k * bbox.max().z + (1 - k) * bbox.min().z;
				float newx = cos_theta * x + sin_theta * z;
				float newz = -sin_theta * x + cos_theta * z;
				glm::vec3 tester(newx, y, newz);
				for (int c = 0; c < 3; ++c)
				{
					if (tester[c] > max[c])
					{
						max[c] = tester[c];
					}
					if (tester[c] < min[c])
					{
						min[c] = tester[c];
					}
				}
			}
		}
	}
	bbox = aabb(min, max);
}

bool rotate_y::hit(const ray& r, float tmin, float tmax, hit_record& rec) const
{
	glm::vec3 origin = r.origin();
	glm::vec3 direction = r.direction();
	origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
	origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];
	direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
	direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];
	ray rotated_r(origin, direction, r.time());
	if (ptr->hit(rotated_r, tmin, tmax, rec))
	{
		glm::vec3 p = rec.p;
		glm::vec3 normal = rec.normal;
		p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
		p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];
		normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
		normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];
		rec.p = p;
		rec.normal = normal;
		return true;
	}
	else
	{
		return false;
	}
}

#endif // !BOXH 
