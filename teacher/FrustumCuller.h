#pragma once
#include <AABB.h>
#include <ShaderData.h>

class FrustumCuller
{
public:
	void beginFrame(const glm::mat4& viewProj);
	inline const FrustumData& getData() const;
	inline bool isCulled(const AABB& bounds) const;

private:
	void _setPlane(Plane& p, float a, float b, float c, float d);
	inline bool _isCulled(const Plane& p, const AABB& bounds) const;

private:
	FrustumData _data;
};

inline const FrustumData& FrustumCuller::getData() const
{
	return _data;
}

inline bool FrustumCuller::isCulled(const AABB& bounds) const
{
	return  _isCulled(_data.near, bounds) ||
	        _isCulled(_data.left, bounds) ||
	        _isCulled(_data.right, bounds) ||
	        _isCulled(_data.bottom, bounds) ||
	        _isCulled(_data.top, bounds) ||
	        _isCulled(_data.far, bounds);
}

inline bool FrustumCuller::_isCulled(const Plane& p, const AABB& bounds) const
{
	return glm::dot(glm::vec3(p.nx, p.ny, p.nz), glm::vec3(bounds[p.px].x, bounds[p.py].y, bounds[p.pz].z)) < -p.offset;
}
