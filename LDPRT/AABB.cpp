#include "AABB.h"

AABB::AABB() :maxp(vec3(FLT_MIN, FLT_MIN, FLT_MIN)), minp(vec3(FLT_MAX, FLT_MAX, FLT_MAX)){}

void AABB::expand(const vec3 &v)
{
	if (v.x > maxp.x)maxp.x = v.x;
	if (v.y > maxp.y)maxp.y = v.y;
	if (v.z > maxp.z)maxp.z = v.z;

	if (v.x < minp.x)minp.x = v.x;
	if (v.y < minp.y)minp.y = v.y;
	if (v.z < minp.z)minp.z = v.z;
}

MAJOR_AXIS AABB::majorAxis()
{
	vec3 distance = maxp - minp;
	if (distance.x > distance.y && distance.x > distance.z)return AXIS_X;
	else if (distance.y > distance.z)return AXIS_Y;
	else return AXIS_Z;
}

bool AABB::isPointInside(const vec3 &point) const
{
	bool ret = ((point.x < maxp.x) &&
		(point.y <= maxp.y) &&
		(point.z <= maxp.z) &&
		(point.x >= minp.x) &&
		(point.y >= minp.y) &&
		(point.z >= minp.z));
	return ret;
}

//Slabs method to check intersecting problem
bool AABB::isIntersect(Ray &ray, double *hitt0, double *hitt1) const
{
	float t0 = FLT_MIN, t1 = FLT_MAX;
	for (int i = 0; i < 3; ++i) {
		float invRayDir = 1.f / ray.direction[i];
		float tNear = (minp[i] - ray.orig[i]) * invRayDir;
		float tFar = (maxp[i] - ray.orig[i]) * invRayDir;

		if (tNear > tFar) std::swap(tNear, tFar);
		t0 = tNear > t0 ? tNear : t0;
		t1 = tFar  < t1 ? tFar : t1;
		if (t0 > t1) return false;
	}
	if (hitt0) *hitt0 = t0;
	if (hitt1) *hitt1 = t1;
	return true;
}