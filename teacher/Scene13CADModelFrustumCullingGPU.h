#pragma once
#include <GL/glew.h>
#include <AABB.h>
#include <BoxMesh.h>
#include <FrustumCuller.h>
#include <ShaderData.h>
#include <ShaderLoader.h>
#include <Random.h>
#include <Timer.h>
#include <FrustumCuller.h>
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

	GLuint boundsSSBO;
	std::vector<BoundsData> drawableBounds;

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
		storeMesh(tess::tessellate_box(glm::make_vec3(b.lengths)), glm::make_mat4(b.transform));
	}

	virtual void validPrimitive(const rvm::Sphere& s)
	{
		storeMesh(tess::tessellate_sphere(s.radius), glm::make_mat4(s.transform));
	}

	virtual void validPrimitive(const rvm::Cylinder& c)
	{
		storeMesh(tess::tessellate_cylinder(c.radius, c.height), glm::make_mat4(c.transform));
	}

	virtual void validPrimitive(const rvm::Dish& d)
	{
		storeMesh(tess::tessellate_dish(d.radius, d.height), glm::make_mat4(d.transform));
	}

	virtual void validPrimitive(const rvm::Pyramid& p)
	{
		storeMesh(tess::tessellate_pyramid(glm::make_vec2(p.topLengths), glm::make_vec2(p.bottomLengths), p.height, glm::make_vec2(p.offset)),
		          glm::make_mat4(p.transform));
	}

	virtual void validPrimitive(const rvm::RectangularTorus& t)
	{
		storeMesh(tess::tessellate_rectangular_torus(t.internalRadius, t.externalRadius, t.height, t.sweepAngle), glm::make_mat4(t.transform));
	}

	virtual void validPrimitive(const rvm::CircularTorus& t)
	{
		storeMesh(tess::tessellate_circular_torus(t.internalRadius, t.externalRadius, t.sweepAngle), glm::make_mat4(t.transform));
	}

	virtual void validPrimitive(const rvm::Cone& c)
	{
		storeMesh(tess::tessellate_cone(c.radiusTop, c.radiusBottom, c.height),  glm::make_mat4(c.transform));
	}

	virtual void validPrimitive(const rvm::SlopedCone& c)
	{
		storeMesh(tess::tessellate_cone_slope_offset(c.radiusTop, c.radiusBottom, c.height, glm::make_vec2(c.topSlopeAngle),
		                                       glm::make_vec2(c.bottomSlopeAngle), glm::make_vec2(c.offset)),
		          glm::make_mat4(c.transform));
	}

	virtual void validPrimitive(const rvm::Mesh& mesh)
	{
		tess::tessellate_polygonal_begin();

		for(const auto& face : mesh.faces)
		{
			tess::polygon tpoly;
			tpoly.contours.reserve(face.polygons.size());

			for(const auto& poly : face.polygons)
			{
				tess::contour tcontour;
				tcontour.points.reserve(poly.points.size());

				for(const auto& point : poly.points)
				{
					tess::point tpoint;
					tpoint.vertex = make_vec3(point.vertex);
					tpoint.normal = make_vec3(point.normal);
					tcontour.points.push_back(tpoint);
				}

				tpoly.contours.push_back(tcontour);
			}

			tess::tessellate_polygonal_add(tpoly);
		}

		auto data = tess::tessellate_polygonal_end();

		if(!data.is_valid())
		{
			return;
		}

		tess::mesh_optimizer opt;
		opt.optimize(data, tess::mesh_optimizer::flag_all_optimizations);

		storeMesh(data, make_mat4(mesh.transform));
	}

	virtual void beginBlock(rvm::CntBegin& block)
	{
		rvm::Material m = _materials.getMaterial(block.colorCode);
		_currMaterial.diffuse = glm::make_vec4(m.diffuseColor);
		_currMaterial.specular = glm::make_vec4(m.specularColor);
		_currMaterial.specular.w = m.shininess;
	}

