#pragma once
#include <tess/triangle_mesh.h>

namespace tess
{
	class mesh_builder
	{
	public:
		mesh_builder();

		void begin();
		void set_primitive_mode(unsigned int mode);
		void add_element(element element);
		unsigned int add_vertex(const vertex& v); // return index of newly added vertex
		const vertex& get_vertex(unsigned int idx);
		triangle_mesh end();

	private:
		void _add_triangle(element last_elem);

		void _add_triangle_fan(element last_elem);

		void _add_triangle_strip(element last_elem);

	private:
		std::vector<vertex>  _vertices;
		std::vector<element> _elements;
		unsigned int _curr_prim_mode;
		element _curr_elements[2]; // keeps track of last two elements of current triangle
		unsigned int _num_curr_elements;
		bool _invert_winding;
	};
} // namespace tess
