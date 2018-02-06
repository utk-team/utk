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
 
 
#ifndef __GEEX_GRAPHICS_GEEXOB__
#define __GEEX_GRAPHICS_GEEXOB__

#include "color.h"
#include "../basics/arrays.h"
#include "opengl.h"

namespace Geex {

    /**
     * Geexob encapsulates management of textures
     * and shaders.
     */
    class GEEX_API Geexob {
    public:
        Geexob() ;
        virtual ~Geexob() ;
        virtual void draw() ;

        void set_shader(const std::string& shader_base_name) {
            shader_base_name_ = shader_base_name ;
            shader_ = 0 ;
            for(unsigned int i=0; i<max_uniforms; i++) {
                uniform_id_[i] = 0 ;
            }
        }

        void set_texture(unsigned int index, GLuint tex_id, GLuint tex_target = GL_TEXTURE_2D) {
            if(textures_[index] != 0) {
                glDeleteTextures(1,&(textures_[index])) ;
            }
            textures_[index] = tex_id ;
            texture_targets_[index] = tex_target ;
        }

        void set_texture(unsigned int index, const std::string& filename) ;
        void set_bump_map(unsigned int index, const std::string& filename, float scaling = 0.15) ;
        void set_3D_texture(unsigned int index, const std::string& filename) ;
        void set_3D_bump_map(unsigned int index, const std::string& filename, float scaling = 0.15) ;
       
        void set_color(const vec4& x) { base_color_ = x ; }
        const vec4& color() const { return base_color_ ; }
        void set_culling(GLenum x) { culling_ = x ; }
        void set_xform(GLfloat* xform) ;

        GLfloat& new_uniform(const std::string& name, GLfloat value) ;
        unsigned int nb_uniforms() const { return nb_uniforms_ ; }

        int nb_frames() const { return nb_frames_ ; }
        virtual void set_frame(int x) ;
        int frame() const { return frame_ ; }

        virtual void get_bbox(
            real& x_min, real& y_min, real& z_min,
            real& x_max, real& y_max, real& z_max
        ) ;

        static bool program_objects_supported() ; 

    protected:
        virtual void pre_draw() ;
        virtual void do_draw() ;
        virtual void post_draw() ;
        void init_shaders() ;

        GLhandleARB load_shader(const std::string& shader_basename) ;
        GLuint load_3D_texture(const std::string& filename) ;
        GLuint load_3D_bump_map(const std::string& filename, float scaling) ;
        GLuint load_texture(const std::string& filename_in) ;
        GLuint load_bump_map(const std::string& filename_in, float zscale = 0.15) ;   
        

    protected:
        GLhandleARB shader_ ;

        GLuint shader_lighting_map_id_ ;
        GLuint shader_diff_lighting_map_id_ ;
        GLuint shader_lighting_mat_id_ ;
        GLuint shader_eye_pos_id_ ;

        GLuint shader_diffuse_map_id_ ;
        GLuint shader_specular_map_id_ ;
        GLuint shader_normal_map_id_ ;

        GLenum culling_ ;
        vec4 base_color_ ;

        GLfloat xform_[4][4] ;

        Array1d<GLuint> textures_ ;
        Array1d<GLuint> texture_targets_ ;

        std::string shader_base_name_ ;

        enum { max_uniforms = 32 } ;
        GLuint uniform_id_[max_uniforms] ;
        std::string uniform_name_[max_uniforms] ;
        GLfloat uniform_value_[max_uniforms] ;
        unsigned int nb_uniforms_ ;
        

    protected:
        int frame_ ;
        int nb_frames_ ;
    } ;

}

#endif

