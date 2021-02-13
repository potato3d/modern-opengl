#pragma once
#include <GL/glew.h>
#include <AABB.h>
#include <BoxMesh.h>
#include <ShaderData.h>
#include <ShaderLoader.h>
#include <Random.h>
#include <tess/tessellator.h>

class Scene
{
public:
	bool initialize()
	{
		// ------------------------------------------------------------------------
		// 1- Load scene data
		// ------------------------------------------------------------------------

		auto randomColor = make_random(0.2f, 1.0f, 123.0f);
		auto randomAngle = make_random(0.0f, glm::radians(360.0f), 123.0f);
		auto randomAxis = make_random(-1.0f, 1.0f, 123.0f);
		auto randomScale = make_random(0.5f, 1.2f, 123.0f);
		auto randomGeometry = make_random(0, 7, 123);

		auto randomTorusAngle = make_random(0.0f, glm::radians(180.0f), 123.0f);

		unsigned int numX = 20;
		unsigned int numY = 20;
		unsigned int numZ = 20;

		_sceneSize = numX * numY * numZ;

		// pre-allocate exact memory that will be used
		_transforms.reserve(_sceneSize);
		_materials.reserve(_sceneSize);
		_drawCmds.reserve(_sceneSize);

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

					tess::triangle_mesh mesh;

					// decide which geometry to create
					switch(randomGeometry())
					{
					case 0: // box
					{
						mesh = tess::tessellate_box(vec3(1,1,1));
						break;
					}
					case 1: // sphere
					{
						mesh = tess::tessellate_sphere(0.5f);
						break;
					}
					case 2: // cylinder
					{
						mesh = tess::tessellate_cylinder(0.5f, 1.0f);
						break;
					}
					case 3: // dish
					{
						mesh = tess::tessellate_dish(0.5f, 1.0f);
						break;
					}
					case 4: // circular torus
					{
						mesh = tess::tessellate_circular_torus(0.2f, 1.0f, randomTorusAngle());
						break;
					}
					case 5: // rectangular torus
					{
						mesh = tess::tessellate_rectangular_torus(0.2f, 1.0f, 0.5f, randomTorusAngle());
						break;
					}
					case 6: // cone
					{
						mesh = tess::tessellate_cone(0.2f, 0.5f, 1.0f);
						break;
					}
					default:
					case 7: // pyramid
					{
						mesh = tess::tessellate_pyramid(vec2(0.2f), vec2(0.5f), 1.0f);
						break;
					}
					}

					//scene9: create and store draw command

					//scene9: store vertices

					//scene9: store elements

					for(auto v : mesh.vertices)
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
		glNamedBufferStorage(vbo, _sceneVertices.size()*sizeof(tess::vertex), _sceneVertices.data(), 0); // flags = 0

		GLuint ebo;
		glCreateBuffers(1, &ebo);
		glNamedBufferStorage(ebo, _sceneElements.size()*sizeof(tess::element), _sceneElements.data(), 0); // flags = 0

		// ------------------------------------------------------------------------
		// 3- Setup vertex array object
		// ------------------------------------------------------------------------

		GLuint bufferIndex = 0;

		// create vao
		glCreateVertexArrays(1, &_vao);

		// bind vbo to vao
		glVertexArrayVertexBuffer(_vao, bufferIndex, vbo, 0, sizeof(tess::vertex)); // offset = 0, stride = sizeof(TriangleVertex)

		// setup position attrib
		glEnableVertexArrayAttrib(_vao, IN_POSITION);
		glVertexArrayAttribBinding(_vao, IN_POSITION, bufferIndex);
		glVertexArrayAttribFormat(_vao, IN_POSITION, 3, GL_FLOAT, GL_FALSE, 0); // size = 3, normalized = false, offset = 0

		// setup normal attrib
		glEnableVertexArrayAttrib(_vao, IN_NORMAL);
		glVertexArrayAttribBinding(_vao, IN_NORMAL, bufferIndex);
		glVertexArrayAttribFormat(_vao, IN_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(tess::vertex::position)); // size = 3, normalized = false, offset = sizeof(TriangleVertex::position)

		// bind ebo
		glVertexArrayElementBuffer(_vao, ebo);

		// ------------------------------------------------------------------------
		// 4- Create shader program
		// ------------------------------------------------------------------------

		ShaderLoader loader;
		if(!loader.addFile(GL_VERTEX_SHADER, "../src/Scene9ManyGeometriesIndividual.vert", "../src/ShaderData.h"))
		{
			return false;
		}
		if(!loader.addFile(GL_FRAGMENT_SHADER, "../src/Scene9ManyGeometriesIndividual.frag", "../src/ShaderData.h"))
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

		glCreateBuffers(1, &_transformSSBO);
		glNamedBufferStorage(_transformSSBO, sizeof(TransformData), nullptr, GL_DYNAMIC_STORAGE_BIT); // data = nullptr, flags = dynamic (enable buffersubdata)

		glCreateBuffers(1, &_materialSSBO);
		glNamedBufferStorage(_materialSSBO, sizeof(MaterialData), nullptr, GL_DYNAMIC_STORAGE_BIT); // data = nullptr, flags = dynamic (enable buffersubdata)

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
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SB_TRANSFORM, _transformSSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SB_MATERIAL, _materialSSBO);
		for(unsigned int i = 0; i < _sceneSize; ++i)
		{
			glNamedBufferSubData(_transformSSBO, 0, sizeof(TransformData), &_transforms.at(i));
			glNamedBufferSubData(_materialSSBO, 0, sizeof(MaterialData), &_materials.at(i));
			//scene9: draw using indirect command
		}
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
	unsigned int _sceneSize;
	GLuint _vao;
	GLuint _program;
	GLuint _transformSSBO;
	std::vector<TransformData> _transforms;
	GLuint _materialSSBO;
	std::vector<MaterialData> _materials;
	std::vector<DrawCommand> _drawCmds;
	std::vector<tess::vertex> _sceneVertices;
	std::vector<tess::element> _sceneElements;
};
