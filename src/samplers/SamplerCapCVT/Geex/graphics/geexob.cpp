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

#include <Geex/graphics/geexob.h>
#include <Geex/graphics/geexapp.h>
#include <Geex/basics/file_system.h>
#include <glut_viewer/glut_viewer.h>
#include <misc/normalMapUtils.h>
#include <map>
#include <stdio.h>

namespace Geex {
    
    Geexob::Geexob() {
        shader_ = 0 ;
        culling_ = GL_FRONT_AND_BACK ;
        base_color_ = vec4(1.0f,1.0f,1.0f, 1.0f) ;
        shader_base_name_ = "hdr" ;

        textures_.allocate(8) ;
        textures_.set_all(0) ;
        texture_targets_.allocate(8) ;
        texture_targets_.set_all(0) ;

        for(unsigned int i=0; i<4; i++) {
            for(unsigned int j=0; j<4; j++) {
                xform_[i][j] = (i == j) ? 1.0f : 0.0f ;
            }
        }
        
        if(GeexApp::instance() == nil || GeexApp::instance()->in_main_loop()) {
            set_texture(2, "white.png") ;
        }

        nb_uniforms_ = 0 ;
        
        nb_frames_ = 1 ;
        frame_ = 0 ;


    }



    Geexob::~Geexob() {
    }


    bool Geexob::program_objects_supported() {
        static bool checked = false ;
        static bool result = false ;
        if(!checked) {
            checked = true ;
#ifdef WIN32
            result = 
                wglGetProcAddress("glCreateProgramObjectARB") != 0 &&
                wglGetProcAddress("glUseProgramObjectARB") != 0 ;
#else
            // TODO: glXGetProcAddress ...
            result = true ;
#endif            
        }
        return result ;
    }

    void Geexob::draw() {
        if(GeexApp::instance() == nil || GeexApp::instance()->in_main_loop()) {
            pre_draw() ;
            do_draw() ;
            post_draw() ;
        }
    }



    void Geexob::pre_draw() {

        if(shader_ == 0 && program_objects_supported()) { init_shaders() ; }

        glMatrixMode(GL_MODELVIEW) ;
        glPushMatrix() ;
        glMultMatrixf(&(xform_[0][0])) ;
        
        if (program_objects_supported()) {
            for(unsigned int i=0; i<textures_.size(); i++) {
                if(textures_[i] != 0) {
                    glActiveTextureARB(GL_TEXTURE0 + i) ;
                    glBindTexture(texture_targets_[i], textures_[i]) ;
                }
                glActiveTextureARB(GL_TEXTURE0) ;
            }
            
            glUseProgramObjectARB(shader_) ;    
            
            glUniform1iARB(shader_lighting_map_id_, 0);    
            glUniform1iARB(shader_diff_lighting_map_id_, 1);    
            glUniform1iARB(shader_diffuse_map_id_, 2);    
            glUniform1iARB(shader_specular_map_id_, 3);    
            glUniform1iARB(shader_normal_map_id_, 4);    
            
            glUniform3fARB(shader_eye_pos_id_, 0, 0, 5) ; // From glut_viewer.c gluLookAt call
            glUniformMatrix4fvARB(shader_lighting_mat_id_, 1, GL_TRUE, glut_viewer_get_light_matrix()) ;

            for(unsigned int i=0; i<nb_uniforms_; i++) {
                if(uniform_id_[i] == 0) {
                    uniform_id_[i] = glGetUniformLocationARB(shader_, uniform_name_[i].c_str()) ;
                }
                glUniform1fARB(uniform_id_[i], uniform_value_[i]) ;
            }
        }
        

       
        if(culling_ != GL_FRONT_AND_BACK) {
            glEnable(GL_CULL_FACE) ;
            glCullFace(culling_) ;
        }

        glColor(base_color_) ;
    }

    void Geexob::do_draw() {
    }
    
    void Geexob::post_draw() {
        if (program_objects_supported()) {
            glUseProgramObjectARB(0);
        }
        glPopMatrix() ;
        glDisable(GL_CULL_FACE) ;
        glColor4f(1.0, 1.0, 1.0, 1.0) ;
    }

    void Geexob::set_xform(GLfloat* xform) {
        memcpy(xform_, xform, sizeof(GLfloat) * 16) ;
    }

