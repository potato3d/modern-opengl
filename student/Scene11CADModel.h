#pragma once
#include <GL/glew.h>
#include <AABB.h>
#include <BoxMesh.h>
#include <ShaderData.h>
#include <ShaderLoader.h>
#include <Random.h>
#include <rvm/FileReader.h>
#include <rvm/StatsCollector.h>
#include <tess/tessellator.h>
#include <tess/mesh_optimizer.h>

struct ModelData
{
	AABB bounds;

	GLuint vao;
	GLuint transformsSSBO;
	std::vector<TransformData> transforms;
	GLuint materialsSSBO;
	std::vector<MaterialData> materials;

	GLuint drawCmdsBuffer;
	std::vector<DrawCommand> drawCmds;
	std::vector<tess::vertex> vertices;
	std::vector<tess::element> elements;

	GLuint program;
};

class ModelLoader : public rvm::FileReader::IObserver
{
public:
	ModelLoader(ModelData* model) {_model = model;}

	virtual void validPrimitive(const rvm::Box& b)
	{
		//scene11:
		//storeMesh(tess::tessellate_box(glm::make_vec3(b.lengths)), glm::make_mat4(b.transform));
	}

	virtual void validPrimitive(const rvm::Sphere& s)
	{
		//scene11:
		//storeMesh(tess::tessellate_sphere(s.radius), glm::make_mat4(s.transform));
	}

	virtual void validPrimitive(const rvm::Cylinder& c)
	{
		//scene11:
		//storeMesh(tess::tessellate_cylinder(c.radius, c.height), glm::make_mat4(c.transform));
	}

	virtual void validPrimitive(const rvm::Dish& d)
	{
		//scene11:
		//storeMesh(tess::tessellate_dish(d.radius, d.height), glm::make_mat4(d.transform));
	}

	virtual void validPrimitive(const rvm::Pyramid& p)
	{
		//scene11:
		//storeMesh(tess::tessellate_pyramid(glm::make_vec2(p.topLengths), glm::make_vec2(p.bottomLengths), p.height, glm::make_vec2(p.offset)), glm::make_mat4(p.transform));
	}

	virtual void validPrimitive(const rvm::RectangularTorus& t)
	{
		//scene11:
		//storeMesh(tess::tessellate_rectangular_torus(t.internalRadius, t.externalRadius, t.height, t.sweepAngle), glm::make_mat4(t.transform));
	}

	virtual void validPrimitive(const rvm::CircularTorus& t)
	{
		//scene11:
		//storeMesh(tess::tessellate_circular_torus(t.internalRadius, t.externalRadius, t.sweepAngle), glm::make_mat4(t.transform));
	}

	virtual void validPrimitive(const rvm::Cone& c)
	{
		//scene11:
		//storeMesh(tess::tessellate_cone(c.radiusTop, c.radiusBottom, c.height),  glm::make_mat4(c.transform));
	}

	virtual void validPrimitive(const rvm::SlopedCone& c)
	{
		//scene11:
		//storeMesh(tess::tessellate_cone_slope_offset(c.radiusTop, c.radiusBottom, c.height, glm::make_vec2(c.topSlopeAngle), glm::make_vec2(c.bottomSlopeAngle), glm::make_vec2(c.offset)), glm::make_mat4(c.transform));
	}

	virtual void validPrimitive(const rvm::Mesh& mesh)
	{
		//scene11:
//		tess::tessellate_polygonal_begin();

//		for(const auto& face : mesh.faces)
//		{
//			tess::polygon tpoly;
//			tpoly.contours.reserve(face.polygons.size());

//			for(const auto& poly : face.polygons)
//			{
//				tess::contour tcontour;
//				tcontour.points.reserve(poly.points.size());

//				for(const auto& point : poly.points)
//				{
//					tess::point tpoint;
//					tpoint.vertex = make_vec3(point.vertex);
//					tpoint.normal = make_vec3(point.normal);
//					tcontour.points.push_back(tpoint);
//				}

//				tpoly.contours.push_back(tcontour);
//			}

//			tess::tessellate_polygonal_add(tpoly);
//		}

//		auto data = tess::tessellate_polygonal_end();

//		if(!data.is_valid())
//		{
//			return;
//		}

//		tess::mesh_optimizer opt;
//		opt.optimize(data, tess::mesh_optimizer::flag_all_optimizations);

//		storeMesh(data, make_mat4(mesh.transform));
	}

