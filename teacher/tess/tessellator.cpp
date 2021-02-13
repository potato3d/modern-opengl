#include <tess/tessellator.h>
#include <tess/polygon_tessellator.h>

namespace tess
{
	// ---------------------------------------------------------------------------------------------------------------------------------------------------------
	// rectangular-base pyramid
	// ---------------------------------------------------------------------------------------------------------------------------------------------------------
	triangle_mesh tessellate_box(const vec3& extents)
	{
		const auto ext = vec2(extents.x, extents.y);
		return tessellate_pyramid(ext, ext, extents.z);
	}

	triangle_mesh tessellate_pyramid(const vec2& top_extents, const vec2& bottom_extents, float height, const vec2& offset /*= {0.0f, 0.0f}*/)
	{
		// vertices are computed as follows
		//     7+------+6
		//     /|     /|      y
		//    / |    / |      |
		//   / 3+---/--+2     |
		// 4+------+5 /       *---x
		//  | /    | /       /
		//  |/     |/       z
		// 0+------+1
		//
		// v0(--+) v4(-++)
		// v1(+-+) v5(+++)
		// v2(+--) v6(++-)
		// v3(---) v7(-+-)
		const auto half_offset = offset * 0.5f;

		const auto v0 = vec3(-0.5f * top_extents.x    + half_offset.x, -0.5f * top_extents.y    + half_offset.y,  0.5f * height);
		const auto v1 = vec3( 0.5f * top_extents.x    + half_offset.x, -0.5f * top_extents.y    + half_offset.y,  0.5f * height);
		const auto v2 = vec3( 0.5f * bottom_extents.x - half_offset.x, -0.5f * bottom_extents.y - half_offset.y, -0.5f * height);
		const auto v3 = vec3(-0.5f * bottom_extents.x - half_offset.x, -0.5f * bottom_extents.y - half_offset.y, -0.5f * height);
		const auto v4 = vec3(-0.5f * top_extents.x    + half_offset.x,  0.5f * top_extents.y    + half_offset.y,  0.5f * height);
		const auto v5 = vec3( 0.5f * top_extents.x    + half_offset.x,  0.5f * top_extents.y    + half_offset.y,  0.5f * height);
		const auto v6 = vec3( 0.5f * bottom_extents.x - half_offset.x,  0.5f * bottom_extents.y - half_offset.y, -0.5f * height);
		const auto v7 = vec3(-0.5f * bottom_extents.x - half_offset.x,  0.5f * bottom_extents.y - half_offset.y, -0.5f * height);

		const auto nxp = normalize(cross(v2-v1, v5-v1));
		const auto nxn = normalize(cross(v4-v0, v3-v0));
		const auto nyp = normalize(cross(v6-v5, v4-v5));
		const auto nyn = normalize(cross(v0-v1, v2-v1));

		const vec3 unit_z(0.0f, 0.0f, 1.0f);

		triangle_mesh mesh;

		// -----------------------------------------------------------------------------------------------------------------------------------------------------
		// vertices
		// -----------------------------------------------------------------------------------------------------------------------------------------------------
		// +x
		mesh.vertices.push_back({v2, nxp});
		mesh.vertices.push_back({v6, nxp});
		mesh.vertices.push_back({v5, nxp});
		mesh.vertices.push_back({v1, nxp});
		// +z
		mesh.vertices.push_back({v1, unit_z});
		mesh.vertices.push_back({v5, unit_z});
		mesh.vertices.push_back({v4, unit_z});
		mesh.vertices.push_back({v0, unit_z});
		// +y
		mesh.vertices.push_back({v5, nyp});
		mesh.vertices.push_back({v6, nyp});
		mesh.vertices.push_back({v7, nyp});
		mesh.vertices.push_back({v4, nyp});
		// -x
		mesh.vertices.push_back({v0, nxn});
		mesh.vertices.push_back({v4, nxn});
		mesh.vertices.push_back({v7, nxn});
		mesh.vertices.push_back({v3, nxn});
		// -z
		mesh.vertices.push_back({v3, -unit_z});
		mesh.vertices.push_back({v7, -unit_z});
		mesh.vertices.push_back({v6, -unit_z});
		mesh.vertices.push_back({v2, -unit_z});
		// -y
		mesh.vertices.push_back({v0, nyn});
		mesh.vertices.push_back({v3, nyn});
		mesh.vertices.push_back({v2, nyn});
		mesh.vertices.push_back({v1, nyn});

		// -----------------------------------------------------------------------------------------------------------------------------------------------------
		// elements
		// -----------------------------------------------------------------------------------------------------------------------------------------------------
		for(unsigned int i = 0; i < 6; ++i)
		{
			mesh.elements.push_back(0+i*4);
			mesh.elements.push_back(1+i*4);
			mesh.elements.push_back(2+i*4);
			mesh.elements.push_back(0+i*4);
			mesh.elements.push_back(2+i*4);
			mesh.elements.push_back(3+i*4);
		}

		return mesh;
	}

