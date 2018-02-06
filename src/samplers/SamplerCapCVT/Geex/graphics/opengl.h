/*
 *  _____ _____ ________  _
 * /  __//  __//  __/\  \//
 * | |  _|  \  |  \   \  / 
 * | |_//|  /_ |  /_  /  \ 
 * \____\\____\\____\/__/\\
 *
 * Graphics Environment for EXperimentations.
 *  Copyright (C) 2006 INRIA - Project ALICE
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  If you modify this software, you should include a notice giving the
 *  name of the person performing the modification, the date of modification,
 *  and the reason for such modification.
 *
 *  Contact: 
 *
 *     ALICE Project - INRIA
 *     INRIA Lorraine, 
 *     Campus Scientifique, BP 239
 *     54506 VANDOEUVRE LES NANCY CEDEX 
 *     FRANCE
 *
 *  Note that the GNU General Public License does not permit incorporating
 *  the Software into proprietary programs. 
 */
 
 
#ifndef __GEEX_GRAPHICS_OPENGL__
#define __GEEX_GRAPHICS_OPENGL__

#include <Geex/mathematics/glsl_linear.h>
#include <GLsdk/gl_stuff.h>

namespace Geex {

    // Note: we convert doubles into floats before passing
    // them to OpenGL since this accelerates rendering
    // (especially when display lists are used).

    inline void glVertex(const vec2& p) {
        ::glVertex2f( GLfloat(p.x), GLfloat(p.y) ) ;
    }

    inline void glVertex(const vec3& p) {
        ::glVertex3f( GLfloat(p.x), GLfloat(p.y), GLfloat(p.z) ) ;
    }

    inline void glVertex(const vec4& p) {
        ::glVertex4f( GLfloat(p.x), GLfloat(p.y), GLfloat(p.z), GLfloat(p.w) ) ;
    }

    inline void glTexCoord(real s) {
        ::glTexCoord1f( GLfloat(s) ) ;
    }

    inline void glTexCoord(const vec2& p) {
        ::glTexCoord2f( GLfloat(p.x), GLfloat(p.y) ) ;
    }

    inline void glTexCoord(const vec3& p) {
        ::glTexCoord3f( GLfloat(p.x), GLfloat(p.y), GLfloat(p.z) ) ;
    }

    inline void glTexCoord(const vec4& p) {
        ::glTexCoord4f( GLfloat(p.x), GLfloat(p.y), GLfloat(p.z), GLfloat(p.w) ) ;
    }

    inline void glNormal(const vec3& v) {
        ::glNormal3f( GLfloat(v.x), GLfloat(v.y), GLfloat(v.z) ) ;
    }

    inline void glColor(const vec3& c) {
        ::glColor3f( GLfloat(c.x), GLfloat(c.y), GLfloat(c.z) ) ;
    }

    inline void glColor(const vec4& c) {
        ::glColor4f( GLfloat(c.x), GLfloat(c.y), GLfloat(c.z), GLfloat(c.w) ) ;
    }

    inline void glMultiTexCoord(unsigned int unit, real s) {
        glMultiTexCoord1f(GL_TEXTURE0 + unit, (GLfloat)s) ;
    }

    inline void glMultiTexCoord(unsigned int unit, const vec2& v) {
        glMultiTexCoord2f(
            GL_TEXTURE0 + unit, 
            GLfloat(v.x), 
            GLfloat(v.y)
        ) ;
    }

    inline void glMultiTexCoord(unsigned int unit, const vec3& v) {
        glMultiTexCoord3f(
            GL_TEXTURE0 + unit, 
            GLfloat(v.x), 
            GLfloat(v.y),
            GLfloat(v.z)
        ) ;
    }

    inline void glMultiTexCoord(unsigned int unit, const vec4& v) {
        glMultiTexCoord4f(
            GL_TEXTURE0 + unit, 
            GLfloat(v.x), 
            GLfloat(v.y),
            GLfloat(v.z),
            GLfloat(v.w)
        ) ;
    }



    void GEEX_API begin_spheres() ;
    void GEEX_API end_spheres() ;
    void GEEX_API notify_screen_resize(int width, int height) ;

    GLuint GEEX_API create_texture_from_xpm_data(const char** xpm_data) ;

}

#endif
