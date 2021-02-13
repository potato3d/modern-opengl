#pragma once
#include <ShaderData.h>
#include <GL/glew.h>

class Light
{
public:
	bool initialize()
	{
		LightData light;
		light.ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
		light.diffuse = glm::vec4(1.0f);
		light.specular = glm::vec4(0.5f);

		// create UBO to store light data inside GPU
		GLuint lightUBO;
		glCreateBuffers(1, &lightUBO);
		glNamedBufferStorage(lightUBO, sizeof(LightData), &light, 0); // flags = 0

		// associate light UBO to shader binding point
		glBindBufferBase(GL_UNIFORM_BUFFER, UB_LIGHT, lightUBO);

		return true;
	}

private:
	LightData _data;
};
