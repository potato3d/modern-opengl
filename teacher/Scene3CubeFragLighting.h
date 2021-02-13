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

		GLuint vbo;
		glCreateBuffers(1, &vbo);
		glNamedBufferStorage(vbo, vertices.size()*sizeof(BoxVertex), vertices.data(), 0); // flags = 0

		GLuint ebo;
		glCreateBuffers(1, &ebo);
		glNamedBufferStorage(ebo, elements.size()*sizeof(unsigned int), elements.data(), 0); // flags = 0

		// ------------------------------------------------------------------------
		// 3- Setup vertex array object
		// ------------------------------------------------------------------------

		GLuint bufferIndex = 0;

		// create vao
		glCreateVertexArrays(1, &_vao);

		// bind vbo to vao
		glVertexArrayVertexBuffer(_vao, bufferIndex, vbo, 0, sizeof(BoxVertex)); // offset = 0, stride = sizeof(BoxVertex)

		// setup position attrib
		glEnableVertexArrayAttrib(_vao, IN_POSITION);
		glVertexArrayAttribBinding(_vao, IN_POSITION, bufferIndex);
		glVertexArrayAttribFormat(_vao, IN_POSITION, 3, GL_FLOAT, GL_FALSE, 0); // size = 3, normalized = false, offset = 0

		// setup normal attrib
		glEnableVertexArrayAttrib(_vao, IN_NORMAL);
		glVertexArrayAttribBinding(_vao, IN_NORMAL, bufferIndex);
		glVertexArrayAttribFormat(_vao, IN_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(BoxVertex::position)); // size = 3, normalized = false, offset = sizeof(BoxVertex::position)

		// bind ebo
		glVertexArrayElementBuffer(_vao, ebo);

		// ------------------------------------------------------------------------
		// 4- Create shader program
		// ------------------------------------------------------------------------

		ShaderLoader loader;
		if(!loader.addFile(GL_VERTEX_SHADER, "../src/Scene3CubeFragLighting.vert", "../src/ShaderData.h"))
		{
			return false;
		}
		if(!loader.addFile(GL_FRAGMENT_SHADER, "../src/Scene3CubeFragLighting.frag", "../src/ShaderData.h"))
		{
			return false;
		}
		if(!loader.link(_program))
		{
			return false;
		}

		// ------------------------------------------------------------------------
		// 5- Setup storage buffers to store per-instance data
		// ------------------------------------------------------------------------

		_material.diffuse = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		_material.specular = glm::vec4(1.0f, 1.0f, 1.0f, 32.0f);

		glCreateBuffers(1, &_materialSSBO);
		glNamedBufferStorage(_materialSSBO, sizeof(MaterialData), &_material, 0); // flags = 0

		return true;
	}

	const AABB& getBounds()
	{
		return _bounds;
	}

	void draw(const CameraData& cameraData)
	{
		glUseProgram(_program);
		glBindVertexArray(_vao);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SB_MATERIAL, _materialSSBO);
		glDrawElements(GL_TRIANGLES, _numElements, GL_UNSIGNED_INT, 0); // offset = 0
	}

private:
	AABB _bounds;
	unsigned int _numElements;
	GLuint _vao;
	GLuint _program;
	GLuint _materialSSBO;
	MaterialData _material;
};
