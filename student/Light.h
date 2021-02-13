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

		//scene2: create UBO to store light data inside GPU

		//scene2: associate light UBO to shader binding point

		return true;
	}

private:
	LightData _data;
};
