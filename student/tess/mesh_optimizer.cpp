#include <tess/mesh_optimizer.h>
#include <unordered_map>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------------------------------------------
// functor to compute hash value of a vertex
//----------------------------------------------------------------------------------------------------------------------------------------------------------

namespace std
{
	template<>
	class hash<tess::vertex>
	{
	public:
		size_t operator()(const tess::vertex& v) const
		{
			const unsigned int* h = reinterpret_cast<const unsigned int*>(&v);
			unsigned int f = (h[0] + h[1] * 11 - (h[2] * 17) + h[3] * 7 + h[4] * 3 - h[5] * 13) & 0x7fffffff; // mask to avoid problems with +-0
			return (f >> 22) ^ (f >> 12) ^ (f);
		}
	};
} // namespace std

namespace tess
{
	//----------------------------------------------------------------------------------------------------------------------------------------------------------
	// global constants
	//----------------------------------------------------------------------------------------------------------------------------------------------------------

	static const element INVALID_ELEMENT = std::numeric_limits<element>::max();

	//----------------------------------------------------------------------------------------------------------------------------------------------------------
	// public
	//----------------------------------------------------------------------------------------------------------------------------------------------------------

	void mesh_optimizer::optimize(triangle_mesh& mesh, flag flags)
	{
		if(!mesh.is_valid())
		{
			return;
		}

		unsigned int original_vertex_count  = mesh.vertices.size();
		unsigned int original_element_count = mesh.elements.size();

		if(flags & flag_remove_unused_vertices)
		{
			// Store vertices that are actually referenced by an element
			_temp_vertices.clear();
			_temp_vertices.reserve(mesh.vertices.size());

			// Element cross-reference table: for a given element of incoming value X, what is its updated value Y?
			_temp_elements.clear();
			_temp_elements.resize(mesh.vertices.size());
			std::fill(std::begin(_temp_elements), std::end(_temp_elements), INVALID_ELEMENT);

			for(unsigned int i = 0; i < mesh.elements.size(); ++i)
			{
				auto e = mesh.elements[i];

				// If its the first time we encounter this element value
				if(_temp_elements[e] == INVALID_ELEMENT)
				{
					// Get position where corresponding vertex will be inserted (i.e. the new element value)
					auto pos = static_cast<element>(_temp_vertices.size());
					// Save vertex
					_temp_vertices.push_back(mesh.vertices[e]);
					// Update element value
					mesh.elements[i] = pos;
					// Save new element value in cross-reference table
					_temp_elements[e] = pos;
				}
				else
				{
					// We already encontered this element value, just update to its new value using cross-reference table
					mesh.elements[i] = _temp_elements[e];
				}
			}

			// Copy unique vertices to result
			mesh.vertices = _temp_vertices;
			// Elements are already updated
		}

		if(flags & flag_weld_vertices_exact)
		{
			// Store unique vertices (with different attributes)
			_temp_vertices.clear();
			_temp_vertices.reserve(mesh.vertices.size());
			// Element cross-reference table: for a given element of incoming value X, what is its updated value Y?
			_temp_elements.clear();
			_temp_elements.resize(mesh.vertices.size());
			// Quickly determine if, for a given vertex, already exists another with the exact same attributes
			// key = vertex
			// value = final vertex index (final element value to point to corresponding vertex)
			std::unordered_map<vertex, element> hashTable;

			for(unsigned int i = 0; i < mesh.vertices.size(); ++i)
			{
				const auto& v = mesh.vertices[i];
				// Get position where vertex will be inserted if it is unique (i.e. the new element value)
				auto pos = static_cast<element>(_temp_vertices.size());
				auto ret = hashTable.emplace(v, pos);

				// If vertex was successfully inserted in hash table (i.e. is a unique vertex)
				if(ret.second)
				{
					// Save unique vertex
					_temp_vertices.push_back(v);
					// Save new element value in cross-reference table
					_temp_elements[i] = pos;
				}
				else
				{
					// Save exising element value in cross-reference table
					_temp_elements[i] = ret.first->second;
				}
			}

			// Copy unique vertices to result
			mesh.vertices = _temp_vertices;

			// Translate elements from old to new values
			for(unsigned int i = 0; i < mesh.elements.size(); ++i)
			{
				mesh.elements[i] = _temp_elements[mesh.elements[i]];
			}
		}

		if(flags & flag_check_results)
		{
			unsigned int ndup = 0;

			for(unsigned int i = 0; i < mesh.vertices.size(); ++i)
			{
				for(unsigned int j = i + 1; j < mesh.vertices.size(); ++j)
				{
					if(mesh.vertices[i] == mesh.vertices[j])
					{
						++ndup;
					}
				}
			}

			if(ndup > 0)
			{
				std::cout << "tess::mesh_optimizer: " << ndup << " duplicate vertices found!" << std::endl;
			}

			for(unsigned int i = 0; i < mesh.elements.size(); ++i)
			{
				if(mesh.elements[i] >= mesh.vertices.size())
				{
					std::cout << "tess::mesh_optimizer: elements[" << i << "] = " << mesh.elements[i] << " out of range in vertex array (size: " <<
					mesh.vertices.size() << ")" << std::endl;
				}
			}

			if(mesh.vertices.size() > original_vertex_count)
			{
				std::cout << "tess::mesh_optimizer: actual number of vertices increased from " << original_vertex_count << " to " << mesh.vertices.size() <<
				std::endl;
			}

			if(mesh.vertices.size() > original_element_count)
			{
				std::cout << "tess::mesh_optimizer: actual number of elements increased from " << original_element_count << " to " << mesh.elements.size() <<
				std::endl;
			}
		}
	}
} // namespace tess