	// ---------------------------------------------------------------------------------------------------------------------------------------------------------
	// truncated cone
	// ---------------------------------------------------------------------------------------------------------------------------------------------------------
	triangle_mesh tessellate_cylinder(float radius, float height,
											 int segment_count /*= 16*/, bool with_caps /*= true*/)
	{
		return tessellate_cone_slope_offset(radius, radius, height, vec2(), vec2(), vec2(), segment_count, with_caps);
	}

	triangle_mesh tessellate_cylinder_offset(float radius, float height, const vec2& offset,
													int segment_count /*= 16*/, bool with_caps /*= true*/)
	{
		return tessellate_cone_slope_offset(radius, radius, height, vec2(), vec2(), offset, segment_count, with_caps);
	}

	triangle_mesh tessellate_cylinder_slope(float radius, float height, const vec2& top_slope_angles, const vec2& bottom_slope_angles,
												   int segment_count /*= 16*/, const bool with_caps /*= true*/)
	{
		return tessellate_cone_slope_offset(radius, radius, height, top_slope_angles, bottom_slope_angles, vec2(), segment_count, with_caps);
	}

	triangle_mesh tessellate_cylinder_slope_offset(float radius, float height, const vec2& top_slope_angles, const vec2& bottom_slope_angles, const vec2& offset,
												   int segment_count /*= 16*/, bool with_caps /*= true*/)
	{
		return tessellate_cone_slope_offset(radius, radius, height, top_slope_angles, bottom_slope_angles, offset, segment_count, with_caps);
	}

	triangle_mesh tessellate_cone(float top_radius, float bottom_radius, float height,
										 int segment_count /*= 16*/, bool with_caps /*= true*/)
	{
		return tessellate_cone_slope_offset(top_radius, bottom_radius, height, vec2(), vec2(), vec2(), segment_count, with_caps);
	}

	triangle_mesh tessellate_cone_offset(float top_radius, float bottom_radius, float height, const vec2& offset,
												int segment_count /*= 16*/, bool with_caps /*= true*/)
	{
		return tessellate_cone_slope_offset(top_radius, bottom_radius, height, vec2(), vec2(), offset, segment_count, with_caps);
	}

	triangle_mesh tessellate_cone_slope(float top_radius, float bottom_radius, float height, const vec2& top_slope_angles, const vec2& bottom_slope_angles,
											   int segment_count /*= 16*/, const bool with_caps /*= true*/)
	{
		return tessellate_cone_slope_offset(top_radius, bottom_radius, height, top_slope_angles, bottom_slope_angles, vec2(), segment_count, with_caps);
	}

