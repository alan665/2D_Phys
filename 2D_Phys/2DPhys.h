#pragma once
#include "olcPixelGameEngine.h"
#include <algorithm>
#include <functional>
#undef min
#undef max

namespace aabb 
{
	struct rect
	{
		olc::vf2d pos;
		olc::vf2d size;
		olc::vf2d vel;

		std::array<aabb::rect*, 4> contact;
	};

	bool PointVsRect(const olc::vf2d& p, const aabb::rect* r)
	{
		return (p.x >= r->pos.x && p.y >= r->pos.y && p.x < r->pos.x + r->size.x && p.y < r->pos.y + r->size.y);
	}

	bool RectVsRect(const aabb::rect* r1, const aabb::rect* r2)
	{
		return (r1->pos.x < r2->pos.x + r2->size.x && r1->pos.x + r1->size.x > r2->pos.x && r1->pos.y < r2->pos.y + r2->size.y && r1->pos.y + r1->size.y > r2->pos.y);
	}

	bool RayVsRect(const olc::vf2d& ray_origin, const olc::vf2d& ray_dir, const rect* target, olc::vf2d& contact_point, olc::vf2d& contact_normal, float& t_hit_near)
	{
		contact_normal = { 0,0 };
		contact_point = { 0,0 };

		// Cache division
		olc::vf2d invdir = 1.0f / ray_dir;

		// Calculate intersections with rect bounding axes
		olc::vf2d t_near = (target->pos - ray_origin) * invdir;
		olc::vf2d t_far = (target->pos + target->size - ray_origin) * invdir;

		if (std::isnan(t_far.y) || std::isnan(t_far.x)) return false;
		if (std::isnan(t_near.y) || std::isnan(t_near.x)) return false;

		// Sort distances
		if (t_near.x > t_far.x) std::swap(t_near.x, t_far.x);
		if (t_near.y > t_far.y) std::swap(t_near.y, t_far.y);

		// Early rejection		
		if (t_near.x > t_far.y || t_near.y > t_far.x) return false;

		// Closest 'time' will be the first contact
		t_hit_near = std::max(t_near.x, t_near.y);

		// Furthest 'time' is contact on opposite side of target
		float t_hit_far = std::min(t_far.x, t_far.y);

		// Reject if ray direction is pointing away from object
		if (t_hit_far < 0)
			return false;

		// Contact point of collision from parametric line equation
		contact_point = ray_origin + t_hit_near * ray_dir;

		if (t_near.x > t_near.y)
			if (invdir.x < 0)
				contact_normal = { 1, 0 };
			else
				contact_normal = { -1, 0 };
		else if (t_near.x < t_near.y)
			if (invdir.y < 0)
				contact_normal = { 0, 1 };
			else
				contact_normal = { 0, -1 };

		return true;
	}

	bool DynamicRectVsRect(const aabb::rect* r_dynamic, const float fTimeStep, const aabb::rect& r_static,
		olc::vf2d& contact_point, olc::vf2d& contact_normal, float& contact_time)
	{
		// Check if dynamic rectangle is actually moving - we assume rectangles are NOT in collision to start
		if (r_dynamic->vel.x == 0 && r_dynamic->vel.y == 0)
			return false;

		// Expand target rectangle by source dimensions
		aabb::rect expanded_target;
		expanded_target.pos = r_static.pos - r_dynamic->size / 2;
		expanded_target.size = r_static.size + r_dynamic->size;

		if (RayVsRect(r_dynamic->pos + r_dynamic->size / 2, r_dynamic->vel * fTimeStep, &expanded_target, contact_point, contact_normal, contact_time))
			return (contact_time >= 0.0f && contact_time < 1.0f);
		else
			return false;
	}

	bool ResolveDynamicRectVsRect(aabb::rect* r_dynamic, const float fTimeStep, aabb::rect* r_static)
	{
		olc::vf2d contact_point, contact_normal;
		float contact_time = 0.0f;
		if (DynamicRectVsRect(r_dynamic, fTimeStep, *r_static, contact_point, contact_normal, contact_time))
		{
			if (contact_normal.y > 0) r_dynamic->contact[0] = r_static; else nullptr;
			if (contact_normal.x < 0) r_dynamic->contact[1] = r_static; else nullptr;
			if (contact_normal.y < 0) r_dynamic->contact[2] = r_static; else nullptr;
			if (contact_normal.x > 0) r_dynamic->contact[3] = r_static; else nullptr;

			r_dynamic->vel += contact_normal * olc::vf2d(std::abs(r_dynamic->vel.x), std::abs(r_dynamic->vel.y)) * (1 - contact_time);
			return true;
		}
		return false;
	}
}