    GLfloat& Geexob::new_uniform(const std::string& name, GLfloat value) {
        for(unsigned int i=0; i<nb_uniforms_; i++) {
            if(name == uniform_name_[i]) {
                return uniform_value_[i] ;
            }
        }
        gx_assert(nb_uniforms_ < max_uniforms - 1) ;
        uniform_value_[nb_uniforms_] = value ;
        uniform_name_[nb_uniforms_] = name ;
        uniform_id_[nb_uniforms_] = 0 ;
        nb_uniforms_++ ;
        return uniform_value_[nb_uniforms_ - 1] ;
    }

    void Geexob::init_shaders() {

        if(shader_base_name_.length() == 0) { 
            glUseProgramObjectARB(0);    
            return ; 
        }

        typedef std::map<std::string, GLhandleARB> ShaderCache ;
        static  ShaderCache shader_cache ;

        ShaderCache::iterator it = shader_cache.find(shader_base_name_) ;
        if(it != shader_cache.end()) {
            shader_ = (*it).second ;
        } else {
            std::cerr << "Loading shader: " << shader_base_name_ << std::endl ;
            std::string vp_filename = 
                Geex::FileSystem::get_project_root() + "/data/shaders/" + shader_base_name_ + "_vp.glsl" ;
            std::string fp_filename =
                Geex::FileSystem::get_project_root() + "/data/shaders/" + shader_base_name_ + "_fp.glsl" ;
            shader_ = glut_viewer_load_program((char*)vp_filename.c_str(), (char*)fp_filename.c_str()) ;
            shader_cache[shader_base_name_] = shader_ ;
        }

        glUseProgramObjectARB(shader_);    
        shader_lighting_map_id_ = glGetUniformLocationARB(shader_, "lighting_map");
        shader_diff_lighting_map_id_ = glGetUniformLocationARB(shader_, "lighting_map_diff");
        shader_lighting_mat_id_ = glGetUniformLocationARB(shader_, "lighting_mat");
        shader_eye_pos_id_ = glGetUniformLocationARB(shader_, "eye_pos");
        shader_diffuse_map_id_ = glGetUniformLocationARB(shader_, "diffuse_map");
        shader_specular_map_id_ = glGetUniformLocationARB(shader_, "specular_map");
        shader_normal_map_id_ = glGetUniformLocationARB(shader_, "normal_map");

        glUseProgramObjectARB(0);    
    }

//---------------------------------------- Textures --------------------------------------------------------------------

    GLhandleARB Geexob::load_shader(const std::string& shader_basename) {
        std::cerr << "Loading shader: " << shader_basename << std::endl ;
        std::string vp_filename = 
            Geex::FileSystem::get_project_root() + "/data/shaders/" + shader_basename + "_vp.glsl" ;
        std::string fp_filename =
            Geex::FileSystem::get_project_root() + "/data/shaders/" + shader_basename + "_fp.glsl" ;
        return glut_viewer_load_program((char*)vp_filename.c_str(), (char*)fp_filename.c_str()) ;
    }


    void Geexob::set_texture(unsigned int index, const std::string& filename) {
        set_texture(index, load_texture(filename)) ;
    }
    
    void Geexob::set_bump_map(unsigned int index, const std::string& filename, float scaling) {
        set_texture(index, load_bump_map(filename, scaling)) ;
    }

    void Geexob::set_3D_texture(unsigned int index, const std::string& filename) {
        set_texture(index, load_3D_texture(filename), GL_TEXTURE_3D) ;
    }

    void Geexob::set_3D_bump_map(unsigned int index, const std::string& filename, float scaling) {
        set_texture(index, load_3D_bump_map(filename, scaling), GL_TEXTURE_3D) ;       
    }


    static std::string find_file(const std::string& filename_in, const std::string& subdir) {
#ifdef WIN32
        if(filename_in.length() >= 2 && filename_in[1] == ':') { return filename_in ; }
#else
        if(filename_in[0] == '/') { return filename_in ; }
#endif
        return Geex::FileSystem::get_project_root() + "/data/" + subdir + "/" + filename_in ;
    }