	triangle_mesh tessellate_cone_slope_offset(float top_radius, float bottom_radius, float height, const vec2& top_slope_angles, const vec2& bottom_slope_angles,
											   const vec2& offset, int segment_count /*= 16*/, bool with_caps /*= true*/)
	{
		std::vector<vec3> top_positions;
		std::vector<vec3> bottom_positions;

		top_positions.reserve(segment_count);
		bottom_positions.reserve(segment_count);

		const auto top_scale = epsilonEqual(top_radius, 0.0f, 1e-6f)? 1e-6f : top_radius;
		const auto bottom_scale = epsilonEqual(bottom_radius, 0.0f, 1e-6f)? 1e-6f : bottom_radius;

		const auto top_offset = vec3(offset.x*0.5f, offset.y*0.5f, height * 0.5f);
		const auto bottom_offset = vec3(-offset.x*0.5f, -offset.y*0.5f, -height * 0.5f);

		auto delta_angle = (2.0f * pi<float>()) / static_cast<float>(segment_count);
		auto angle = 0.0f;

		const auto top_slope_quat = quat(vec3(top_slope_angles.x, top_slope_angles.y, 0));
		const auto bottom_slope_quat = quat(vec3(bottom_slope_angles.x, bottom_slope_angles.y, 0));

		for(int i = 0; i < segment_count; ++i)
		{
			auto x = cos(angle);
			auto y = sin(angle);

			// top position
			auto top_pos = vec3(x*top_scale, y*top_scale, 0.0f);
			top_pos = top_slope_quat * top_pos;
			top_pos += top_offset;
			top_positions.push_back(top_pos);

			// bottom position
			auto bottom_pos = vec3(x*bottom_scale, y*bottom_scale, 0.0f);
			bottom_pos = bottom_slope_quat * bottom_pos;
			bottom_pos += bottom_offset;
			bottom_positions.push_back(bottom_pos);

			angle += delta_angle;
		}

		const vec3 unit_z(0.0f, 0.0f, 1.0f);

		triangle_mesh mesh;

		// -----------------------------------------------------------------------------------------------------------------------------------------------------
		// vertices
		// -----------------------------------------------------------------------------------------------------------------------------------------------------

		int top_cap_start = 0;
		int bottom_cap_start = 0;

		if(with_caps)
		{
			// top cap
			top_cap_start = mesh.vertices.size();
			for(int i = 0; i < segment_count; ++i)
			{
				mesh.vertices.push_back({top_positions[i], top_slope_quat * unit_z});
			}

			// bottom cap
			bottom_cap_start = mesh.vertices.size();
			for(int i = 0; i < segment_count; ++i)
			{
				mesh.vertices.push_back({bottom_positions[i], bottom_slope_quat * -unit_z});
			}
		}

		// body
		int body_start = mesh.vertices.size();
		for(int i = 0; i < segment_count; ++i)
		{
			const auto& b = bottom_positions[i];
			const auto& b1 = bottom_positions[(i+1) % segment_count];
			const auto& b0 = bottom_positions[(i-1) < 0? segment_count-1 : i-1];

			const auto& t = top_positions[i];
			const auto& t1 = top_positions[(i+1) % segment_count];
			const auto& t0 = top_positions[(i-1) < 0? segment_count-1 : i-1];

			const auto bn = normalize(cross(b-t, b1-b0));
			const auto tn = normalize(cross(b-t, t1-t0));

			mesh.vertices.push_back({b, bn});
			mesh.vertices.push_back({t, tn});
		}

		// -----------------------------------------------------------------------------------------------------------------------------------------------------
		// elements
		// -----------------------------------------------------------------------------------------------------------------------------------------------------

		if(with_caps)
		{
			int cap_count = (segment_count - 2) / 2;

			// top cap
			for(int i = 0; i < cap_count; ++i)
			{
				mesh.elements.push_back(top_cap_start + i + 1);
				mesh.elements.push_back(top_cap_start + segment_count - (i+1));
				mesh.elements.push_back(top_cap_start + i);

				mesh.elements.push_back(top_cap_start + segment_count - (i+2));
				mesh.elements.push_back(top_cap_start + segment_count - (i+1));
				mesh.elements.push_back(top_cap_start + i + 1);
			}

			// bottom cap
			for(int i = 0; i < cap_count; ++i)
			{
				mesh.elements.push_back(bottom_cap_start + i);
				mesh.elements.push_back(bottom_cap_start + segment_count - (i+1));
				mesh.elements.push_back(bottom_cap_start + i + 1);

				mesh.elements.push_back(bottom_cap_start + i + 1);
				mesh.elements.push_back(bottom_cap_start + segment_count - (i+1));
				mesh.elements.push_back(bottom_cap_start + segment_count - (i+2));
			}
		}

		// body
		int body_limit = segment_count * 2;
		for(int i = 0; i < segment_count; ++i)
		{
			auto curr = i*2;
			mesh.elements.push_back(body_start + (curr + 2) % body_limit);
			mesh.elements.push_back(body_start + curr + 1);
			mesh.elements.push_back(body_start + curr + 0);

			mesh.elements.push_back(body_start + (curr + 2) % body_limit);
			mesh.elements.push_back(body_start + (curr + 3) % body_limit);
			mesh.elements.push_back(body_start + curr + 1);
		}

		return mesh;
	}


