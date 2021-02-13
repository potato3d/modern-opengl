#pragma once
#include <tess/tessellator.h>

namespace tess
{
	class polygon_tessellator
	{
	public:
		polygon_tessellator();
		~polygon_tessellator();

		void begin();
		void add_polygon(const polygon& poly);
		triangle_mesh end();

	private:
		polygon_tessellator(const polygon_tessellator&) = delete;
		polygon_tessellator& operator=(const polygon_tessellator&) = delete;

		class impl;
		impl* _d;
	};
} // namespace tess
