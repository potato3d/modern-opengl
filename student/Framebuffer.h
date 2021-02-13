#pragma once
#include <GL/glew.h>
#include <ShaderData.h>

class Framebuffer
{
public:
	Framebuffer(int width, int height)
	{
		_width = width;
		_height = height;
		_fbo = 0;
		_colorTex = 0;
		_depthTex = 0;
	}

	int getWidth() const
	{
		return _width;
	}

	int getHeight() const
	{
		return _height;
	}

	bool initialize()
	{
		// ------------------------------------------------------------------------
		// 1- Create textures to hold pixel data
		// ------------------------------------------------------------------------

		//scene0: color texture

		//scene1: depth texture

		// ------------------------------------------------------------------------
		// 2- Setup framebuffer object
		// ------------------------------------------------------------------------

		//scene0: create fbo

		//scene0: setup color buffer

		//scene1: setup depth buffer

		//scene0: check fbo status

		return true;
	}

	void resize(int width, int height)
	{
		_width = width;
		_height = height;

		// resize framebuffer textures

		//scene0: color texture

		//scene1: depth texture
	}

	void clear()
	{
		// clear framebuffer

		//scene0: clear color

		//scene1: clear depth
	}

	void bindToDraw()
	{
		//scene0: set framebuffer output to correct color attachment

		//scene0: enable drawing to the framebuffer
	}

	void copyToDefault()
	{
		// to display the rendered image on screen, we will read from the framebuffer's output to the default framebuffer

		//scene0: set read buffer

		//scene0: set draw buffer

		//scene0: copy framebuffer contents to default framebuffer
	}

private:
	GLsizei _width;
	GLsizei _height;
	GLuint _fbo;
	GLuint _colorTex;
	GLuint _depthTex;
};
