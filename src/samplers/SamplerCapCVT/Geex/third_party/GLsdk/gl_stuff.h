
#ifndef GL_STUFF
#define GL_STUFF

#include <GLsdk/gl.h>
#include <GLsdk/glprocs.h>
#include <GL/glu.h>

/* 
 * gluBuild3DMipmaps is missing in
 * the GLU lib shipped with Windows.
 */
#ifdef WIN32
#include <glu/mipmap3D.h>
#endif

#endif
