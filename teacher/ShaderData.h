#ifdef __cplusplus
#pragma once
#include <glm/glm.hpp>
using namespace glm;
#ifdef near
#undef near
#endif
#ifdef far
#undef far
#endif
#endif

struct BoundsData
{
	vec4 minmax[2];
};

struct Plane
{
	float nx;
	float ny;
	float nz;
	float offset;
	int px;
	int py;
	int pz;
};

struct FrustumData
{
	Plane near;
	Plane left;
	Plane right;
	Plane bottom;
	Plane top;
	Plane far;
};

struct CameraData
{
	mat4 viewMatrix;
	mat4 viewProjMatrix;
};

struct MaterialData
{
	vec4 diffuse;
	vec4 specular; // w = shininess
};

struct TransformData
{
	vec4 row0;
	vec4 row1;
	vec4 row2;
};

struct LightData
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

struct DrawCommand
{
	uint  elementCount; // count
	uint  instanceCount; // instanceCount
	uint  firstElement; // firstIndex
	uint  baseVertex; // baseVertex
	uint  baseInstance; // baseInstance
};

// Uniform Buffers
#define UB_CAMERA		0
#define UB_LIGHT		1
#define UB_TRANSFORM	2
#define UB_MATERIAL		3

// Storage Buffers
#define SB_TRANSFORM	0
#define SB_MATERIAL		1
#define SB_TEXTURE		2
#define SB_IN_DRAW_CMD	3
#define SB_OUT_DRAW_CMD	4
#define SB_BOUNDS		5
#define SB_FRUSTUM      6

// Vertex Attributes
#define IN_POSITION		0
#define IN_NORMAL		1
#define IN_COLOR		2
#define IN_TEXCOORD		3
#define IN_DRAWID		4
#define IN_TRANSFORM	5

// Fragment Outputs
#define OUT_COLOR		0
#define NUM_FRAG_OUT	1

// Texture Units
#define TEX_TEXTURE		0

// Compute Shader
#define CS_BLOCK_SIZE_X	256

// Uniform Variables
#define U_SCENE_SIZE	0
#define U_RAND_SEED		1

// Atomic Counters
#define AC_DRAW_COUNT	0
