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
 
#include <Geex/graphics/opengl.h>
#include <iostream>
#include <malloc.h>
#include <stdio.h>

// Knowing the dimension of the OpenGL window
// is required by the raytraced spheres vertex and 
// fragment programs. Note: we could make it 
// cleaner by using the GL_POINT_SPRITE extension.

namespace Geex {

    static const char* sphere_vertex_program_source = 
        "!!ARBvp1.0" 
        "ATTRIB iPos        = vertex.position;       " 
        "ATTRIB iCol        = vertex.color ;         "
        "PARAM mvp[4]       = { state.matrix.mvp };  "
        "PARAM point_size   = state.point.size ;     "
        "PARAM screen_scale = program.local[0] ;     "
        "PARAM zero         = {0, 0, 0, 0} ;         " 
        "                                            "
        "TEMP R0, R1 ;                               "
        "                                            "
        "OUTPUT oPos    = result.position;           " 
        "OUTPUT oCol    = result.color ;             "
        "OUTPUT oCenter = result.texcoord[0] ;       "
        "OUTPUT oRadius = result.texcoord[1] ;       "
        "OUTPUT oPtSize = result.pointsize ;         "
        "                                            "
        "DP4 R1.x, mvp[0], iPos;                     "
        "DP4 R1.y, mvp[1], iPos;                     "
        "DP4 R1.z, mvp[2], iPos;                     "
        "DP4 R1.w, mvp[3], iPos;                     "
        "RCP R0.x, R1.w;                             "
        "MUL R0, R1, R0.x;                           "
        "                                            " 
        "MOV oPos,    R0;                            "
        "MOV oCenter.xy, R0;                         "
        "MOV oCenter.zw, zero ;                      "
        "                                            "
        "MUL R0.x, point_size.x, screen_scale.x ;    "
        "MOV oPtSize.x, point_size.x ;               "
        "                                            " 
        "MOV oRadius.x, R0.x ;                       "
        "MUL oRadius.y, R0.x, R0.x ;                 "
        "RCP oRadius.z, R0.x ;                       " 
        "                                            " 
        "MOV oCol, iCol ;                            "
        "                                            "
        "END                                         "
        ;
    
    static const char* sphere_fragment_program_source = 
        "!!ARBfp1.0" 
        "ATTRIB iPos    = fragment.position ;      "
        "ATTRIB iCol    = fragment.color ;         " 
        "                                          "
        "ATTRIB iCenter = fragment.texcoord[0] ;   " 
        "ATTRIB iRadius = fragment.texcoord[1] ;   "
        "                                          "
        "PARAM screen_offset = program.local[0] ;  "
        "PARAM screen_scale  = program.local[1] ;  "
        "                                          "
        "TEMP R0, H0 ;                             "
        "                                          "
        "ADD R0, iPos, screen_offset ;             "
        "MAD R0, screen_scale, R0, iCenter ;       "
        "                                          "
        "DP3 R0.z, R0, R0;                         "
        "ADD R0.z, iRadius.y, -R0.z ;              "
        "                                          "
        "KIL R0.z ;                                "
        "                                          "
        "RSQ R0.z, R0.z;                           "
        "RCP R0.z, R0.z;                           "
        "                                          "
//         Removed the depth update code (does not work in perspective mode)
//        "MAD result.depth, R0.z, state.matrix.projection.row[2].z, fragment.position.z ; "
        "                                          "
        "MUL R0, R0, -iRadius.z ;                  "
        "DP3 R0.x,  R0,  -state.light[0].position ; "
        "MUL result.color, R0.x, iCol ;            "
        "                                          " 
        "END                                       " ;
    
    static GLuint sphere_vertex_program_id = 0 ;
    static GLuint sphere_fragment_program_id = 0 ;

    static void create_sphere_programs() {
        GLint errpos ;

        glGenProgramsARB(1, &sphere_vertex_program_id) ;        
        glBindProgramARB(GL_VERTEX_PROGRAM_ARB,sphere_vertex_program_id) ;
        glProgramStringARB(
            GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, 
            (GLsizei)strlen(sphere_vertex_program_source), sphere_vertex_program_source
        ) ;
        glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &errpos) ;
        if(errpos != -1) {
            const char* s = (const char*)(glGetString(GL_PROGRAM_ERROR_STRING_ARB)) ;
            std::cerr << "vertex program ARB error" << ":" << errpos << ": " << s << std::endl ;
        } 

