#ifndef __glutess_facade_h__
#ifndef __GLUTESS_FACADE_H__

#define __glutess_facade_h__
#define __GLUTESS_FACADE_H__

/*
************************************************************************************************************************
	This is just a simple copy and rename of the GLU tesselation routines found in OpenGL Sample Implementation.
	ref: http://oss.sgi.com/projects/ogl-sample/
************************************************************************************************************************
	The following help has been copied from: http://www.opengl.org/resources/faq/technical/glu.htm
************************************************************************************************************************

4.040 How do I use GLU tessellation routines?

GLU provides tessellation routines to let you render concave polygons, self-intersecting polygons, and polygons with
holes. The tessellation routines break these complex primitives up into (possibly groups of) simpler, convex primitives
that can be rendered by the OpenGL API. Do this by providing the data of the simpler primitives to your application from
callback routines that your application must provide. Your app can then send the data to OpenGL using normal API calls.

An example program is available in the GLUT distribution under progs/redbook/tess.c.

The usual steps for using tessellation routines are:

1. Allocate a new GLU tessellation object:

GLUtesselator *tess = gluNewTess();

2. Assign callbacks for use with this tessellation object:

gluTessCallback (tess, GLU_TESS_BEGIN, tcbBegin);
gluTessCallback (tess, GLU_TESS_VERTEX, tcbVertex);
gluTessCallback (tess, GLU_TESS_END, tcbEnd);

2a. If your primitive is self-intersecting, you must also specify a callback to create new vertices:

gluTessCallback (tess, GLU_TESS_COMBINE, tcbCombine);

3. Send the complex primitive data to GLU:

// Assumes:
//    GLdouble data[numVerts][3];
// ...and assumes the array has been filled with 3D vertex data.

gluTessBeginPolygon (tess, NULL);
gluTessBeginContour (tess);
for (i=0; i<sizeof(data)/(sizeof(GLdouble)*3);i++)
   gluGLUTESS_vertex (tess, data[i], data[i]);
gluTessEndContour (tess);
gluEndPolygon (tess);

4. In your callback routines, make the appropriate OpenGL calls:

void tcbBegin (GLenum prim);
{
   glBegin (prim);
}

void tcbVertex (void *data)
{
   glVertex3dv ((GLdouble *)data);
}

void tcbEnd ();
{
   glEnd ();
}

void tcbCombine (GLdouble c[3], void *d[4], GLfloat w[4], void **out)
{
   GLdouble *nv = (GLdouble *) malloc(sizeof(GLdouble)*3);

   nv[0] = c[0];
   nv[1] = c[1];
   nv[2] = c[2];
   *out = nv;
}

The above list of steps and code segments is a bare-bones example and is not intended to demonstrate the full
capabilities of the tessellation routines. By providing application-specific data as parameters to gluTessBeginPolygon()
and tessVertex() and handling the data in the appropriate callback routines, your application can color and texture
map these primitives as it would a normal OpenGL primitive.
*/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
	struct GLUTESS_tesselator;
	typedef struct GLUTESS_tesselator GLUTESS_tessellator_obj;
	typedef struct GLUTESS_tesselator GLUTESS_triangulator_obj;
#else
	typedef struct GLUTESS_tesselator GLUTESS_tesselator;
	typedef struct GLUTESS_tesselator GLUTESS_tessellator_obj;
	typedef struct GLUTESS_tesselator GLUTESS_triangulator_obj;
#endif

typedef unsigned int     GLUTESS_enum;
typedef unsigned char    GLUTESS_boolean;
typedef float            GLUTESS_float;
typedef double           GLUTESS_double;

#define GL_POINTS                         0x0000
#define GL_LINES                          0x0001
#define GL_LINE_LOOP                      0x0002
#define GL_LINE_STRIP                     0x0003
#define GL_TRIANGLES                      0x0004
#define GL_TRIANGLE_STRIP                 0x0005
#define GL_TRIANGLE_FAN                   0x0006

#ifdef WIN32
#define GLUTESS_CALLBACK void (__cdecl*)()
#else
#define GLUTESS_CALLBACK void (*)()
#endif

GLUTESS_tesselator * tessNew(void);
void  tessDelete(GLUTESS_tesselator *tess);
void  tessBeginPolygon(GLUTESS_tesselator *tess,void *polygon_data);
void  tessBeginContour(GLUTESS_tesselator *tess);
void  tessVertex(GLUTESS_tesselator *tess,double coords[3],void *data);
void  tessEndContour(GLUTESS_tesselator *tess);
void  tessEndPolygon(GLUTESS_tesselator *tess);
void  tessProperty(GLUTESS_tesselator *tess,GLUTESS_enum which,GLUTESS_double value);
void  tessNormal(GLUTESS_tesselator *tess,GLUTESS_double x,GLUTESS_double y,GLUTESS_double z);
void  tessCallback(GLUTESS_tesselator *tess,GLUTESS_enum which, GLUTESS_CALLBACK);
void  tessGetProperty(GLUTESS_tesselator *tess,GLUTESS_enum which,GLUTESS_double *value);

#define GLUTESS_INVALID_ENUM 100900
#define GLUTESS_INVALID_VALUE 100901
#define GLUTESS_OUT_OF_MEMORY 100902

#define GLUTESS_MAX_COORD 1.0e150

#define GLUTESS_WINDING_RULE 100140
#define GLUTESS_BOUNDARY_ONLY 100141
#define GLUTESS_TOLERANCE 100142

#define GLUTESS_WINDING_ODD 100130
#define GLUTESS_WINDING_NONZERO 100131
#define GLUTESS_WINDING_POSITIVE 100132
#define GLUTESS_WINDING_NEGATIVE 100133
#define GLUTESS_WINDING_ABS_GEQ_TWO 100134

#define GLUTESS_BEGIN 100100
#define GLUTESS_VERTEX 100101
#define GLUTESS_END 100102
#define GLUTESS_ERROR 100103
#define GLUTESS_EDGE_FLAG 100104
#define GLUTESS_COMBINE 100105
#define GLUTESS_BEGIN_DATA 100106
#define GLUTESS_VERTEX_DATA 100107
#define GLUTESS_END_DATA 100108
#define GLUTESS_ERROR_DATA 100109
#define GLUTESS_EDGE_FLAG_DATA 100110
#define GLUTESS_COMBINE_DATA 100111

#define GLUTESS_ERROR1 100151
#define GLUTESS_ERROR2 100152
#define GLUTESS_ERROR3 100153
#define GLUTESS_ERROR4 100154
#define GLUTESS_ERROR5 100155
#define GLUTESS_ERROR6 100156
#define GLUTESS_ERROR7 100157
#define GLUTESS_ERROR8 100158

#define GLUTESS_MISSING_BEGIN_POLYGON GLUTESS_ERROR1
#define GLUTESS_MISSING_BEGIN_CONTOUR GLUTESS_ERROR2
#define GLUTESS_MISSING_END_POLYGON GLUTESS_ERROR3
#define GLUTESS_MISSING_END_CONTOUR GLUTESS_ERROR4
#define GLUTESS_COORD_TOO_LARGE GLUTESS_ERROR5
#define GLUTESS_NEED_COMBINE_CALLBACK GLUTESS_ERROR6

#ifdef __cplusplus
}
#endif
#endif /* __glutess_facade_h__ */
#endif /* __GLUTESS_FACADE_H__ */
