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

#include <Geex/graphics/geexapp.h>
#include <Geex/basics/file_system.h>
#include <Geex/basics/stopwatch.h>
#include <misc/hdrtexture.h>
#include <malloc.h>
#include <stdlib.h>

namespace Geex {

    GeexApp* GeexApp::instance_ = nil ;

    GeexApp::GeexApp(int argc, char** argv) {
        gx_assert(instance_ == nil) ; // Only one instance at a time.

        instance_ = this ;
        argc_ = argc ;
        argv_ = argv ;
        width_ = 800 ;
        height_ = 800 ;
        get_arg("w", width_) ;
        get_arg("h", height_) ;
        scene_ = nil ;

        in_main_loop_ = GL_FALSE ;

        // GUI
        viewer_properties_ = nil ;

        // Rendering
        hdr_ = GL_TRUE ;
        get_arg("hdr", hdr_) ;
        diffuse_cubemap_ = 0 ;
        cubemap_ = 0 ;
        fp_filtering_ = GL_TRUE ;

        // Animation
        frame_ = 0.0f ;
        delay_ = 0.025f ;
        frame_delta_ = 1 ;
        frame_wrap_ = GL_FALSE ;
        animate_ = GL_FALSE ;
    }

    GeexApp::~GeexApp() {
        instance_ = nil ;
        delete scene_ ;
    }

    void GeexApp::get_arg(std::string arg_name, GLint& arg_val) {
        arg_name = std::string("-") + arg_name ;
        for(int i=1; i<argc_-1; i++) {
            if(argv_[i] == arg_name) {
                arg_val = atoi(argv_[i+1]) ;
            }
        }
    }

    void GeexApp::get_arg(std::string arg_name, std::string& arg_val) {
        arg_name = std::string("-") + arg_name ;
        for(int i=1; i<argc_-1; i++) {
            if(argv_[i] == arg_name) {
                arg_val = argv_[i+1] ;
            }
        }
    }

    void GeexApp::get_arg(std::string arg_name, GLfloat& arg_val) {
        arg_name = std::string("-") + arg_name ;
        for(int i=1; i<argc_-1; i++) {
            if(argv_[i] == arg_name) {
                arg_val = (float)atof(argv_[i+1]) ;
            }
        }
    }

    void GeexApp::get_arg(std::string arg_name, GLboolean& arg_val) {
        std::string on_arg_name = std::string("+") + arg_name ;
        std::string off_arg_name = std::string("-") + arg_name ;
        for(int i=1; i<argc_; i++) {
            if(argv_[i] == on_arg_name) {
                arg_val = GL_TRUE ;
            } else if(argv_[i] == off_arg_name) {
                arg_val = GL_FALSE ;
            }
        }
    }

    std::string GeexApp::get_file_arg(std::string extension, int start_index) {
        for(int i=start_index; i<argc_; i++) {
            if(FileSystem::extension(argv_[i]) == extension) {
                return argv_[i] ;
            }
        }        
        return "" ;
    }

    GLuint GeexApp::create_hdr_cubemap(const std::string& file_name_in) {
        GLuint result ;
        // Note: There is a mem leak when allocating hdr
        std::cerr << "Creating HDR tex: " << file_name_in << std::endl ;
        std::string file_name = ::Geex::FileSystem::get_project_root() + "/data/hdr/" + file_name_in ;
        HDRtexture hdr((char*)(file_name.c_str())) ;
        if(!hdr.IsValid()) {
            std::cerr << "Invalid HDR" << std::endl ;
            exit(-1) ;
        }
        hdr.Analyze() ;
        // Note: we should use HILO instead (and have
        //   a Program Object that does translation).
        if(!fp_filtering_) {
            result = hdr.CreateCubemap(GL_RGB) ;
        } else {
            result = hdr.CreateCubemap(GL_RGB_FLOAT16_ATI) ;
        }
        return result ;
    }

    void GeexApp::set_cubemap(const std::string& filename) {
        if(diffuse_cubemap_ != 0) {
            glDeleteTextures(1, &diffuse_cubemap_) ;
        }
        if(cubemap_ != 0) {
            glDeleteTextures(1, &cubemap_) ;
        }
        diffuse_cubemap_ = create_hdr_cubemap(filename + "_cross_diffuse.hdr")  ;
        cubemap_ = create_hdr_cubemap(filename + "_cross.hdr")  ;
        glut_viewer_set_skybox(cubemap_) ;
    }

    void GeexApp::inc_frame() {
		float nf = ::fabs((float)scene()->nb_frames()) ;
        frame_ += frame_delta_ ;
        if(frame_ < 0) { 
            if(frame_wrap_) {
                frame_ = nf - 1.0f ;
            } else {
                frame_ = 0.0f ; 
                frame_delta_ = -frame_delta_ ; 
            }
        }
        if(frame_ >= nf) { 
            if(frame_wrap_) {
                frame_ = 0.0f ;
            } else {
                frame_ = nf - 1.0f ;
                frame_delta_ = -frame_delta_ ; 
            }
        }
    }

    void GeexApp::dec_frame() {
        float nf = ::fabs((float)scene()->nb_frames()) ;
        frame_ -= frame_delta_ ;
        if(frame_ < 0) { 
            frame_ = 0 ; 
            frame_delta_ = -frame_delta_ ; 
        }
        if(frame_ >= nf) { 
            frame_ = nf - 1.0f ; 
            frame_delta_ = -frame_delta_ ; 
        }
    }

    GLhandleARB GeexApp::load_program(const std::string& shader_base_name) {
        std::string vp_filename = 
            Geex::FileSystem::get_project_root() + "/data/shaders/" + shader_base_name + "_vp.glsl" ;
        std::string fp_filename =
            Geex::FileSystem::get_project_root() + "/data/shaders/" + shader_base_name + "_fp.glsl" ;
        return glut_viewer_load_program((char*)vp_filename.c_str(), (char*)fp_filename.c_str()) ;
    }