    static Memory::pointer load_3D_image(
        const std::string& filename_in, GLuint& width, GLuint& height, GLuint& depth, GLuint& bpp
    ) {
        std::string basedir = find_file(filename_in, "textures3D") ;
        char buff[1024] ;
        
        std::vector<std::string> filenames ;

        std::string extension = "png" ;
        if(!FileSystem::is_file(basedir + "/result00.png")) {
            extension = "bmp" ;
        }

        int cur = 0 ;
        bool more = true ;
        do {
            sprintf(buff, "%s/result%02d.%s", basedir.c_str(), cur, extension.c_str()) ;
            more = FileSystem::is_file(std::string(buff)) ;
            if(more) {
                filenames.push_back(buff) ;
                cur++ ;
            }
        } while(more);

        depth = (GLuint)filenames.size() ;
        Memory::byte* data = nil ;
	  
        if(depth == 0) {
            return nil ;
        }
       
        GLvoid* slice_data = nil ;

        glut_viewer_load_image((char*)filenames[0].c_str(), &width, &height, &bpp, &slice_data) ;
        data = new Memory::byte[width*height*depth*bpp] ;
       
        Memory::pointer slice_pointer = data ;
        Memory::copy(slice_pointer, slice_data, width*height*bpp) ;
        free(slice_data) ;
	  
        for(unsigned int i=1; i<depth; i++) {
            slice_pointer += width*height*bpp ;
            GLuint slice_width, slice_height, slice_bpp ;
            glut_viewer_load_image((char*)filenames[i].c_str(), &slice_width, &slice_height, &slice_bpp, &slice_data) ;
            gx_assert(slice_width == width) ;
            gx_assert(slice_height == height) ;
            gx_assert(slice_bpp == bpp) ;
            Memory::copy(slice_pointer, slice_data, width*height*bpp) ;
            free(slice_data) ;
        }
        return data ;
    }
   
    static void rgb_to_gray(int width, int height, int bpp, void* pixels_in, void* pixels_out) {
        int i ;
        unsigned char* pin  = (unsigned char*)pixels_in ;
        unsigned char* pout = (unsigned char*)pixels_out ;
        float g ;
        gx_assert(bpp >= 3) ;
        for(i=0; i<width*height; i++) {
            g = 0.3f * (float)(pin[0]) + 0.59f * (float)(pin[1]) + 0.11f * (float)(pin[2]) ;
            *pout = (unsigned char)g ;
            pin += bpp ;
            pout++ ;
        }
    }
   
    static void pack_normal_map(float* pixels_bump, GLuint width, GLuint height) {
        unsigned int nb = width * height * 3 ;
        for(unsigned int i=0; i<nb; i++) {
            pixels_bump[i] = float(0.5 * (pixels_bump[i] + 1.0)) ;
        }
    }

    static GLfloat* texture_3D_to_bumpmap(
        Memory::pointer data, GLuint width, GLuint height, GLuint depth, GLuint bpp, GLfloat zscale
    ) {
        float scale[3] ;
        scale[0] = 1 ; 
        scale[1] = 1 ;
        scale[2] = zscale ;

        unsigned char* gray_slice = nil ;
        if(bpp > 1) { 
            gray_slice = new unsigned char[width*height] ; 
            std::cerr << " ... converting to graylevel" << std::endl ;
        }
       
       
        GLfloat* result = new GLfloat[width * height * depth * 3] ;
        Memory::pointer from = data ;
        GLfloat* to = result ;
        for(unsigned int k=0; k<depth; k++) {
            if(bpp == 1) {
                BumpmapToNormalmap(
                    from, to, scale, width, height
                ) ;
            } else {
                rgb_to_gray(width, height, bpp, from, gray_slice) ;
                BumpmapToNormalmap(
                    gray_slice, to, scale, width, height
                ) ;
            }
            pack_normal_map(to, width, height) ;	  
            from += width * height * bpp ;
            to += width * height * 3 ;
        }
        if(gray_slice != nil) {
            delete[] gray_slice ;
        }
        return result ;
    }
   
