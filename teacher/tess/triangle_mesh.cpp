#include <tess/triangle_mesh.h>

namespace tess
{
	// ---------------------------------------------------------------------------------------------------------------------------------------------------------
	// vertex
	// ---------------------------------------------------------------------------------------------------------------------------------------------------------

	vertex::vertex()
	{
	}

	vertex::vertex(const vec3& _position, const vec3& _normal)
		: position(_position), normal(_normal)
	{
	}

	bool vertex::operator==(const vertex& other) const
	{
		return position == other.position && normal == other.normal;
	}

	// ---------------------------------------------------------------------------------------------------------------------------------------------------------
	// triangle_mesh
	// ---------------------------------------------------------------------------------------------------------------------------------------------------------

	bool triangle_mesh::is_valid() const
	{
		// Check if there are at least 3 vertices and 3 elements to define a triangle
		if(vertices.size() < 3 || elements.size() < 3)
		{
			return false;
		}

		// Check if mesh has at least one valid triangle
		for(unsigned int i = 0; i < elements.size() - 3; i += 3)
		{
			if(elements[i] != elements[i+1] && elements[i+1] != elements[i+2])
			{
				if(elements[i] >= vertices.size() || elements[i+1] >= vertices.size() || elements[i+2] >= vertices.size())
				{
					continue;
				}

				const auto& v1 = vertices[elements[i]].position;
				const auto& v2 = vertices[elements[i+1]].position;
				const auto& v3 = vertices[elements[i+2]].position;

				if(!std::isfinite(v1.x) || !std::isfinite(v1.y) || !std::isfinite(v1.z) ||
				   !std::isfinite(v2.x) || !std::isfinite(v2.y) || !std::isfinite(v2.z) ||
				   !std::isfinite(v3.x) || !std::isfinite(v3.y) || !std::isfinite(v3.z))
				{
					continue;
				}

				if(v1 == v2 || v2 == v3 || v1 == v3)
				{
					continue;
				}

				return true;
			}
		}

		return false;
	}
} // namespace tess