	virtual void beginBlock(rvm::CntBegin& block)
	{
		//scene11:
//		rvm::Material m = _materials.getMaterial(block.colorCode);
//		_currMaterial.diffuse = glm::make_vec4(m.diffuseColor);
//		_currMaterial.specular = glm::make_vec4(m.specularColor);
//		_currMaterial.specular.w = m.shininess;
	}

private:
	void storeMesh(const tess::triangle_mesh& mesh, const glm::mat4& m4)
	{
		//scene11:
//		_model->transforms.push_back(toTransform(m4));

//		DrawCommand drawCmd;
//		drawCmd.elementCount = mesh.elements.size();
//		drawCmd.instanceCount = 1;
//		drawCmd.firstElement = _model->elements.size();
//		drawCmd.baseVertex = _model->vertices.size();
//		drawCmd.baseInstance = _model->drawCmds.size(); // automatically fetch the drawID instanced attribute
//		_model->drawCmds.push_back(drawCmd);

//		_model->vertices.insert(_model->vertices.end(), mesh.vertices.begin(), mesh.vertices.end());
//		_model->elements.insert(_model->elements.end(), mesh.elements.begin(), mesh.elements.end());

//		for(auto v : mesh.vertices)
//		{
//			_model->bounds.expand(glm::vec3(m4 * glm::vec4(v.position, 1.0f)));
//		}

//		_model->materials.push_back(_currMaterial);
	}

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

	ModelData* _model;
	rvm::MaterialTable _materials;
	MaterialData _currMaterial;
};