	// ---------------------------------------------------------------------------------------------------------------------------------------------------------
	// truncated toroid
	// ---------------------------------------------------------------------------------------------------------------------------------------------------------
	triangle_mesh tessellate_circular_torus(float in_radius, float out_radius, float sweep_angle,
											int segment_count /*= 16*/, int sweep_count /*= 8*/, bool with_caps /*= false*/)
	{
		std::vector<vec3> positions;
		std::vector<vec3> section_centers;

		auto xy_angle = 0.0f;
		const auto xy_delta_angle = (2.0f * pi<float>()) / static_cast<float>(segment_count);

		auto sweep = 0.0f;
		const auto sweep_delta_angle = sweep_angle / static_cast<float>(sweep_count);

		for(int i = 0; i < sweep_count+1; ++i)
		{
			section_centers.push_back(vec3(out_radius * cos(sweep), out_radius * sin(sweep), 0.0f));
			for(int j = 0; j < segment_count; ++j)
			{
				float section = out_radius + in_radius * sin(xy_angle);
				positions.push_back({section * cos(sweep), section * sin(sweep), in_radius * cos(xy_angle)});
				xy_angle += xy_delta_angle;
			}
			sweep += sweep_delta_angle;
		}

		const vec3 unit_y = vec3(0.0f, 1.0f, 0.0f);

		triangle_mesh mesh;

		// -----------------------------------------------------------------------------------------------------------------------------------------------------
		// vertices
		// -----------------------------------------------------------------------------------------------------------------------------------------------------

		int first_cap_start = 0;
		int second_cap_start = 0;

		if(with_caps)
		{
			// first cap
			first_cap_start = mesh.vertices.size();
			for(int i = 0; i < segment_count; ++i)
			{
				mesh.vertices.push_back({positions[i], -unit_y});
			}

			// second cap
			second_cap_start = mesh.vertices.size();

			const auto& last = positions[positions.size()-1];
			const auto& last_minus_1 = positions[positions.size()-2];
			const auto& last_minus_2 = positions[positions.size()-3];
			const auto cap_normal = normalize(cross(last - last_minus_1, last_minus_2 - last_minus_1));

			for(int i = 0; i < segment_count; ++i)
			{
				mesh.vertices.push_back({positions[positions.size()-segment_count+i], cap_normal});
			}
		}

		// body
		int body_start = mesh.vertices.size();

		for(int i = 0; i < sweep_count+1; ++i)
		{
			for(int j = 0; j < segment_count; ++j)
			{
				const auto& v = positions[i*segment_count+j];
				const auto n = normalize(v - section_centers[i]);
				mesh.vertices.push_back({v, n});
			}
		}

		// -----------------------------------------------------------------------------------------------------------------------------------------------------
		// elements
		// -----------------------------------------------------------------------------------------------------------------------------------------------------

		if(with_caps)
		{
			int cap_count = (segment_count - 2) / 2;

			// first cap
			for(int i = 0; i < cap_count; ++i)
			{
				mesh.elements.push_back(first_cap_start + i);
				mesh.elements.push_back(first_cap_start + segment_count - (i+1));
				mesh.elements.push_back(first_cap_start + i + 1);

				mesh.elements.push_back(first_cap_start + i + 1);
				mesh.elements.push_back(first_cap_start + segment_count - (i+1));
				mesh.elements.push_back(first_cap_start + segment_count - (i+2));
			}

			// second cap
			for(int i = 0; i < cap_count; ++i)
			{
				mesh.elements.push_back(second_cap_start + i + 1);
				mesh.elements.push_back(second_cap_start + segment_count - (i+1));
				mesh.elements.push_back(second_cap_start + i);

				mesh.elements.push_back(second_cap_start + segment_count - (i+2));
				mesh.elements.push_back(second_cap_start + segment_count - (i+1));
				mesh.elements.push_back(second_cap_start + i + 1);
			}
		}

		// body
		for(int i = 0; i < sweep_count; ++i)
		{
			for(int j = 0; j < segment_count; ++j)
			{
				int curr_start = i*segment_count;
				int next_start = (i+1)*segment_count;

				mesh.elements.push_back(body_start+curr_start+(j+1) % segment_count);
				mesh.elements.push_back(body_start+next_start+j);
				mesh.elements.push_back(body_start+curr_start+j);

				mesh.elements.push_back(body_start+next_start+(j+1) % segment_count);
				mesh.elements.push_back(body_start+next_start+j);
				mesh.elements.push_back(body_start+curr_start+(j+1) % segment_count);
			}
		}

		return mesh;
	}

