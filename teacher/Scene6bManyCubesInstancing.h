#pragma once
#include <GL/glew.h>
#include <AABB.h>
#include <BoxMesh.h>
#include <ShaderData.h>
#include <ShaderLoader.h>
#include <Random.h>

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

		auto randomColor = make_random(0.2f, 1.0f, 123.0f);
		auto randomAngle = make_random(0.0f, glm::radians(360.0f), 123.0f);
		auto randomAxis = make_random(-1.0f, 1.0f, 123.0f);
		auto randomScale = make_random(0.5f, 1.2f, 123.0f);

		unsigned int numX = 20;
		unsigned int numY = 20;
		unsigned int numZ = 20;

		_sceneSize = numX * numY * numZ;

		// pre-allocate exact memory that will be used
		_transforms.reserve(_sceneSize);
		_materials.reserve(_sceneSize);

		// Check if data would fit in a single SSBO
		int maxSSBOSize = 0;
		glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &maxSSBOSize);

		if(_transforms.size()*sizeof(TransformData) > (unsigned int)maxSSBOSize || _materials.size()*sizeof(MaterialData) > (unsigned int)maxSSBOSize)
		{
			return false;
		}

		// Generate the scene
		for(unsigned int z = 0; z < numZ; ++z)
		{
			for(unsigned int y = 0; y < numY; ++y)
			{
				for(unsigned int x = 0; x < numX; ++x)
				{
					_transforms.push_back(glm::vec3(x*2.0f, y*2.0f, z*2.0f));

					MaterialData material;
					material.diffuse = glm::vec4(randomColor(), randomColor(), randomColor(), 1.0f);
					material.specular = glm::vec4(1.0f, 1.0f, 1.0f, 128.0f);
					_materials.push_back(material);

					for(auto v : vertices)
					{
						_bounds.expand(v.position + _transforms.back());
					}
				}
			}
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
		if(!loader.addFile(GL_VERTEX_SHADER, "../src/Scene6bManyCubesInstancing.vert", "../src/ShaderData.h"))
		{
			return false;
		}
		if(!loader.addFile(GL_FRAGMENT_SHADER, "../src/Scene6bManyCubesInstancing.frag", "../src/ShaderData.h"))
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

		glCreateBuffers(1, &_materialsSSBO);
		glNamedBufferStorage(_materialsSSBO, _materials.size()*sizeof(MaterialData), _materials.data(), 0); // flags = 0

		// ------------------------------------------------------------------------
		// 6- Setup transform as an additional vertex attribute inside the same VAO
		// ------------------------------------------------------------------------

		// create buffer and store transforms
		GLuint transformBuffer;
		glCreateBuffers(1, &transformBuffer);
		glNamedBufferStorage(transformBuffer, _transforms.size()*sizeof(vec3), _transforms.data(), 0);

		// increment buffer index inside vao
		++bufferIndex;

		// add transform buffer to vao
		glVertexArrayVertexBuffer(_vao, bufferIndex, transformBuffer, 0, sizeof(vec3)); // offset = 0, stride = sizeof(vec3)

		// enable instancing for entire transform buffer
		glVertexArrayBindingDivisor(_vao, bufferIndex, 1);

		// setup transform attrib
		glEnableVertexArrayAttrib(_vao, IN_TRANSFORM);
		glVertexArrayAttribBinding(_vao, IN_TRANSFORM, bufferIndex);
		glVertexArrayAttribFormat(_vao, IN_TRANSFORM, 3, GL_FLOAT, GL_FALSE, 0); // size = 3, offset = 0

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
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SB_MATERIAL, _materialsSSBO);
		glDrawElementsInstanced(GL_TRIANGLES, _numElements, GL_UNSIGNED_INT, 0, _sceneSize); // offset = 0
	}

private:
	AABB _bounds;
	unsigned int _numElements;
	unsigned int _sceneSize;
	GLuint _vao;
	GLuint _program;
	GLuint _transformsSSBO;
	GLuint _materialsSSBO;
	std::vector<vec3> _transforms;
	std::vector<MaterialData> _materials;
};
