#include <tess/polygon_tessellator.h>
#include <tess/mesh_builder.h>
#include <tess/glutess/glutess_facade.h>
#include <cstring>

namespace tess
{
	//----------------------------------------------------------------------------------------------------------------------------------------------------------
	// Hidden implementation
	//----------------------------------------------------------------------------------------------------------------------------------------------------------

	class polygon_tessellator::impl
	{
	public:
		impl();

		~impl();

		void begin();

		void addPolygon(const polygon& poly);

		triangle_mesh end();

		void begin(unsigned int type);

		void vertex(void* vertex_data);

		void combine(double coords[3], void* vertex_data[4], float weight[4], void** out_data);

		static void beginCB(unsigned int type, void* user_data);

		static void vertexCB(void* vertex_data, void* user_data);

		static void combineCB(double coords[3], void* vertex_data[4], float weight[4], void** out_data, void* user_data);

		GLUTESS_tesselator* _tess_obj;
		mesh_builder _builder;
	};

	//----------------------------------------------------------------------------------------------------------------------------------------------------------
	// polygon_tessellator::public
	//----------------------------------------------------------------------------------------------------------------------------------------------------------

	polygon_tessellator::polygon_tessellator() : _d(new impl())
	{
	}

	polygon_tessellator::~polygon_tessellator()
	{
		delete _d;
	}

	void polygon_tessellator::begin()
	{
		_d->begin();
	}

	void polygon_tessellator::add_polygon(const polygon& poly)
	{
		_d->addPolygon(poly);
	}

	triangle_mesh polygon_tessellator::end()
	{
		return _d->end();
	}

	//----------------------------------------------------------------------------------------------------------------------------------------------------------
	// polygon_tessellator::impl::public
	//----------------------------------------------------------------------------------------------------------------------------------------------------------

	polygon_tessellator::impl::impl() : _tess_obj(nullptr)
	{
	}

	polygon_tessellator::impl::~impl()
	{
		if(_tess_obj != nullptr)
		{
			tessDelete(_tess_obj);
		}
	}

	void polygon_tessellator::impl::begin()
	{
		if(_tess_obj != nullptr)
		{
			tessDelete(_tess_obj);
		}

		_tess_obj = tessNew();
		tessCallback(_tess_obj, GLUTESS_BEGIN_DATA, reinterpret_cast<GLUTESS_CALLBACK>(&polygon_tessellator::impl::beginCB));
		tessCallback(_tess_obj, GLUTESS_VERTEX_DATA, reinterpret_cast<GLUTESS_CALLBACK>(&polygon_tessellator::impl::vertexCB));
		tessCallback(_tess_obj, GLUTESS_COMBINE_DATA, reinterpret_cast<GLUTESS_CALLBACK>(&polygon_tessellator::impl::combineCB));

		_builder.begin();
	}

	void polygon_tessellator::impl::addPolygon(const polygon& poly)
	{
		// Tesselate the polygon
		tessBeginPolygon(_tess_obj, this);

		for(unsigned int iContour = 0; iContour < poly.contours.size(); ++iContour)
		{
			const contour& cont = poly.contours[iContour];
			tessBeginContour(_tess_obj);

			for(unsigned int iPoint = 0; iPoint < cont.points.size(); ++iPoint)
			{
				// get contour point
				const point& p = cont.points[iPoint];

				// convert to vertex and store it
				tess::vertex v(p.vertex, p.normal);
				unsigned int idx = _builder.add_vertex(v);

				// convert float to double to send to tesselator
				double coords[] = {v.position.x, v.position.y, v.position.z};

				// last parameter is the index in _resultVertices array, used to recover the i-th vertex from tesselator.
				// the libtess API actually expects a pointer, but since we are using std::vector's, they may get reallocated when growing.
				// this will invalidate the pointers that we previously passed to libtess.
				// so we are hacking unsigned int's into void* pointers to store the vertex indices instead of the pointers.
				tessVertex(_tess_obj, coords, reinterpret_cast<void*>(idx));
			}

			tessEndContour(_tess_obj);
		}

		tessEndPolygon(_tess_obj);
	}

	triangle_mesh polygon_tessellator::impl::end()
	{
		return _builder.end();
	}

	void polygon_tessellator::impl::begin(unsigned int type)
	{
		// Begin new draw method
		_builder.set_primitive_mode(type);
	}

	void polygon_tessellator::impl::vertex(void* vertex_data)
	{
		// Ref: http://blog.regehr.org/archives/959
		element e;
		std::memcpy(&e, &vertex_data, sizeof(element));

		// Find the offset of the vertex in the vertex array
		//cad::MeshElement element = *reinterpret_cast<cad::MeshElement*>(&vertexData);

		// Add index to triangles mode converter
		_builder.add_element(e);
	}

	void polygon_tessellator::impl::combine(double coords[3], void* vertex_data[4], float weight[4], void** out_data)
	{
		// We're going to create a new vertex/normal pair in the arrays
		tess::vertex v;

		// The new vertex's coordinates are already supplied by TESS
		v.position.x = static_cast<float>(coords[0]);
		v.position.y = static_cast<float>(coords[1]);
		v.position.z = static_cast<float>(coords[2]);

		// The new vertex's normal needs to be interpolated from four existing vertices
		for(int i = 0; i < 4; ++i)
		{
			// according to libglutess source, here vertexData[i] could be NULL.
			// but in our scheme, that can also mean the vertex with index 0.
			// so we are always considering 0 as a valid value and hoping that the corresponding weights will also be 0 if what libglutess really wanted was
			//   for us to ignore vertexData[i].
			// see SpliceMergeVertices() in sweep.c in libglutess
			element index = *reinterpret_cast<element*>(&vertex_data[i]);
			const tess::vertex& other_vertex = _builder.get_vertex(index);

			v.normal.x += weight[i] * other_vertex.normal.x;
			v.normal.y += weight[i] * other_vertex.normal.y;
			v.normal.z += weight[i] * other_vertex.normal.z;
		}

		// Make sure it is normalized
		v.normal = normalize(v.normal);

		// OK, now add the vertex in the arrays
		element idx = static_cast<element>(_builder.add_vertex(v));

		// ...and inform TESS about its void* data pointer (i.e. the index of the newly added vertex in the array)
		*out_data = reinterpret_cast<void*>(idx);
	}

	void polygon_tessellator::impl::beginCB(unsigned int type, void* user_data)
	{
		static_cast<polygon_tessellator::impl*>(user_data)->begin(type);
	}

	void polygon_tessellator::impl::vertexCB(void* vertex_data, void* user_data)
	{
		static_cast<polygon_tessellator::impl*>(user_data)->vertex(vertex_data);
	}

	void polygon_tessellator::impl::combineCB(double coords[3], void* vertex_data[4], float weight[4], void** out_data, void* user_data)
	{
		static_cast<polygon_tessellator::impl*>(user_data)->combine(coords, vertex_data, weight, out_data);
	}
} // namespace tess