	triangle_mesh tessellate_rectangular_torus(float in_radius, float out_radius, float in_height, float sweep_angle,
											   int sweep_count /*= 8*/, bool with_caps /*= false*/)
	{
		std::vector<vec3> positions;

		auto sweep = 0.0f;
		const auto sweep_delta_angle = sweep_angle / static_cast<float>(sweep_count);

		const auto hh = in_height * 0.5f;

		for(int i = 0; i < sweep_count+1; ++i)
		{
			const auto inner_scale = out_radius - in_radius;
			const auto outer_scale = out_radius + in_radius;

			// lower inner corner
			positions.push_back({inner_scale * cos(sweep), inner_scale * sin(sweep), -hh});

			// lower outer corner
			positions.push_back({outer_scale * cos(sweep), outer_scale * sin(sweep), -hh});

			// upper outer corner
			positions.push_back({outer_scale * cos(sweep), outer_scale * sin(sweep), hh});

			// upper inner corner
			positions.push_back({inner_scale * cos(sweep), inner_scale * sin(sweep), hh});

			sweep += sweep_delta_angle;
		}

		triangle_mesh mesh;

		// -----------------------------------------------------------------------------------------------------------------------------------------------------
		// vertices
		// -----------------------------------------------------------------------------------------------------------------------------------------------------

		// normals will be computed later for each triangle

		int first_cap_start = 0;
		int second_cap_start = 0;

		if(with_caps)
		{
			// first cap
			first_cap_start = mesh.vertices.size();
			for(int i = 0; i < 4; ++i)
			{
				mesh.vertices.push_back({positions[i], {}});
			}

			// second cap
			second_cap_start = mesh.vertices.size();
			for(int i = 0; i < 4; ++i)
			{
				mesh.vertices.push_back({positions[positions.size()-4+i], {}});
			}
		}

		// body
		// add entire curved surfaces one after the other to make element indexing easier
		int body_start = mesh.vertices.size();
		for(int k = 0; k < 4; ++k)
		{
			for(int i = 0; i < sweep_count+1; ++i)
			{
				const auto base = i*4;
				const auto curr = base + k;
				const auto next = base + (k+1) % 4;
				mesh.vertices.push_back({positions[curr], {}});
				mesh.vertices.push_back({positions[next], {}});
			}
		}

		// -----------------------------------------------------------------------------------------------------------------------------------------------------
		// elements
		// -----------------------------------------------------------------------------------------------------------------------------------------------------

		if(with_caps)
		{
			// first cap
			mesh.elements.push_back(first_cap_start + 1);
			mesh.elements.push_back(first_cap_start + 4 - 1);
			mesh.elements.push_back(first_cap_start + 0);

			mesh.elements.push_back(first_cap_start + 4 - 2);
			mesh.elements.push_back(first_cap_start + 4 - 1);
			mesh.elements.push_back(first_cap_start + 1);

			// second cap
			mesh.elements.push_back(second_cap_start + 0);
			mesh.elements.push_back(second_cap_start + 4 - 1);
			mesh.elements.push_back(second_cap_start + 1);

			mesh.elements.push_back(second_cap_start + 1);
			mesh.elements.push_back(second_cap_start + 4 - 1);
			mesh.elements.push_back(second_cap_start + 4 - 2);
		}

		// body
		for(int k = 0; k < 4; ++k)
		{
			for(int i = 0; i < sweep_count; ++i)
			{
				const auto base = 2*k*(sweep_count+1)+i*2;
				const auto curr = body_start + base;
				const auto next_curr = body_start + base + 2;

				mesh.elements.push_back(next_curr);
				mesh.elements.push_back(curr+1);
				mesh.elements.push_back(curr);

				mesh.elements.push_back(next_curr);
				mesh.elements.push_back(next_curr+1);
				mesh.elements.push_back(curr+1);
			}
		}

		// -----------------------------------------------------------------------------------------------------------------------------------------------------
		// normals
		// -----------------------------------------------------------------------------------------------------------------------------------------------------

		// flat surfaces: all vertices of the same triangle get the same normal
		for(decltype(mesh.elements)::size_type i = 0; i < mesh.elements.size(); i+=3)
		{
			auto& v0 = mesh.vertices[mesh.elements[i+0]];
			auto& v1 = mesh.vertices[mesh.elements[i+1]];
			auto& v2 = mesh.vertices[mesh.elements[i+2]];
			const auto n = normalize(cross(v1.position - v0.position, v2.position - v0.position));
			v0.normal = n;
			v1.normal = n;
			v2.normal = n;
		}

		return mesh;
	}

