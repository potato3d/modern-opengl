#include <FrustumCuller.h>

void FrustumCuller::beginFrame(const glm::mat4& viewProj)
{
	// NOTE: glm uses mat[col][row]
	_setPlane(_data.near, viewProj[0][3] + viewProj[0][2], viewProj[1][3] + viewProj[1][2], viewProj[2][3] + viewProj[2][2], viewProj[3][3] + viewProj[3][2]);   // near
	_setPlane(_data.left, viewProj[0][3] + viewProj[0][0], viewProj[1][3] + viewProj[1][0], viewProj[2][3] + viewProj[2][0], viewProj[3][3] + viewProj[3][0]);   // left
	_setPlane(_data.right, viewProj[0][3] - viewProj[0][0], viewProj[1][3] - viewProj[1][0], viewProj[2][3] - viewProj[2][0], viewProj[3][3] - viewProj[3][0]);  // right
	_setPlane(_data.bottom, viewProj[0][3] + viewProj[0][1], viewProj[1][3] + viewProj[1][1], viewProj[2][3] + viewProj[2][1], viewProj[3][3] + viewProj[3][1]); // bottom
	_setPlane(_data.top, viewProj[0][3] - viewProj[0][1], viewProj[1][3] - viewProj[1][1], viewProj[2][3] - viewProj[2][1], viewProj[3][3] - viewProj[3][1]);    // top
	_setPlane(_data.far, viewProj[0][3] - viewProj[0][2], viewProj[1][3] - viewProj[1][2], viewProj[2][3] - viewProj[2][2], viewProj[3][3] - viewProj[3][2]);    // far
}

void FrustumCuller::_setPlane(Plane& p, float a, float b, float c, float d)
{
	p.nx = a;
	p.ny = b;
	p.nz = c;
	p.offset   = d;
	p.px = (p.nx > 0.0f) ? 1 : 0;
	p.py = (p.ny > 0.0f) ? 1 : 0;
	p.pz = (p.nz > 0.0f) ? 1 : 0;
	float invLen = 1.0f / glm::length(glm::vec3(p.nx, p.ny, p.nz));
	p.nx *= invLen;
	p.ny *= invLen;
	p.nz *= invLen;
	p.offset *= invLen;
}