    void GeexApp::initialize() {
        std::cerr 
            << glGetString(GL_VENDOR) << " -- " 
            << glGetString(GL_RENDERER) << " -- " 
            << glGetString(GL_VERSION) << std::endl ;
        
        // TODO: is there another way of checking whether Floating Point Textures
        // filtering is supported ?
        // Note: forced to TRUE for the moment
        fp_filtering_ = hdr_ ;
        if(strstr((char*)glGetString(GL_VENDOR), "NVIDIA") == NULL) {
//            fp_filtering_ = false ;
        }

        if(hdr_) {
            set_cubemap("rnl") ;
        }
        init_scene() ;
        
        Geex::real xmin, ymin, zmin, xmax, ymax, zmax ;
        scene()->get_bbox(xmin, ymin, zmin, xmax, ymax, zmax) ;
        glut_viewer_set_region_of_interest(
            float(xmin), float(ymin), float(zmin), float(xmax), float(ymax), float(zmax)
        ) ;
        init_gui() ;   

    }
    
    void GeexApp::initialize_CB() {
        instance()->initialize() ;
    }

    void GeexApp::toggle_skybox_CB() {
        if(glut_viewer_get_skybox() == 0) {
            glut_viewer_set_skybox(GeexApp::instance()->cubemap_) ;
            glut_viewer_enable(GLUT_VIEWER_HDR_VIGNETTE) ;
        } else {
            glut_viewer_set_skybox(0) ;
            glut_viewer_disable(GLUT_VIEWER_HDR_VIGNETTE) ;
        }
    }

    void GeexApp::init_scene() {
    }

    void GeexApp::init_gui() {
        if(scene()->nb_frames() > 1 || scene()->nb_frames() < 0) {
            viewer_properties_->add_separator("Animation") ;
            viewer_properties_->add_toggle("Animate", animate_) ;
            if(scene()->nb_frames() > 0) {
                viewer_properties_->add_slider("Delay", delay_, 0.01f, 0.05f) ;
                viewer_properties_->add_toggle("Wrap anim.", frame_wrap_) ;
                viewer_properties_->add_slider("Frame", frame_, 0.0f, float(scene()->nb_frames()-1)) ;
            }
        }        
        glut_viewer_add_toggle('t', &viewer_properties_->visible(), "viewer properties") ;   
        glut_viewer_add_toggle('a', &animate_, "animation") ;   
        glut_viewer_add_key_func('B', toggle_skybox_CB, "toggle skybox") ;
    }

    void GeexApp::display_CB() {
        instance()->display() ;
	static bool first_display = true ;
	if(first_display) {
	    first_display = false ;
	    for(unsigned int i=1; i+1<instance()->argc(); i++) {
		if(!strcmp(instance()->argv()[i],"-cmd")) {
		    for(char* c = instance()->argv()[i+1]; *c != '\0'; c++) {
			glut_viewer_keyboard_callback(*c, 0, 0) ;
		    }
		}
		if(!strcmp(instance()->argv()[i],"-bkg")) {
		    std::cerr << "loading background tex: " << instance()->argv()[i+1] << std::endl ;
		    GLuint tex ;
		    glGenTextures(1,&tex) ;
		    glBindTexture(GL_TEXTURE_2D, tex) ;
		    glTexImage2Dfile(instance()->argv()[i+1]) ;		    
		    glut_viewer_set_background(tex) ;
		}
	    }
	}
    }

    void GeexApp::display() {
        if(animate_) {
            glut_viewer_enable(GLUT_VIEWER_IDLE_REDRAW) ;
        } else {
            glut_viewer_disable(GLUT_VIEWER_IDLE_REDRAW) ;
        }
   
        scene()->set_frame(int(frame_)) ;
        viewer_properties_->apply() ;
        static double now = ::Geex::SystemStopwatch::now() ;
        if(animate_) {
            double n = ::Geex::SystemStopwatch::now() ;
            if(n - now > delay_) {
                now = n ;
                inc_frame() ;
            }
        }

        glActiveTexture(GL_TEXTURE0) ;
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, cubemap_) ;
        glActiveTexture(GL_TEXTURE1) ;
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, diffuse_cubemap_) ;
        glActiveTexture(GL_TEXTURE0) ;

        scene_->draw() ;
    }

    void GeexApp::overlay_CB() {
        instance()->overlay() ;
    }
   
    void GeexApp::overlay() {
        GUI_.draw() ;
    }

    GLboolean GeexApp::mouse_CB(float x, float y, int button, enum GlutViewerEvent event) {
        return instance()->mouse(x, y, button, event) ;
    }

    GLboolean GeexApp::mouse(float x, float y, int button, enum GlutViewerEvent event) {
        return GUI_.process_mouse_event(x, y, button, event) ;
    }

    void GeexApp::main_loop() {
        in_main_loop_ = GL_TRUE ;
        if(hdr_) {
            glut_viewer_enable(GLUT_VIEWER_HDR) ;
        }
        viewer_properties_ = new GlutViewerGUI::ViewerProperties(400,2900) ;
        GUI_.add_child(viewer_properties_) ;

        glut_viewer_set_display_func(display_CB) ;
        glut_viewer_set_overlay_func(overlay_CB) ;
        glut_viewer_set_mouse_func(mouse_CB) ;
        glut_viewer_set_init_func(initialize_CB) ;
        glut_viewer_set_screen_size(width_, height_) ;
        glut_viewer_main_loop(argc_, argv_) ;
        in_main_loop_ = GL_FALSE ;
    }

}
