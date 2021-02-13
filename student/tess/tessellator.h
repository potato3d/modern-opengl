#pragma once
#include <tess/triangle_mesh.h>

namespace tess
{
	// ---------------------------------------------------------------------------------------------------------------------------------------------------------
	// rectangular-base pyramid
	// ---------------------------------------------------------------------------------------------------------------------------------------------------------
	triangle_mesh tessellate_box(const vec3& extents);
	triangle_mesh tessellate_pyramid(const vec2& top_extents, const vec2& bottom_extents, float height, const vec2& offset = {0.0f, 0.0f});

	// ---------------------------------------------------------------------------------------------------------------------------------------------------------
	// truncated cone
	// ---------------------------------------------------------------------------------------------------------------------------------------------------------
	triangle_mesh tessellate_cylinder(float radius, float height,
									  int segment_count = 16, bool with_caps = true);
	triangle_mesh tessellate_cylinder_offset(float radius, float height, const vec2& offset,
											 int segment_count = 16, bool with_caps = true);
	triangle_mesh tessellate_cylinder_slope(float radius, float height, const vec2& top_slope_angles, const vec2& bottom_slope_angles,
											int segment_count = 16, const bool with_caps = true);
	triangle_mesh tessellate_cylinder_slope_offset(float radius, float height, const vec2& top_slope_angles, const vec2& bottom_slope_angles, const vec2& offset,
												   int segment_count = 16, bool with_caps = true);
	triangle_mesh tessellate_cone(float top_radius, float bottom_radius, float height,
								  int segment_count = 16, bool with_caps = true);
	triangle_mesh tessellate_cone_offset(float top_radius, float bottom_radius, float height, const vec2& offset,
										 int segment_count = 16, bool with_caps = true);
	triangle_mesh tessellate_cone_slope(float top_radius, float bottom_radius, float height, const vec2& top_slope_angles, const vec2& bottom_slope_angles,
										int segment_count = 16, const bool with_caps = true);
	triangle_mesh tessellate_cone_slope_offset(float top_radius, float bottom_radius, float height, const vec2& top_slope_angles, const vec2& bottom_slope_angles,
											   const vec2& offset, int segment_count = 16, bool with_caps = true);

	// ---------------------------------------------------------------------------------------------------------------------------------------------------------
	// truncated toroid
	// ---------------------------------------------------------------------------------------------------------------------------------------------------------
	triangle_mesh tessellate_circular_torus(float in_radius, float out_radius, float sweep_angle,
											int segment_count = 16, int sweep_count = 8, bool with_caps = false);
	triangle_mesh tessellate_rectangular_torus(float in_radius, float out_radius, float in_height, float sweep_angle,
											   int sweep_count = 8, bool with_caps = false);

	// ---------------------------------------------------------------------------------------------------------------------------------------------------------
	// ellipsoid
	// ---------------------------------------------------------------------------------------------------------------------------------------------------------
	triangle_mesh tessellate_dish(float radius, float height, int horizontal_count = 16, int vertical_count = 8, bool with_cap = false);
	triangle_mesh tessellate_sphere(float radius, int horizontal_count = 16, int vertical_count = 16);
	triangle_mesh tessellate_ellipsoid(const vec3& radii, int horizontal_count = 16, int vertical_count = 16,
									   float max_vertical_angle = glm::pi<float>(), bool bottom_cap = true);

	// ---------------------------------------------------------------------------------------------------------------------------------------------------------
	// polygonal mesh
	// ---------------------------------------------------------------------------------------------------------------------------------------------------------
	struct point
	{
		vec3 vertex;
		vec3 normal;
	};
	struct contour
	{
		std::vector<point> points;
	};
	struct polygon
	{
		std::vector<contour> contours;
	};
	void tessellate_polygonal_begin();
	void tessellate_polygonal_add(const polygon& poly);
	triangle_mesh tessellate_polygonal_end();
} // namespace tess