	// ---------------------------------------------------------------------------------------------------------------------------------------------------------
	// ellipsoid
	// ---------------------------------------------------------------------------------------------------------------------------------------------------------
	triangle_mesh tessellate_dish(float radius, float height, int horizontal_count /*= 16*/, int vertical_count /*= 8*/, bool with_cap /*= false*/)
	{
		return tessellate_ellipsoid(vec3(radius, radius, height), horizontal_count, vertical_count, pi<float>()*0.5f, with_cap);
	}

	triangle_mesh tessellate_sphere(float radius,
									int horizontal_count /*= 16*/, int vertical_count /*= 16*/)
	{
		return tessellate_ellipsoid(vec3(radius, radius, radius), horizontal_count, vertical_count);
	}

	triangle_mesh tessellate_ellipsoid(const vec3& radii, int horizontal_count /*= 16*/, int vertical_count /*= 16*/,
									   float max_vertical_angle /*= glm::pi<float>()*/, bool bottom_cap /*= true*/)
	{
		triangle_mesh mesh;

		// -----------------------------------------------------------------------------------------------------------------------------------------------------
		// vertices
		// -----------------------------------------------------------------------------------------------------------------------------------------------------

		const auto delta_horizontal_angle = (2.0f * pi<float>()) / static_cast<float>(horizontal_count);
		const auto delta_vertical_angle = max_vertical_angle / static_cast<float>(vertical_count);

		auto vertical_angle = delta_vertical_angle;

		if(max_vertical_angle < glm::pi<float>())
		{
			++vertical_count;
		}

		const vec3 unit_z(0.0f, 0.0f, 1.0f);

		// top vertex
		mesh.vertices.push_back({unit_z * radii.z, unit_z});

		// body
		vec3 radii2 = radii * radii;
		const auto inv_raddi_sqr = vec3(1.0f/radii2.x, 1.0f/radii2.y, 1.0f/radii2.z);
		for(int v = 0; v < vertical_count-1; ++v)
		{
			float horizontal_angle = 0.0f;

			for(int h = 0; h < horizontal_count; ++h)
			{
				vec3 pos;
				pos.x = radii.x*sin(vertical_angle)*cos(horizontal_angle);
				pos.y = radii.y*sin(vertical_angle)*sin(horizontal_angle);
				pos.z = radii.z*cos(vertical_angle);
				vec3 nrm = pos;
				nrm *= inv_raddi_sqr;
				mesh.vertices.push_back({pos, normalize(nrm)});
				horizontal_angle += delta_horizontal_angle;
			}

			vertical_angle += delta_vertical_angle;
		}

		// bottom vertices
		int bottom_cap_start = 0;
		if(bottom_cap)
		{
			if(max_vertical_angle < glm::pi<float>())
			{
				bottom_cap_start = mesh.vertices.size();
				for(int i = 0; i < horizontal_count; ++i)
				{
					mesh.vertices.push_back({mesh.vertices[bottom_cap_start-horizontal_count+i].position, -unit_z});
				}
			}
			else
			{
				mesh.vertices.push_back({-unit_z * radii.z, -unit_z});
			}
		}

		// -----------------------------------------------------------------------------------------------------------------------------------------------------
		// elements
		// -----------------------------------------------------------------------------------------------------------------------------------------------------

		// top cap
		int top_start = 1;
		for(int i = 0; i < horizontal_count; ++i)
		{
			mesh.elements.push_back(0);
			mesh.elements.push_back(top_start + i);
			mesh.elements.push_back(top_start + (i+1) % horizontal_count);
		}

		// body
		int body_start = top_start;
		for(int j = 0; j < vertical_count-2; ++j)
		{
			for(int i = 0; i < horizontal_count; ++i)
			{
				mesh.elements.push_back(body_start + (i+1) % horizontal_count);
				mesh.elements.push_back(body_start + i);
				mesh.elements.push_back(body_start + i + horizontal_count);

				mesh.elements.push_back(body_start + i + horizontal_count);
				mesh.elements.push_back(body_start + (i+1) % horizontal_count + horizontal_count);
				mesh.elements.push_back(body_start + (i+1) % horizontal_count);
			}
			body_start += horizontal_count;
		}

		// bottom cap
		if(bottom_cap)
		{
			if(max_vertical_angle < glm::pi<float>())
			{
				int cap_count = (horizontal_count - 2) / 2;
				for(int i = 0; i < cap_count; ++i)
				{
					mesh.elements.push_back(bottom_cap_start + i);
					mesh.elements.push_back(bottom_cap_start + horizontal_count - (i+1));
					mesh.elements.push_back(bottom_cap_start + i + 1);

					mesh.elements.push_back(bottom_cap_start + i + 1);
					mesh.elements.push_back(bottom_cap_start + horizontal_count - (i+1));
					mesh.elements.push_back(bottom_cap_start + horizontal_count - (i+2));
				}
			}
			else
			{
				int bottom_start = mesh.vertices.size() - 1 - horizontal_count;
				for(int i = 0; i < horizontal_count; ++i)
				{
					mesh.elements.push_back(mesh.vertices.size()-1);
					mesh.elements.push_back(bottom_start + (i+1) % horizontal_count);
					mesh.elements.push_back(bottom_start + i);
				}
			}
		}

		return mesh;
	}

	// ---------------------------------------------------------------------------------------------------------------------------------------------------------
	// polygonal mesh
	// ---------------------------------------------------------------------------------------------------------------------------------------------------------
	static polygon_tessellator s_polygon_tessellator;

	void tessellate_polygonal_begin()
	{
		s_polygon_tessellator.begin();
	}

	void tessellate_polygonal_add(const polygon& poly)
	{
		s_polygon_tessellator.add_polygon(poly);
	}

	triangle_mesh tessellate_polygonal_end()
	{
		return s_polygon_tessellator.end();
	}
} // namespace tess
