#pragma once
#include <glm.h>

namespace tess
{
	struct box
	{
		vec3 extents = {0.0f, 0.0f, 0.0f};
	};

	struct circular_torus
	{
		float in_radius = 0.1f;
		float out_radius = 0.4f;
		float sweep_angle = math::half_pi();
	};

	struct cone
	{
		float top_radius = 0.25f;
		float bottom_radius = 0.5f;
		float height = 1.0f;
	};

	struct cone_offset
	{
		float top_radius = 0.25f;
		float bottom_radius = 0.5f;
		float height = 1.0f;
		vec2  offset = {0.25f, 0.25f};
	};

	struct cylinder
	{
		float radius = 0.5f;
		float height = 1.0f;
	};

	struct cylinder_offset
	{
		float radius = 0.5f;
		float height = 1.0f;
		vec2  offset = {0.25f, 0.25f};
	};

	struct dish
	{
		float radius = 0.5f;
		float height = 1.0f;
	};

	struct pyramid
	{
		vec2 top_extents = {0.25f, 0.25f};
		vec2 bottom_extents = {0.5f, 0.5f};
		vec2 offset = {0.25f, 0.25f};
		float height = 1.0f;
	};

	struct rectangular_torus
	{
		float in_height = 1.0f;
		float in_radius = 0.1f;
		float out_radius = 0.4f;
		float sweep_angle = math::half_pi();
	};

	struct sphere
	{
		float radius = 0.5f;
	};
} // namespace tess