    GLuint Geexob::load_3D_texture(const std::string& filename) {
        GLuint width, height, depth, bpp ;
        Memory::pointer data = load_3D_image(filename, width, height, depth, bpp) ;
        GLuint texture ;
        glGenTextures(1, &texture) ;
        glBindTexture(GL_TEXTURE_3D, texture) ;

        std::cerr << "3D texture: " 
                  << width << "x" << height << "x" << depth << " - " << bpp << "bpp" << std::endl ;
       
        switch(bpp) {
	case 1:
            gluBuild3DMipmaps(
                GL_TEXTURE_3D, GL_LUMINANCE, width, height, depth, GL_LUMINANCE, GL_UNSIGNED_BYTE, data
            ) ;
            break ;
	case 3:
            gluBuild3DMipmaps(
                GL_TEXTURE_3D, GL_RGBA, width, height, depth, GL_RGB, GL_UNSIGNED_BYTE, data
            ) ;
            break ;
	case 4:
            gluBuild3DMipmaps(
                GL_TEXTURE_3D, GL_RGBA, width, height, depth, GL_RGBA, GL_UNSIGNED_BYTE, data
            ) ;
            break ;
        }
       
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_3D, 0) ;

        delete[] data ;
       
        return texture ;
    }

    GLuint Geexob::load_3D_bump_map(const std::string& filename, float scaling) {
        GLuint width, height, depth, bpp ;
        Memory::pointer data_height = load_3D_image(filename, width, height, depth, bpp) ;
        GLfloat* data_bump = texture_3D_to_bumpmap(data_height, width, height, depth, bpp, scaling) ;

        std::cerr << "3D texture: " 
                  << width << "x" << height << "x" << depth << " - " << bpp << "bpp" << std::endl ;
       
        GLuint texture ;
        glGenTextures(1, &texture) ;
        glBindTexture(GL_TEXTURE_3D, texture) ;

        gluBuild3DMipmaps(
            GL_TEXTURE_3D, GL_RGBA, width, height, depth, GL_RGB, GL_FLOAT, data_bump
        ) ;

        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_3D, 0) ;

        delete[] data_bump ;
        delete[] data_height ;
       
        return texture ;
    }

    GLuint Geexob::load_texture(const std::string& filename_in) {
        GLuint texture ;
        std::string filename = find_file(filename_in, "textures") ;
        glGenTextures(1, &texture) ;
        glBindTexture(GL_TEXTURE_2D, texture) ;
        glTexImage2Dfile((char*)(filename.c_str())) ;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0) ;
        return texture ;
    }

    GLuint Geexob::load_bump_map(const std::string& filename_in, float zscale) {
        GLuint texture ;
        std::string filename = find_file(filename_in, "textures") ;
        GLuint width, height, bpp ;
        GLvoid* pixels ; GLfloat* pixels_bump ;
        float scale[3] ;
        scale[0] = 1 ; 
        scale[1] = 1 ;
        scale[2] = zscale ;
        
        glut_viewer_load_image((char*)filename.c_str(), &width, &height, &bpp, &pixels) ;
        pixels_bump = new GLfloat[width * height * 3] ;

        if(bpp == 1) {
            BumpmapToNormalmap(
                (unsigned char*)pixels, pixels_bump, scale, width, height
            ) ;
        } else {
            for(unsigned int i=0; i<width*height; i++) {
                pixels_bump[i] = ((unsigned char*)pixels)[i] ;
            }
        }

        pack_normal_map(pixels_bump, width, height) ;
        
        glGenTextures(1, &texture) ;
        glBindTexture(GL_TEXTURE_2D, texture) ;
        
        gluBuild2DMipmaps(
            GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_FLOAT, pixels_bump
        ) ;
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        free(pixels) ;
        delete[] pixels_bump ;
        glBindTexture(GL_TEXTURE_2D, 0) ;
        
        return texture ;
    }

    void Geexob::set_frame(int x) {
        if(x < 0) { x = 0 ; }
        if(x >= ::fabs((float)nb_frames_)) { x = (int)::fabs((float)nb_frames_) - 1 ; }
        frame_ = x ;
    }

    void Geexob::get_bbox(
        real& x_min, real& y_min, real& z_min,
        real& x_max, real& y_max, real& z_max
    ) {
        x_min = 0.0 ; y_min = 0.0 ; z_min = 0.0 ;
        x_max = 1.0 ; y_max = 1.0 ; z_max = 1.0 ;
    }
    
}
