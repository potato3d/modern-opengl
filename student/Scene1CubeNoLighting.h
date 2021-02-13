#pragma once
#include <GL/glew.h>
#include <AABB.h>
#include <BoxMesh.h>
#include <ShaderData.h>
#include <ShaderLoader.h>

class Scene
{
public:
    bool initialize()
    {
		// ------------------------------------------------------------------------
		// 1- Load scene data
		// ------------------------------------------------------------------------

		std::vector<BoxVertex> vertices;
		std::vector<unsigned int> elements;
		getBoxMesh(vertices, elements);

		_numElements = elements.size();

		for(auto v : vertices)
		{
			_bounds.expand(v.position);
		}

		// ------------------------------------------------------------------------
		// 2- Create buffers and transfer data to GPU
		// ------------------------------------------------------------------------

		//scene1: create vbo

		//scene1: create ebo

		// ------------------------------------------------------------------------
		// 3- Setup vertex array object
		// ------------------------------------------------------------------------

		//scene1: create vao

		//scene1: bind vbo to vao

		//scene1: setup position attrib

		//scene1: setup color attrib

		//scene1: bind ebo

		// ------------------------------------------------------------------------
		// 4- Create shader program
		// ------------------------------------------------------------------------

		ShaderLoader loader;
		if(!loader.addFile(GL_VERTEX_SHADER, "../src/Scene1CubeNoLighting.vert", "../src/ShaderData.h"))
		{
			return false;
		}
		if(!loader.addFile(GL_FRAGMENT_SHADER, "../src/Scene1CubeNoLighting.frag", "../src/ShaderData.h"))
		{
			return false;
		}
		if(!loader.link(_program))
		{
			return false;
		}

        return true;
    }

    const AABB& getBounds()
    {
        return _bounds;
    }

	void draw(const CameraData& /*cameraData*/)
    {
		//scene1: use program
		//scene1: bind vao
		//scene1: draw
    }

private:
	AABB _bounds;
	unsigned int _numElements;
	GLuint _vao;
	GLuint _program;
};
