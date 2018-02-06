
/* 
 * gluBuild3DMipmaps is missing in
 * the GLU lib shipped with Windows.
 */

#ifndef __GLU_MIPMAP3D_PROTOTYPES__
#define __GLU_MIPMAP3D_PROTOTYPES__

#include <misc/linkage.h>

#ifdef __cplusplus
extern "C" {
#endif

GLint GEEX_API
gluBuild3DMipmapLevels(GLenum target, GLint internalFormat, 
			     GLsizei width, GLsizei height, GLsizei depth,
			     GLenum format, GLenum type, 
			     GLint userLevel, GLint baseLevel, GLint maxLevel,
			     const void *data) ;

GLint GEEX_API
gluBuild3DMipmaps(GLenum target, GLint internalFormat, 
			GLsizei width, GLsizei height, GLsizei depth,
			GLenum format, GLenum type, const void *data) ;

#ifdef __cplusplus
}
#endif


#endif
