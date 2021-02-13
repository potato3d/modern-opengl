#pragma once
#include <tess/triangle_mesh.h>

namespace tess
{
	class mesh_optimizer
	{
	public:
		typedef unsigned int flag;

		static const flag flag_check_results = 1;              // check results for consistency and print any problems found: O(n^2)
		static const flag flag_all_optimizations = (~0U) << 1; // enable all optimizations: O(n)
		static const flag flag_remove_unused_vertices = 1 << 1; // remove vertices not referenced by any element: O(n)
		static const flag flag_weld_vertices_exact = 1 << 2;    // merge vertices with exactly the same attributes: O(n)
		// todo: static const flag flag_weld_vertices_nearby =  1<<3; // merge nearby vertices using tolerance: O(n)

		void optimize(triangle_mesh& mesh, flag flags);

	private:
		std::vector<vertex>  _temp_vertices;
		std::vector<element> _temp_elements;
	};
} // namespace tess