private:
	void storeMesh(const tess::triangle_mesh& mesh, const glm::mat4& m4)
	{
		_model->transforms.push_back(_toTransform(m4));

		DrawCommand drawCmd;
		drawCmd.elementCount = mesh.elements.size();
		drawCmd.instanceCount = 1;
		drawCmd.firstElement = _model->elements.size();
		drawCmd.baseVertex = _model->vertices.size();
		drawCmd.baseInstance = _model->drawCmds.size(); // automatically fetch the drawID instanced attribute
		_model->drawCmds.push_back(drawCmd);

		_model->vertices.insert(_model->vertices.end(), mesh.vertices.begin(), mesh.vertices.end());
		_model->elements.insert(_model->elements.end(), mesh.elements.begin(), mesh.elements.end());

		AABB bounds;

		for(auto v : mesh.vertices)
		{
			auto worldPos = glm::vec3(m4 * glm::vec4(v.position, 1.0f));
			_model->bounds.expand(worldPos);
			bounds.expand(worldPos);
		}

		_model->drawableBounds.push_back(_toBoundsData(bounds));

		_model->materials.push_back(_currMaterial);
	}

	TransformData _toTransform(const glm::mat4& m)
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

	BoundsData _toBoundsData(const AABB& b)
	{
		return {vec4(b.min, 0.0f), vec4(b.max, 0.0f)};
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

		GLuint vbo;
		glCreateBuffers(1, &vbo);
		glNamedBufferStorage(vbo, _model.vertices.size()*sizeof(tess::vertex), _model.vertices.data(), 0); // flags = 0

		GLuint ebo;
		glCreateBuffers(1, &ebo);
		glNamedBufferStorage(ebo, _model.elements.size()*sizeof(tess::element), _model.elements.data(), 0); // flags = 0

		// ------------------------------------------------------------------------
		// 3- Setup vertex array object
		// ------------------------------------------------------------------------

		GLuint bufferIndex = 0;

		// create vao
		glCreateVertexArrays(1, &_model.vao);

		// bind vbo to vao
		glVertexArrayVertexBuffer(_model.vao, bufferIndex, vbo, 0, sizeof(tess::vertex)); // offset = 0, stride = sizeof(tess::vertex)

		// setup position attrib
		glEnableVertexArrayAttrib(_model.vao, IN_POSITION);
		glVertexArrayAttribBinding(_model.vao, IN_POSITION, bufferIndex);
		glVertexArrayAttribFormat(_model.vao, IN_POSITION, 3, GL_FLOAT, GL_FALSE, 0); // size = 3, normalized = false, offset = 0

		// setup normal attrib
		glEnableVertexArrayAttrib(_model.vao, IN_NORMAL);
		glVertexArrayAttribBinding(_model.vao, IN_NORMAL, bufferIndex);
		glVertexArrayAttribFormat(_model.vao, IN_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(tess::vertex::position)); // size = 3, normalized = false, offset = sizeof(tess::vertex::position)

		// bind ebo
		glVertexArrayElementBuffer(_model.vao, ebo);

		// ------------------------------------------------------------------------
		// 4- Create shader program
		// ------------------------------------------------------------------------

		ShaderLoader loader;
		if(!loader.addFile(GL_VERTEX_SHADER, "../src/Scene13CADModelFrustumCullingGPU.vert", "../src/ShaderData.h"))
		{
			return false;
		}
		if(!loader.addFile(GL_FRAGMENT_SHADER, "../src/Scene13CADModelFrustumCullingGPU.frag", "../src/ShaderData.h"))
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

		glCreateBuffers(1, &_model.transformsSSBO);
		glNamedBufferStorage(_model.transformsSSBO, _model.transforms.size()*sizeof(TransformData), _model.transforms.data(), 0); // flags = 0

		glCreateBuffers(1, &_model.materialsSSBO);
		glNamedBufferStorage(_model.materialsSSBO, _model.materials.size()*sizeof(MaterialData), _model.materials.data(), 0); // flags = 0

		glCreateBuffers(1, &_model.boundsSSBO);
		glNamedBufferStorage(_model.boundsSSBO, _model.drawableBounds.size()*sizeof(BoundsData), _model.drawableBounds.data(), 0); // flags = 0

		// ------------------------------------------------------------------------
		// 6- Setup draw command buffer
		// ------------------------------------------------------------------------

		glCreateBuffers(1, &_model.drawCmdsBuffer);
		glNamedBufferStorage(_model.drawCmdsBuffer, _model.drawCmds.size()*sizeof(DrawCommand), _model.drawCmds.data(), 0); // flags = 0

		// ------------------------------------------------------------------------
		// 7- Setup custom draw ID
		// ------------------------------------------------------------------------

		std::vector<int> drawIDs(_model.drawCmds.size());
		for(unsigned int i = 0; i < drawIDs.size(); ++i)
		{
			drawIDs[i] = i;
		}

		GLuint drawIdBuffer = 0;

		glCreateBuffers(1, &drawIdBuffer);
		glNamedBufferStorage(drawIdBuffer, drawIDs.size()*sizeof(int), drawIDs.data(), 0); // flags = 0

		// setup drawID as an additional vertex attribute with instancing enabled

		// use another binding index inside the same vao used to store scene geometry
		++bufferIndex;

		// bind drawID buffer to vao
		glVertexArrayVertexBuffer(_model.vao, bufferIndex, drawIdBuffer, 0, sizeof(int)); // offset = 0, stride = sizeof(int)

		// enable instancing (this is for the entire vertex buffer and not just for the specific drawID attrib)
		glVertexArrayBindingDivisor(_model.vao, bufferIndex, 1);

		// setup drawID attrib
		glEnableVertexArrayAttrib(_model.vao, IN_DRAWID);
		glVertexArrayAttribBinding(_model.vao, IN_DRAWID, bufferIndex);
		glVertexArrayAttribIFormat(_model.vao, IN_DRAWID, 1, GL_INT, 0); // size = 1, offset = 0

		// ------------------------------------------------------------------------
		// 8- Setup compute shader to generate work inside GPU
		// ------------------------------------------------------------------------

		// load compute shader
		ShaderLoader computeLoader;
		if(!computeLoader.addFile(GL_COMPUTE_SHADER, "../src/Scene13CADModelFrustumCullingGPU.comp", "../src/ShaderData.h"))
		{
			return false;
		}
		if(!computeLoader.link(_computeProgram))
		{
			return false;
		}

		// compute the number of groups to be dispatched according to scene size and block size
		// each shader invocation will compute a single geometry
		_numGroupsX = (_model.drawCmds.size() + CS_BLOCK_SIZE_X - 1) / CS_BLOCK_SIZE_X;

		// tell compute shader how many geometries are in the scene, so any extra shader invocations can return immediatelly
		glProgramUniform1ui(_computeProgram, U_SCENE_SIZE, _model.drawCmds.size());

		// -------------------------------------------------------------------------------------------
		// 9- Setup atomic counter to keep track of how many draw calls were generated inside the GPU
		// -------------------------------------------------------------------------------------------

		glCreateBuffers(1, &_atomicCounterBuffer);
		glNamedBufferStorage(_atomicCounterBuffer, sizeof(GLuint), nullptr, GL_DYNAMIC_STORAGE_BIT); // data = nullptr

		// -------------------------------------------------------------------------------------------
		// 10- Setup draw commands for visible geometries inside GPU
		// -------------------------------------------------------------------------------------------

		glCreateBuffers(1, &_visibleDrawCmdsBuffer);
		glNamedBufferStorage(_visibleDrawCmdsBuffer, _model.drawCmds.size()*sizeof(DrawCommand), nullptr, 0);

		// -------------------------------------------------------------------------------------------
		// 11- Setup buffer to store frustum data
		// -------------------------------------------------------------------------------------------

		glCreateBuffers(1, &_frustumSSBO);
		glNamedBufferStorage(_frustumSSBO, sizeof(FrustumData), nullptr, GL_DYNAMIC_STORAGE_BIT); // data = nullptr

		return true;
	}

	const AABB& getBounds()
	{
		return _model.bounds;
	}

	void draw(const CameraData& cameraData)
	{
		// ----------------------------------------------------------------------------------------------------------------------
		// Generate work using the GPU
		// ----------------------------------------------------------------------------------------------------------------------

		// update frustum data and send to GPU
		_frustumCuller.beginFrame(cameraData.viewProjMatrix);
		glNamedBufferSubData(_frustumSSBO, 0, sizeof(FrustumData), &_frustumCuller.getData());

		// clear atomic counter (zero how many draw calls were generated in the previous frame)
		GLuint zero = 0;
		glNamedBufferSubData(_atomicCounterBuffer, 0, sizeof(GLuint), &zero); // offset = 0

		// clear draw command buffer (maybe it is more efficient to use a compute shader or to copy from another gpu buffer)
		// we take benefit of the fact that if data is null, the range is filled with zeroes
		glClearNamedBufferSubData(_visibleDrawCmdsBuffer, GL_R32UI, 0, _model.drawCmds.size()*sizeof(DrawCommand), GL_RED, GL_UNSIGNED_INT, nullptr);

		// bind stuff to compute
		glUseProgram(_computeProgram);
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, AC_DRAW_COUNT, _atomicCounterBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SB_IN_DRAW_CMD, _model.drawCmdsBuffer); // bind as SSBO to read!
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SB_OUT_DRAW_CMD, _visibleDrawCmdsBuffer); // bind as SSBO to write!
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SB_BOUNDS, _model.boundsSSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SB_FRUSTUM, _frustumSSBO);

		// dispatch compute
		glDispatchCompute(_numGroupsX, 1, 1); // num_groups_y = 1, num_groups_z = 1

		// insert memory barrier to guarantee data will be visible when drawing
		// the parameter indicates how the written memory will be used afterwards
		glMemoryBarrier(GL_COMMAND_BARRIER_BIT); // this corresponds to GL_DRAW_INDIRECT_BUFFER later

		// ----------------------------------------------------------------------------------------------------------------------
		// Draw scene
		// ----------------------------------------------------------------------------------------------------------------------

		// bind stuff to draw
		glUseProgram(_model.program);
		glBindVertexArray(_model.vao);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SB_TRANSFORM, _model.transformsSSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SB_MATERIAL, _model.materialsSSBO);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _visibleDrawCmdsBuffer); // use compute shader ouput as draw command buffer

		// this is the GL_ARB_indirect_parameters extension, which is actually slower than clearing the draw indirect buffer and invoking empty draw calls
		// if there are few visible geometries, it can improve performance by 50%. but if there are a lot of visible geometries, performance drops to 10%!
		// if you want to test this, you can comment the "clear draw command buffer" line above, just before compute dispatch, since it would no longer be needed
		// remember to comment the old draw call below
//		glBindBuffer(GL_PARAMETER_BUFFER_ARB, _atomicCounterBuffer); // bind atomic counter as the parameter buffer for the multidrawindirect call
//		glMultiDrawElementsIndirectCountARB(GL_TRIANGLES, GL_UNSIGNED_INT, 0, 0, _model.drawCmds.size(), 0); // drawOffset = 0, drawCountOffset = 0, stride = 0

		// draw indirect using commands generated by compute shader inside GPU
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, _model.drawCmds.size(), 0); // offset = 0, stride = 0
	}

private:
	ModelData _model;
	GLuint _computeProgram;
	GLuint _atomicCounterBuffer;
	GLuint _numGroupsX;
	GLuint _visibleDrawCmdsBuffer;
	FrustumCuller _frustumCuller;
	GLuint _frustumSSBO;
};
