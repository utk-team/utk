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

#ifndef __GEEX_GRAPHICS_GEEXAPP__
#define __GEEX_GRAPHICS_GEEXAPP__

#include "geexob.h"
#include <glut_viewer/glut_viewer_gui.h>

class HDRtexture ;

namespace Geex {

    class GEEX_API GeexApp {
    public:
        GeexApp(int argc, char** argv) ;
        virtual ~GeexApp() ; 
        int argc() { return argc_ ; }
        char** argv() { return argv_ ; }

        static GeexApp* instance() { return instance_ ; }
        Geexob* scene() { return scene_ ; }

        void main_loop() ;

        // Creates a slider connected to a uniform variable 
        // of the shader (works for both vertex and fragment 
        // shaders)
        void create_gui_for_uniform(
            const std::string& caption,            // The name displayed in the GUI near the slider
            const std::string& uniform_name, // The name of the uniform variable in the shader
            GLfloat initial_value,                      // Initial value of the variable / position of the slider
            GLfloat range_min = 0.0f,              // The slider is mapped to [range_min...range_man] 
            GLfloat range_max = 1.0f,
            Geexob* object = nil
        ) {			
            if(object == nil) { object = scene_ ; }
            viewer_properties_->add_slider(
                caption, object->new_uniform(uniform_name, initial_value), range_min, range_max
            ) ;
        }

        void get_arg(std::string arg_name, GLint& arg_val) ;
        void get_arg(std::string arg_name, GLfloat& arg_val) ;
        void get_arg(std::string arg_name, GLboolean& arg_val) ;
        void get_arg(std::string arg_name, std::string& arg_val) ;
        std::string get_file_arg(std::string extension, int start_index = 1) ;

        GLuint create_hdr_cubemap(const std::string& filename) ;
        void set_cubemap(const std::string& filename) ;

        void inc_frame() ;
        void dec_frame() ;

        virtual void init_scene() ;
        virtual void init_gui() ;

        static GLhandleARB load_program(const std::string& shader_base_name) ;

        GLboolean in_main_loop() const { return in_main_loop_ ; }

    protected:

        // Member-function versions of the callbacks
        virtual void initialize() ;
        virtual void display() ;
        virtual void overlay() ;
        virtual GLboolean mouse(float x, float y, int button, enum GlutViewerEvent event) ;

        // Callbacks
        static void initialize_CB() ;
        static void toggle_skybox_CB() ;
        static void display_CB() ;
        static void overlay_CB() ;
        static GLboolean mouse_CB(float x, float y, int button, enum GlutViewerEvent event) ;


    protected:
        // GeexApp management.
        static GeexApp* instance_ ;
        int argc_ ;
        char** argv_ ;
        int width_ ;
        int height_ ;

        GLboolean in_main_loop_ ;

        // SceneGraph
        Geexob* scene_ ;

        // Rendering
        GLboolean hdr_ ;
        GLuint diffuse_cubemap_ ;
        GLuint cubemap_ ;
        GLboolean fp_filtering_ ;

        // GUI
        GlutViewerGUI::Container GUI_ ; 
        GlutViewerGUI::ViewerProperties* viewer_properties_ ;

        // Animation
        GLfloat frame_ ;
        GLfloat delay_ ;
        int frame_delta_ ;
        GLboolean frame_wrap_ ;
        GLboolean animate_ ;
        
    } ;
    
}

#endif