        glGenProgramsARB(1, &sphere_fragment_program_id) ;
        glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,sphere_fragment_program_id) ;
        glProgramStringARB(
            GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, 
            (GLsizei)strlen(sphere_fragment_program_source), sphere_fragment_program_source
        ) ;
        glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &errpos) ;
        if(errpos != -1) {
            const char* s = (const char*)(glGetString(GL_PROGRAM_ERROR_STRING_ARB)) ;
            std::cerr << "fragment program ARB error" << ":" << errpos << ": " << s << std::endl ;
        } 

    }


    static int screen_w = 300 ;
    static int screen_h = 300 ;

    void notify_screen_resize(int w, int h) {
        screen_w = w ; screen_h = h ;
    }

    void begin_spheres() {
		
        if(sphere_vertex_program_id == 0 && sphere_fragment_program_id == 0) {
            create_sphere_programs() ;
        }
        
        glDisable(GL_NORMALIZE) ;
        glEnable(GL_VERTEX_PROGRAM_ARB) ;
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB) ;
        glBindProgramARB(GL_VERTEX_PROGRAM_ARB, sphere_vertex_program_id) ;
        glEnable(GL_FRAGMENT_PROGRAM_ARB) ;
        glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, sphere_fragment_program_id) ;

        // For the moment, this is the only way we have to find the 
        // dimension of the OpenGL window.
        float w=float(screen_w), h=float(screen_h), r=gx_max(w,h) ;
        
        glProgramLocalParameter4dARB(GL_VERTEX_PROGRAM_ARB, 0, 1.0/r, 0, 0, 0.2) ;
        glProgramLocalParameter4dARB(GL_FRAGMENT_PROGRAM_ARB, 0, -w/2.0, -h/2.0, 0, 0) ;
        glProgramLocalParameter4dARB(GL_FRAGMENT_PROGRAM_ARB, 1, -2.0/r, -2.0/r, 0, 0) ;

        glBegin(GL_POINTS) ;
    }
    
    void end_spheres() {
        glEnd() ;
        glDisable(GL_VERTEX_PROGRAM_ARB) ;
        glDisable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB) ;
        glDisable(GL_FRAGMENT_PROGRAM_ARB) ;
        glEnable(GL_NORMALIZE) ;
    }

    static unsigned char i2r[255] ;
    static unsigned char i2g[255] ;
    static unsigned char i2b[255] ;
    static unsigned char i2a[255] ;

    inline int htoi(char digit) {
        if(digit >= '0' && digit <= '9') {
            return digit - '0' ;
        }
        if(digit >= 'a' && digit <= 'f') {
            return digit - 'a' + 10 ;
        }
        if(digit >= 'A' && digit <= 'F') {
            return digit - 'A' + 10 ;
        }   
        fprintf(stderr, "xpm: unknown digit\n") ;
        return 0 ; 
    }
    
    GLuint create_texture_from_xpm_data(const char** xpm_data) {
        int width, height, nb_colors, chars_per_pixel ;
        int line = 0 ;
        int color = 0 ;
        int key = 0 ;
        const char* colorcode ;
        int r,g,b ;
        int none ;
        int x,y ;
        unsigned char* rgba ;
        unsigned char* pixel ;
        GLuint result ;
        sscanf(xpm_data[line], "%d%d%d%d", &width, &height, &nb_colors, &chars_per_pixel) ;
        line++ ;
        if(nb_colors > 256) {
            fprintf(stderr, "xpm with more than 256 colors\n") ;
            return 0 ;
        }
        if(chars_per_pixel != 1) {
            fprintf(stderr, "xpm with more than 1 char per pixel\n") ;
            return 0 ;
        }
        for(color=0; color<nb_colors; color++) {
            key = xpm_data[line][0] ;
            colorcode = strstr(xpm_data[line], "c #") ;        
            none = 0 ;
            if(colorcode == NULL) {
                colorcode = "c #000000" ;
                if(strstr(xpm_data[line], "None") != NULL) {
                    none = 1 ;
                    fprintf(stderr, "unknown xpm color entry (replaced with black)\n") ;
                }
            }
            colorcode += 3 ;
            
            r = 16 * htoi(colorcode[0]) + htoi(colorcode[1]) ;
            g = 16 * htoi(colorcode[2]) + htoi(colorcode[3]) ;
            b = 16 * htoi(colorcode[4]) + htoi(colorcode[5]) ;      
            
            i2r[key] = (unsigned char)r ;
            i2g[key] = (unsigned char)g ;
            i2b[key] = (unsigned char)b ;
            i2a[key] = none ? 0 : 255 ;
            line++ ;
        }
        rgba = (unsigned char*)malloc(width * height * 4) ;
        pixel = rgba ;
        for(y=0; y<height; y++) {
            for(x=0; x<width; x++) {
                key = xpm_data[line][x] ;
                pixel[0] = i2r[key] ;
                pixel[1] = i2g[key] ;
                pixel[2] = i2b[key] ;
                pixel[3] = i2a[key] ;
                pixel += 4 ;
            }
            line++ ;
        }
        glGenTextures(1, &result) ;
        glBindTexture(GL_TEXTURE_2D, result) ;
        gluBuild2DMipmaps(
            GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, rgba
        ) ;
        free(rgba) ;
        return result ;
    }

}
