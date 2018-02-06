/*
 *    _____   _       _   _   _____        _     _   _   _____   _          __  _____   _____   
 *   /  ___| | |     | | | | |_   _|      | |   / / | | | ____| | |        / / | ____| |  _  \  
 *   | |     | |     | | | |   | |        | |  / /  | | | |__   | |  __   / /  | |__   | |_| |  
 *   | |  _  | |     | | | |   | |        | | / /   | | |  __|  | | /  | / /   |  __|  |  _  /  
 *   | |_| | | |___  | |_| |   | |        | |/ /    | | | |___  | |/   |/ /    | |___  | | \ \  
 *   \_____/ |_____| \_____/   |_|        |___/     |_| |_____| |___/|___/     |_____| |_|  \_\
 *
 *  Version 1.0
 *  Bruno Levy, August 2009
 *  INRIA, Project ALICE
 * 
 * Note: used by glut_viewer.c, should not be included directly
 *
 */

#ifndef __GLUT_VIEWER_HDR__
#define __GLUT_VIEWER_HDR__


#ifdef __cplusplus
extern "C" {
#endif

    void GLUT_VIEWER_API glut_viewer_hdr_init() ;
    void GLUT_VIEWER_API glut_viewer_hdr_resize(int width, int height) ;
    void GLUT_VIEWER_API glut_viewer_hdr_begin_frame() ;
    void GLUT_VIEWER_API glut_viewer_hdr_end_frame() ;
    GLhandleARB GLUT_VIEWER_API glut_viewer_load_program(char* vp, char* fp) ;
    void GLUT_VIEWER_API glut_viewer_draw_skybox(int cube_texture)  ;
    GLuint GLUT_VIEWER_API glut_viewer_hdr_load_cubemap(const char* filename) ;

#ifdef __cplusplus
}
#endif

#endif

