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
					glm::mat4 s = glm::scale(glm::mat4(), glm::vec3(randomScale(), randomScale(), randomScale()));
					glm::mat4 r = glm::rotate(glm::mat4(), randomAngle(), glm::vec3(randomAxis(), randomAxis(), randomAxis()));
					glm::mat4 t = glm::translate(glm::mat4(), glm::vec3(x*2.0f, y*2.0f, z*2.0f));
					glm::mat4 m4 = t * r * s;
					_transforms.push_back(toTransform(m4));

					MaterialData material;
					material.diffuse = glm::vec4(randomColor(), randomColor(), randomColor(), 1.0f);
					material.specular = glm::vec4(1.0f, 1.0f, 1.0f, 128.0f);
					_materials.push_back(material);

					for(auto v : vertices)
					{
						_bounds.expand(glm::vec3(m4 * glm::vec4(v.position, 1.0f)));
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
		if(!loader.addFile(GL_VERTEX_SHADER, "../src/Scene8ManyCubesInstancingTexture.vert", "../src/ShaderData.h"))
		{
			return false;
		}
		if(!loader.addFile(GL_FRAGMENT_SHADER, "../src/Scene8ManyCubesInstancingTexture.frag", "../src/ShaderData.h", 450, "#extension GL_ARB_bindless_texture : enable"))
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

		glCreateBuffers(1, &_transformsSSBO);
		glNamedBufferStorage(_transformsSSBO, _transforms.size()*sizeof(TransformData), _transforms.data(), 0); // flags = 0

		glCreateBuffers(1, &_materialsSSBO);
		glNamedBufferStorage(_materialsSSBO, _materials.size()*sizeof(MaterialData), _materials.data(), 0); // flags = 0

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

		// create texture objects
		_textures.resize(_transforms.size());
		glCreateTextures(GL_TEXTURE_2D, _textures.size(), _textures.data());

		for(auto t : _textures)
		{
			// allocate immutable memory
			glTextureStorage2D(t, 1, GL_RGBA8, width, height); // levels = 1 (no mipmaps)

			// transfer data to GPU
			glTextureSubImage2D(t, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, texels); // level = 0, xoffset = 0, yoffset = 0
		}

		// ------------------------------------------------------------------------
		// 7- Create sampler to configure how texture will be read inside shader
		// ------------------------------------------------------------------------

		glCreateSamplers(1, &_textureSampler);
		glSamplerParameteri(_textureSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glSamplerParameteri(_textureSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glSamplerParameteri(_textureSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(_textureSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// ------------------------------------------------------------------------
		// 8- Create bindless texture handles
		// ------------------------------------------------------------------------

		//scene8: get bindless texture handles from the combination of texture + sampler

		//scene8: create SSBO to store texture handles

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
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SB_TRANSFORM, _transformsSSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SB_MATERIAL, _materialsSSBO);

		//scene8: bind texture handle ssbo

		glDrawElementsInstanced(GL_TRIANGLES, _numElements, GL_UNSIGNED_INT, 0, _sceneSize); // offset = 0
	}

private:
	TransformData toTransform(const glm::mat4& m)
	{
		TransformData t;

		// last row of mat4 is always 0,0,0,1 (affine transform)
		// glm uses m[col][row]
		t.row0[0] = m[0][0];
		t.row0[1] = m[1][0];
		t.row0[2] = m[2][0];
		t.row0[3] = m[3][0];

		t.row1[0] = m[0][1];
		t.row1[1] = m[1][1];
		t.row1[2] = m[2][1];
		t.row1[3] = m[3][1];

		t.row2[0] = m[0][2];
		t.row2[1] = m[1][2];
		t.row2[2] = m[2][2];
		t.row2[3] = m[3][2];

		return t;
	}

	AABB _bounds;
	unsigned int _numElements;
	unsigned int _sceneSize;
	GLuint _vao;
	GLuint _program;
	GLuint _transformsSSBO;
	std::vector<TransformData> _transforms;
	GLuint _materialsSSBO;
	std::vector<MaterialData> _materials;
	std::vector<GLuint> _textures;
	GLuint _textureSampler;
	std::vector<GLuint64> _bindlessTextureHandles;
	GLuint _bindlessTextureSSBO;
};
