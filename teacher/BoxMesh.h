#pragma once
#include <glm/glm.hpp>
#include <vector>

struct BoxVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 texcoord;
};

void getBoxMesh(std::vector<BoxVertex>& vertices, std::vector<unsigned int>& elements)
{
	// vertices are computed as follows
	//     7+------+6
	//     /|     /|      y
	//    / |    / |      |
	//   / 3+---/--+2     |
	// 4+------+5 /       *---x
	//  | /    | /       /
	//  |/     |/       z
	// 0+------+1
	//
	// p0(--+) p4(-++)
	// p1(+-+) p5(+++)
	// p2(+--) p6(++-)
	// p3(---) p7(-+-)

	glm::vec3 p0(-0.5f, -0.5f,  0.5f);
	glm::vec3 p1( 0.5f, -0.5f,  0.5f);
	glm::vec3 p2( 0.5f, -0.5f, -0.5f);
	glm::vec3 p3(-0.5f, -0.5f, -0.5f);
	glm::vec3 p4(-0.5f,  0.5f,  0.5f);
	glm::vec3 p5( 0.5f,  0.5f,  0.5f);
	glm::vec3 p6( 0.5f,  0.5f, -0.5f);
	glm::vec3 p7(-0.5f,  0.5f, -0.5f);

	glm::vec3 unitX(1.0f, 0.0f, 0.0f);
	glm::vec3 unitY(0.0f, 1.0f, 0.0f);
	glm::vec3 unitZ(0.0f, 0.0f, 1.0f);

	// +x
	vertices.push_back({p2, unitX, {0,0,1}, {1,0}});
	vertices.push_back({p6, unitX, {1,1,1}, {1,1}});
	vertices.push_back({p5, unitX, {1,0,0}, {0,1}});
	vertices.push_back({p1, unitX, {0,1,0}, {0,0}});
	// +z
	vertices.push_back({p1, unitZ, {0,0,1}, {1,0}});
	vertices.push_back({p5, unitZ, {1,1,1}, {1,1}});
	vertices.push_back({p4, unitZ, {1,0,0}, {0,1}});
	vertices.push_back({p0, unitZ, {0,1,0}, {0,0}});
	// +y
	vertices.push_back({p5, unitY, {0,0,1}, {1,0}});
	vertices.push_back({p6, unitY, {1,1,1}, {1,1}});
	vertices.push_back({p7, unitY, {1,0,0}, {0,1}});
	vertices.push_back({p4, unitY, {0,1,0}, {0,0}});
	// -x
	vertices.push_back({p0, -unitX, {0,0,1}, {1,0}});
	vertices.push_back({p4, -unitX, {1,1,1}, {1,1}});
	vertices.push_back({p7, -unitX, {1,0,0}, {0,1}});
	vertices.push_back({p3, -unitX, {0,1,0}, {0,0}});
	// -z
	vertices.push_back({p3, -unitZ, {0,0,1}, {1,0}});
	vertices.push_back({p7, -unitZ, {1,1,1}, {1,1}});
	vertices.push_back({p6, -unitZ, {1,0,0}, {0,1}});
	vertices.push_back({p2, -unitZ, {0,1,0}, {0,0}});
	// -y
	vertices.push_back({p0, -unitY, {0,0,1}, {1,0}});
	vertices.push_back({p3, -unitY, {1,1,1}, {1,1}});
	vertices.push_back({p2, -unitY, {1,0,0}, {0,1}});
	vertices.push_back({p1, -unitY, {0,1,0}, {0,0}});

	for(unsigned int i = 0; i < 6; ++i)
	{
		elements.push_back(0+i*4);
		elements.push_back(1+i*4);
		elements.push_back(2+i*4);
		elements.push_back(0+i*4);
		elements.push_back(2+i*4);
		elements.push_back(3+i*4);
	}
}
