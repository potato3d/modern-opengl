#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#else
#include <GL/glxew.h>
#endif
#include <GL/freeglut.h>

#include <cstdio>
#include <iostream>
#include <vector>

#include <Camera.h>
#include <FPS.h>
#include <Framebuffer.h>
#include <Light.h>

//#include <Scene0Framebuffer.h> //scene0
//#include <Scene1CubeNoLighting.h> //scene1
//#include <Scene2CubeVertexLighting.h> //scene2
//#include <Scene3CubeFragLighting.h> //scene3
//#include <Scene4CubeTexture.h> //scene4
//#include <Scene5ManyCubesIndividual.h> //scene5
//#include <Scene6ManyCubesInstancing.h> //scene6
//#include <Scene6bManyCubesInstancing.h> //scene6b
//#include <Scene7ManyCubesIndividualTexture.h> //scene7
//#include <Scene8ManyCubesInstancingTexture.h> //scene8
//#include <Scene9ManyGeometriesIndividual.h> //scene9
//#include <Scene10ManyGeometriesMultiDrawIndirect.h> //scene10
//#include <Scene11CADModel.h> //scene11
//#include <Scene12CADModelFrustumCullingCPU.h> //scene12
//#include <Scene13CADModelFrustumCullingGPU.h> //scene13

//-------------------------------------------------------------------------------------------------
// Global variables
//-------------------------------------------------------------------------------------------------

// FPS
FPS g_fps;

// Framebuffer
Framebuffer g_framebuffer(1024, 768);

// Camera
Camera g_camera(g_framebuffer.getWidth(), g_framebuffer.getHeight());

// Light
Light g_light;

// Scene
Scene g_scene;

//-------------------------------------------------------------------------------------------------
// Auxiliary functions
//-------------------------------------------------------------------------------------------------

void keyPress(unsigned char key, int x, int y)
{
	(void)x;
	(void)y;
	if(key == ' ') // space
	{
		g_camera.setFocus(g_scene.getBounds());
	}
	else if(key == '\x1B') // ESC
	{
		exit(0);
	}
	else if(key == 'z' || key == 'Z') // wireframe
	{
		static int wireframeEnabled = 0;
		wireframeEnabled ^= 1;
		glPolygonMode(GL_FRONT_AND_BACK, wireframeEnabled? GL_LINE : GL_FILL);
	}
	glutPostRedisplay();
}

Camera::MouseButton toCamera(int button)
{
	if(button == GLUT_LEFT_BUTTON) return Camera::Button_Left;
	else if(button == GLUT_RIGHT_BUTTON) return Camera::Button_Right;
	else if(button == GLUT_MIDDLE_BUTTON) return Camera::Button_Middle;
	else return Camera::Button_Left;
}

void mouseButton(int button, int state, int x, int y)
{
	if(button == 3)
	{
		g_camera.mouseWheel(Camera::Wheel_Up);
	}
	else if(button == 4)
	{
		g_camera.mouseWheel(Camera::Wheel_Down);
	}
	else if(state == GLUT_DOWN)
	{
		g_camera.mousePressed(toCamera(button), x, y);
	}
	else if(state == GLUT_UP)
	{
		g_camera.mouseReleased(toCamera(button), x, y);
	}
}

void mouseMove(int x, int y)
{
	g_camera.mouseMoved(x, y);
}

void updateFPS()
{
	float fps = 0.0f;
	if(g_fps.update(fps))
	{
		char title[512];
		snprintf(title, sizeof(title), "ModernOpenGL by Batata  %.2f fps | %.2f ms", fps, 1000.0f/fps);
		glutSetWindowTitle(title);
	}
}

//-------------------------------------------------------------------------------------------------
// OpenGL functions
//-------------------------------------------------------------------------------------------------

void disableVSync()
{
#ifdef _WIN32
	wglSwapIntervalEXT(0);
#else
	glXSwapIntervalSGI(0);
#endif
}

void initialize()
{
	// --------------------------------------------------------------------------------------------
	// initialize OpenGL
	// --------------------------------------------------------------------------------------------
	glewInit();

	// check support
	if(!GLEW_VERSION_4_5)
	{
		std::cout << "OpenGL 4.5 not supported" << std::endl;
		exit(1);
	}

	// enable depth test
	glEnable(GL_DEPTH_TEST);

	// discard backfaces
	glEnable(GL_CULL_FACE);

	// enable full-speed refresh rate
	disableVSync();

	// --------------------------------------------------------------------------------------------
	// initialize scene
	// --------------------------------------------------------------------------------------------
	if(!g_scene.initialize())
	{
		std::cout << "Error initializing scene" << std::endl;
		exit(1);
	}

	// --------------------------------------------------------------------------------------------
	// initialize framebuffer
	// --------------------------------------------------------------------------------------------
	if(!g_framebuffer.initialize())
	{
		std::cout << "Error initializing framebuffer" << std::endl;
		exit(1);
	}

	// --------------------------------------------------------------------------------------------
	// initialize camera
	// --------------------------------------------------------------------------------------------
	if(!g_camera.initialize(g_scene.getBounds()))
	{
		std::cout << "Error initializing camera" << std::endl;
		exit(1);
	}

	// --------------------------------------------------------------------------------------------
	// initialize light
	// --------------------------------------------------------------------------------------------
	if(!g_light.initialize())
	{
		std::cout << "Error initializing light" << std::endl;
		exit(1);
	}
}

void display()
{
	// send camera data to GPU
	g_camera.update();

	// prepare framebuffer
	g_framebuffer.clear();
	g_framebuffer.bindToDraw();

	// draw scene
	g_scene.draw(g_camera.getData());

	// copy to default framebuffer
	g_framebuffer.copyToDefault();

	// display image to screen
	glutSwapBuffers();

	// update performance measurement
	updateFPS();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	g_framebuffer.resize(w, h);
	g_camera.resize(w, h);
}

//-------------------------------------------------------------------------------------------------
// Main
//-------------------------------------------------------------------------------------------------

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	// setup and create window
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(g_framebuffer.getWidth(), g_framebuffer.getHeight());
	glutInitWindowPosition(450, 200);
	glutCreateWindow("ModernOpenGL by Batata");

	// display callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(display);

	// keyboard callbacks
	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(keyPress);

	// mouse callbacks
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);
	glutPassiveMotionFunc(mouseMove);

	// pre-loop
	initialize();

	// mainloop
	glutMainLoop();

	return 0;
}