class Scene
{
public:
	bool initialize()
	{
		// ------------------------------------------------------------------------
		// 1- Load scene data
		// ------------------------------------------------------------------------

		std::string basepath = "C:/Users/psantos/Downloads/";

		std::vector<std::string> filepaths;
		filepaths.push_back(basepath + "U-2400-CIV.rvm");
		filepaths.push_back(basepath + "U-2400-ELE.rvm");
		filepaths.push_back(basepath + "U-2400-EQU.rvm");
		filepaths.push_back(basepath + "U-2400-EST.rvm");
		filepaths.push_back(basepath + "U-2400-INS.rvm");
		filepaths.push_back(basepath + "U-2400-SEG.rvm");
		filepaths.push_back(basepath + "U-2400-TUB.rvm");
		filepaths.push_back(basepath + "U-2400-VAC.rvm");

		ModelLoader modelLoader(&_model);
		rvm::FileReader reader;

		for(const auto& path : filepaths)
		{
			std::cout << "Loading " + path + "... "; std::cout.flush();
			reader.readFile(path.data(), &modelLoader);
			std::cout << "done!" << std::endl;
		}

		// ------------------------------------------------------------------------
		// 2- Create buffers and transfer data to GPU
		// ------------------------------------------------------------------------

		//scene11:
//		GLuint vbo;
//		glCreateBuffers(1, &vbo);
//		glNamedBufferStorage(vbo, _model.vertices.size()*sizeof(tess::vertex), _model.vertices.data(), 0); // flags = 0

		//scene11:
//		GLuint ebo;
//		glCreateBuffers(1, &ebo);
//		glNamedBufferStorage(ebo, _model.elements.size()*sizeof(tess::element), _model.elements.data(), 0); // flags = 0

		// ------------------------------------------------------------------------
		// 3- Setup vertex array object
		// ------------------------------------------------------------------------

		//scene11:
//		GLuint bufferIndex = 0;

//		// create vao
//		glCreateVertexArrays(1, &_model.vao);

//		// bind vbo to vao
//		glVertexArrayVertexBuffer(_model.vao, bufferIndex, vbo, 0, sizeof(tess::vertex)); // offset = 0, stride = sizeof(tess::vertex)

//		// setup position attrib
//		glEnableVertexArrayAttrib(_model.vao, IN_POSITION);
//		glVertexArrayAttribBinding(_model.vao, IN_POSITION, bufferIndex);
//		glVertexArrayAttribFormat(_model.vao, IN_POSITION, 3, GL_FLOAT, GL_FALSE, 0); // size = 3, normalized = false, offset = 0

//		// setup normal attrib
//		glEnableVertexArrayAttrib(_model.vao, IN_NORMAL);
//		glVertexArrayAttribBinding(_model.vao, IN_NORMAL, bufferIndex);
//		glVertexArrayAttribFormat(_model.vao, IN_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(tess::vertex::position)); // size = 3, normalized = false, offset = sizeof(tess::vertex::position)

//		// bind ebo
//		glVertexArrayElementBuffer(_model.vao, ebo);

		// ------------------------------------------------------------------------
		// 4- Create shader program
		// ------------------------------------------------------------------------

		ShaderLoader loader;
		if(!loader.addFile(GL_VERTEX_SHADER, "../src/Scene11CADModel.vert", "../src/ShaderData.h"))
		{
			return false;
		}
		if(!loader.addFile(GL_FRAGMENT_SHADER, "../src/Scene11CADModel.frag", "../src/ShaderData.h"))
		{
			return false;
		}
		if(!loader.link(_model.program))
		{
			return false;
		}

		// ------------------------------------------------------------------------
		// 5- Setup storage buffers to store per-instance data
		// ------------------------------------------------------------------------

		//scene11:
//		glCreateBuffers(1, &_model.transformsSSBO);
//		glNamedBufferStorage(_model.transformsSSBO, _model.transforms.size()*sizeof(TransformData), _model.transforms.data(), 0); // flags = 0

//		glCreateBuffers(1, &_model.materialsSSBO);
//		glNamedBufferStorage(_model.materialsSSBO, _model.materials.size()*sizeof(MaterialData), _model.materials.data(), 0); // flags = 0

		// ------------------------------------------------------------------------
		// 6- Setup draw command buffer
		// ------------------------------------------------------------------------

		//scene11:
//		glCreateBuffers(1, &_model.drawCmdsBuffer);
//		glNamedBufferStorage(_model.drawCmdsBuffer, _model.drawCmds.size()*sizeof(DrawCommand), _model.drawCmds.data(), 0); // flags = 0

		// ------------------------------------------------------------------------
		// 7- Setup custom draw ID
		// ------------------------------------------------------------------------

		//scene11:
//		std::vector<int> drawIDs(_model.drawCmds.size());
//		for(unsigned int i = 0; i < drawIDs.size(); ++i)
//		{
//			drawIDs[i] = i;
//		}

//		GLuint drawIdBuffer = 0;

//		glCreateBuffers(1, &drawIdBuffer);
//		glNamedBufferStorage(drawIdBuffer, drawIDs.size()*sizeof(int), drawIDs.data(), 0); // flags = 0

//		// setup drawID as an additional vertex attribute with instancing enabled

//		// use another binding index inside the same vao used to store scene geometry
//		++bufferIndex;

//		// bind drawID buffer to vao
//		glVertexArrayVertexBuffer(_model.vao, bufferIndex, drawIdBuffer, 0, sizeof(int)); // offset = 0, stride = sizeof(int)

//		// enable instancing (this is for the entire vertex buffer and not just for the specific drawID attrib)
//		glVertexArrayBindingDivisor(_model.vao, bufferIndex, 1);

//		// setup drawID attrib
//		glEnableVertexArrayAttrib(_model.vao, IN_DRAWID);
//		glVertexArrayAttribBinding(_model.vao, IN_DRAWID, bufferIndex);
//		glVertexArrayAttribIFormat(_model.vao, IN_DRAWID, 1, GL_INT, 0); // size = 1, offset = 0

		return true;
	}

	const AABB& getBounds()
	{
		return _model.bounds;
	}

	void draw(const CameraData& /*cameraData*/)
	{
		//scene11:
//		glUseProgram(_model.program);
//		glBindVertexArray(_model.vao);
//		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SB_TRANSFORM, _model.transformsSSBO);
//		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SB_MATERIAL, _model.materialsSSBO);
//		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _model.drawCmdsBuffer);
//		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, _model.drawCmds.size(), 0); // offset = 0, stride = 0
	}

private:
	ModelData _model;
};
