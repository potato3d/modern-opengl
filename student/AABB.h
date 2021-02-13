#pragma once
#include <glm/glm.hpp>

struct AABB
{
	glm::vec3 min;
	glm::vec3 max;

	AABB()
	{
		min = glm::vec3( std::numeric_limits<float>::max());
		max = glm::vec3(-std::numeric_limits<float>::max());
	}

	void expand(const glm::vec3& point)
	{
		min = glm::min(min, point);
		max = glm::max(max, point);
	}

	bool valid() const
	{
		return min.x <= max.x && min.y <= max.y && min.z <= max.z;
	}

	const glm::vec3& operator[](unsigned int i) const
	{
		return *(&min + i);
	}
};
