#include <tess/mesh_builder.h>
#include <stdexcept>

namespace tess
{
	static const unsigned int GL_TRIANGLES = 0x0004;
	static const unsigned int GL_TRIANGLE_STRIP = 0x0005;
	static const unsigned int GL_TRIANGLE_FAN   = 0x0006;

	//----------------------------------------------------------------------------------------------------------------------------------------------------------
	// mesh_builder::public
	//----------------------------------------------------------------------------------------------------------------------------------------------------------

	mesh_builder::mesh_builder() : _curr_prim_mode(GL_TRIANGLES),
								 _num_curr_elements(0),
								 _invert_winding(false)
	{
		_curr_elements[0] = 0;
		_curr_elements[1] = 0;
	}

	void mesh_builder::begin()
	{
		_vertices.clear();
		_vertices.reserve(65535);
		_elements.clear();
		_elements.reserve(65535 * 12);
		_curr_prim_mode = GL_TRIANGLES;
		_curr_elements[0] = 0;
		_curr_elements[1] = 0;
		_num_curr_elements = 0;
		_invert_winding   = false;
	}

	void mesh_builder::set_primitive_mode(unsigned int mode)
	{
		if(mode != GL_TRIANGLES && mode != GL_TRIANGLE_FAN && mode != GL_TRIANGLE_STRIP)
		{
			throw std::invalid_argument("Primitive mode for cad::mesh_builder must be one of: GL_TRIANGLES, GL_TRIANGLE_FAN or GL_TRIANGLE_STRIP.");
		}

		_curr_prim_mode = mode;
		_num_curr_elements = 0;
		_invert_winding   = false;
	}

	void mesh_builder::add_element(element elem)
	{
		// Add the vertex index
		if(_num_curr_elements == 2)
		{
			// If two vertices were previously given, then this vertex completes a triangle
			switch(_curr_prim_mode)
			{
			case GL_TRIANGLES:
				_add_triangle(elem);
				break;
			case GL_TRIANGLE_FAN:
				_add_triangle_fan(elem);
				break;
			case GL_TRIANGLE_STRIP:
				_add_triangle_strip(elem);
				break;
			default:
				throw std::logic_error("Invalid primitive mode stored in cad::mesh_builder.");
			}
		}
		else
		{
			// All modes need at least two vertices
			_curr_elements[_num_curr_elements++] = elem;
		}
	}

	unsigned int mesh_builder::add_vertex(const vertex& v)
	{
		_vertices.push_back(v);
		return static_cast<unsigned int>(_vertices.size() - 1);
	}

	const vertex& mesh_builder::get_vertex(unsigned int idx)
	{
		return _vertices[idx];
	}

	triangle_mesh mesh_builder::end()
	{
		return {_vertices, _elements};
	}

	//----------------------------------------------------------------------------------------------------------------------------------------------------------
	// mesh_builder::private
	//----------------------------------------------------------------------------------------------------------------------------------------------------------

	void mesh_builder::_add_triangle(element last_elem)
	{
		// Three vertices make one unique triangle
		_elements.push_back(_curr_elements[0]);
		_elements.push_back(_curr_elements[1]);
		_elements.push_back(last_elem);
		_num_curr_elements = 0;
	}

	void mesh_builder::_add_triangle_fan(element last_elem)
	{
		_elements.push_back(_curr_elements[0]);
		_elements.push_back(_curr_elements[1]);
		_elements.push_back(last_elem);

		// First vertex is anchor of fan, latest index should make next triangle's first edge.
		// keep _numCurrElements equal to 2
		_curr_elements[1] = last_elem;
	}

	void mesh_builder::_add_triangle_strip(element last_elem)
	{
		// Order of indices depend on current winding
		if(!_invert_winding)
		{
			_elements.push_back(_curr_elements[0]);
			_elements.push_back(_curr_elements[1]);
			_elements.push_back(last_elem);
		}
		else
		{
			_elements.push_back(_curr_elements[0]);
			_elements.push_back(last_elem);
			_elements.push_back(_curr_elements[1]);
		}

		// Last two vertices should make first edge of next triangle
		// keep _numCurrElements equal to 2
		_curr_elements[0] = _curr_elements[1];
		_curr_elements[1] = last_elem;

		// Swap winding
		_invert_winding = !_invert_winding;
	}
} // namespace tess
