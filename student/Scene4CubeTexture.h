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

		// setup texcoord attrib
		glEnableVertexArrayAttrib(_vao, IN_TEXCOORD);
		glVertexArrayAttribBinding(_vao, IN_TEXCOORD, bufferIndex);
		glVertexArrayAttribFormat(_vao, IN_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(BoxVertex::position) + sizeof(BoxVertex::normal)+ sizeof(BoxVertex::color)); // size = 3, normalized = false, offset = sizeof(BoxVertex::position) + sizeof(BoxVertex::normal) + sizeof(BoxVertex::color)

		// bind ebo
		glVertexArrayElementBuffer(_vao, ebo);

		// ------------------------------------------------------------------------
		// 4- Create shader program
		// ------------------------------------------------------------------------

		ShaderLoader loader;
		if(!loader.addFile(GL_VERTEX_SHADER, "../src/Scene4CubeTexture.vert", "../src/ShaderData.h"))
		{
			return false;
		}
		if(!loader.addFile(GL_FRAGMENT_SHADER, "../src/Scene4CubeTexture.frag", "../src/ShaderData.h"))
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

		// ------------------------------------------------------------------------
		// 6- Create texture
		// ------------------------------------------------------------------------
		GLubyte texels[4*4*4] = // 4 rows x 4 columns x 4 coords per texel
		{
			 0,   0,   0,   255,
			 255, 255,  255,   255,
			 0,   0,   0, 255,
			255, 255,  255,   255,

			255, 255,  255,   255,
			0,   0,   0,   255,
			255, 255,  255,   255,
			0,   0,   0, 255,

			0,   0,   0,   255,
			255, 255,  255,   255,
			0,   0,   0, 255,
		   255, 255,  255,   255,

			255, 255,  255,   255,
			0,   0,   0,   255,
			255, 255,  255,   255,
			0,   0,   0, 255,
		};
		GLsizei width = 4;
		GLsizei height = 4;
		GLsizei numLevels = 1 + std::floor(std::log2(std::max(width, height)));

		//scene4: create texture object

		//scene4: allocate immutable memory (including mipmaps)

		//scene4: transfer data to GPU

		//scene4: generate the mipmaps

		// ------------------------------------------------------------------------
		// 7- Create sampler to configure how texture will be read inside shader
		// ------------------------------------------------------------------------

		//scene4: create sampler

		//scene4: set wrap

		//scene4: set filter

		//scene4: test with anisotropic filtering

		return true;
	}

	const AABB& getBounds()
	{
		return _bounds;
	}

	void draw(const CameraData& /*cameraData*/)
	{
		glUseProgram(_program);
		glBindVertexArray(_vao);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SB_MATERIAL, _materialSSBO);

		//scene4: bind sampler
		//scene4: bind texture

		glDrawElements(GL_TRIANGLES, _numElements, GL_UNSIGNED_INT, 0); // offset = 0
	}

private:
	AABB _bounds;
	unsigned int _numElements;
	MaterialData _material;
	GLuint _vao;
	GLuint _program;
	GLuint _materialSSBO;
	GLuint _texture;
	GLuint _textureSampler;
};
