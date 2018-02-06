/*
 *    _____   _       _   _   _____        _     _   _   _____   _          __  _____   _____   
 *   /  ___| | |     | | | | |_   _|      | |   / / | | | ____| | |        / / | ____| |  _  \  
 *   | |     | |     | | | |   | |        | |  / /  | | | |__   | |  __   / /  | |__   | |_| |  
 *   | |  _  | |     | | | |   | |        | | / /   | | |  __|  | | /  | / /   |  __|  |  _  /  
 *   | |_| | | |___  | |_| |   | |        | |/ /    | | | |___  | |/   |/ /    | |___  | | \ \  
 *   \_____/ |_____| \_____/   |_|        |___/     |_| |_____| |___/|___/     |_____| |_|  \_\
 *
 *  Version 0.9
 *  Bruno Levy, August 2006
 *  INRIA, Project ALICE
 * 
 */

#ifndef __GLUT_VIEWER_GUI__
#define __GLUT_VIEWER_GUI__

#include "glut_viewer.h"
#include <vector>
#include <string>
#include <assert.h>

namespace GlutViewerGUI {

//_____________________________________________________________________________________________


    enum Direction {UP, DOWN, LEFT, RIGHT} ;
   
    class Container ;
   
    class GLUT_VIEWER_API Widget {
    public:
      
        enum Style { RedStyle, GreenStyle, BlueStyle, BWStyle } ;
        enum ColorRole { Foreground, Background, Middleground } ;
	    
      
        Widget(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) ;
        virtual ~Widget() ;
        void set_style(Style x) { style_ = x ; }
        void set_geometry(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)	{
            x1_ = x1 ; y1_ = y1 ; x2_ = x2 ; y2_ = y2 ;
            d_ = 0.25f * (y2_ - y1_) ;
        }
        void get_geometry(GLfloat& x1, GLfloat& y1, GLfloat& x2, GLfloat& y2)	{
            x1 = x1_ ; y1 = y1_ ; x2 = x2_ ; y2 = y2_ ;
        }
        void show() { visible_ = GL_TRUE ; }
        void hide() { visible_ = GL_FALSE ; }
        GLboolean& visible() { return visible_ ; }
	    
        virtual void draw() ;
        virtual GLboolean process_mouse_event(float x, float y, int button, GlutViewerEvent event) ;
      
    protected:

        virtual void draw_background() ;
        virtual void draw_border() ;
      
        GLboolean contains(float x, float y) const	{
            return (x >= x1_ && x <= x2_ && y >= y1_ && y <= y2_) ;
        }
      
        void glColor(ColorRole x) ;
      

    protected:
        GLfloat x1_, y1_, x2_, y2_ ;
        GLfloat d_ ; // Margin
        GLfloat r_ ; // Round border radius
        Style style_ ;
        GLboolean visible_ ;
        GLboolean highlight_ ;
      
        friend class ::GlutViewerGUI::Container ;
    } ;
   
//_____________________________________________________________________________________________   

    class GLUT_VIEWER_API Container : public Widget {
    public:
        Container(
            GLfloat x1=0.0f, GLfloat y1=0.0f, GLfloat x2=100.0f, GLfloat y2=100.0f
        ) : Widget(x1, y1, x2, y2), active_child_(NULL) { }
        virtual ~Container() ;
        virtual void draw() ;
        virtual GLboolean process_mouse_event(float x, float y, int button, GlutViewerEvent event) ;
        void add_child(Widget* w)	{ children_.push_back(w) ; }
        void set_as_main_widget() ;
        unsigned int nb_children() { return (unsigned int)children_.size() ; }
        Widget* ith_child(unsigned int i) { return children_[i] ; }

    private:
        static void draw_handler() ;
        static GLboolean mouse_handler(float x, float y, int button, enum GlutViewerEvent event) ;
        static Container* main_widget_ ;
        std::vector<Widget*> children_ ;
        Widget* active_child_ ;
    } ;
   
//_____________________________________________________________________________________________

    class GLUT_VIEWER_API Panel : public Container {
    public:
        Panel(
            GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2
        )	: Container(x1,y1,x2,y2) { }
        virtual void draw() ;
        virtual GLboolean process_mouse_event(float x, float y, int button, GlutViewerEvent event) ;
    } ;
   
//_____________________________________________________________________________________________   

    typedef void (*ButtonCallback)(void* data) ;

    class GLUT_VIEWER_API Button : public Widget {
    public:
        Button(
            GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2
        ) : Widget(x1, y1, x2, y2), callback_(NULL), client_data_(NULL) {
        }
        virtual void draw() ;
        virtual GLboolean process_mouse_event(float x, float y, int button, GlutViewerEvent event) ;
        void set_callback(ButtonCallback x, void* client_data = NULL) { callback_ = x ; client_data_ = client_data ; }
    protected:
        virtual void pressed() ;
    private:
        ButtonCallback callback_ ;
        void* client_data_ ;
    } ;
   

//_____________________________________________________________________________________________   

    class GLUT_VIEWER_API Checkbox : public Button {
    public:
        Checkbox(
            GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLboolean& toggle
        ) : Button(x1, y1, x2, y2), toggle_(toggle) {
        }
        virtual void draw() ;
    protected:
        virtual void pressed() ;
    private:
        GLboolean& toggle_ ;
    } ;
   
//_____________________________________________________________________________________________   
   
    class GLUT_VIEWER_API ArrowButton : public Button {
    public:
        ArrowButton(Direction dir, GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) ;
        virtual void draw() ;
    private:
        Direction direction_ ;
    } ;

//_____________________________________________________________________________________________   
   
    typedef void (*SliderCallback)(GLfloat data) ;
   
    class GLUT_VIEWER_API Slider : public Widget {
    public:
        Slider(
            GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat& value
        ) : Widget(x1,y1,x2,y2), value_(value), min_(0.0f), max_(1.0f), callback_(NULL), continuous_update_(GL_TRUE), integer_(GL_FALSE) {  }
        GLfloat& value() { return value_ ; }
        void set_value(GLfloat x, bool update = true) ;
        void set_range(GLfloat x1, GLfloat x2) ;
        void set_callback(SliderCallback cb) { callback_ = cb ; }
        void set_integer(GLboolean x) { integer_ = x ; }
        GLboolean& continuous_update() { return continuous_update_ ; }

    protected:
        virtual void draw() ;
        virtual GLboolean process_mouse_event(float x, float y, int button, GlutViewerEvent event) ;
      
    private:
        GLfloat& value_ ;
        GLfloat min_, max_ ;
        SliderCallback callback_ ;
        GLboolean continuous_update_ ;
        GLboolean integer_ ;
    } ;
   
//_____________________________________________________________________________________________

    typedef void (*CurveEditorCallback)(GLfloat* data, GLuint size) ;
   
    class GLUT_VIEWER_API CurveEditor : public Widget {
    public:
        enum { CurveSize = 1000 } ;
        CurveEditor(
            GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, CurveEditorCallback CB
        ) : Widget(x1, y1, x2, y2), callback_(CB) {
            for(unsigned int i=0; i<CurveSize; i++) {
                curve_[i] = 0.5 ;
            }
        }
        virtual void draw() ;
        virtual GLboolean process_mouse_event(float x, float y, int button, GlutViewerEvent event) ;
        void set_curve(GLfloat* curve, bool update = false) ;
        void reset(bool update = false) ;
        void reset_ramp(bool update = false) ;
        GLfloat value(GLfloat x) const ;
        GLfloat* curve() { return curve_ ; }

    protected:
        void set_curve(int i1, float val1, int i2, float val2) ;
      
    private:
        GLfloat curve_[CurveSize] ;
        CurveEditorCallback callback_ ;
        int last_i_ ;
        GLfloat last_v_ ;
    } ;
  
//_____________________________________________________________________________________________

    typedef void (*DrawColormapEditorBackgroundCallback)(GLfloat* data, GLuint size) ;
   
    class GLUT_VIEWER_API ColormapEditor : public CurveEditor {
    public:
        enum { ColormapSize = 1000 } ;
        ColormapEditor(
            GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2,
			CurveEditorCallback CB, DrawColormapEditorBackgroundCallback dCB
        ) : CurveEditor(x1, y1, x2, y2, CB), drawBackgroundCB_(dCB) {
            for(unsigned int i=0; i<ColormapSize; i++) {
                curve()[i] = (GLfloat)(double(i) / double(ColormapSize)) ;
            }
        }
        virtual void draw() ;
		virtual void draw_background() ;
		virtual void draw_border() ;

		void update(unsigned char* cmap_data, int size, int component) ;
	
	private:
		DrawColormapEditorBackgroundCallback drawBackgroundCB_ ;
    } ; 
   
//_____________________________________________________________________________________________
   
    class GLUT_VIEWER_API TextLabel : public Widget {
    public:
        TextLabel(
            GLfloat x, GLfloat y, const std::string& text="", GLfloat textwidth = 1.0
        ) : Widget(x,y,x+10,y+10), text_(text), textwidth_(textwidth) { }
        void set_text(const std::string& text) { text_ = text ;	}
        const std::string& text() const { return text_ ; }
        virtual void draw() ;
    private:
        std::string text_ ;
        GLfloat textwidth_ ;
    } ;

//_____________________________________________________________________________________________   

    class GLUT_VIEWER_API LabelList : public std::vector<std::string> {
    public:
        LabelList() { }
        LabelList(const std::string& s) { push_back(s) ; }
        LabelList& operator|(const std::string& s) { push_back(s) ; return *this ;}
        LabelList& operator|=(const std::string& s) { push_back(s) ; return *this ;}
    } ;

    class GLUT_VIEWER_API Spinbox : public Container {
    public:
        Spinbox(GLfloat x, GLfloat y, GLenum& value, const std::vector<std::string>& labels) ;
        void increment() ;
        void decrement() ;
        virtual void draw() ;

    protected:
        static void increment_CB(void* spinbox) ;
        static void decrement_CB(void* spinbox) ;

        GLenum& value_ ;
        std::vector<std::string> labels_ ;

        ArrowButton* up_ ;
        ArrowButton* down_ ;
        TextLabel* text_ ;
    } ;

//_____________________________________________________________________________________________   

    class GLUT_VIEWER_API MessageBox : public Panel {
    public:
        enum {Width = 3000} ;
        MessageBox(GLfloat x_left, GLfloat y_top) : Panel(x_left, y_top - 150, x_left + Width, y_top) {
        }
        void add_message(const std::string& s) { 
            message_.push_back(s) ; y1_ -= 150 ;
        }
        void clear_message() { message_.clear() ; y1_ = y2_ - 150 ; }
        void set_message(int i, const std::string& s) {
            assert(i < (int)message_.size()) ;
            message_[i] = s ; 
        }
        virtual void draw() ;
    protected:
        std::vector<std::string> message_ ;
    } ;

//_____________________________________________________________________________________________   
   
    class GLUT_VIEWER_API PropertyPage : public Panel {
    public:
        enum { Width = 2500} ;
        PropertyPage(GLfloat x_left, GLfloat y_top, const std::string& caption) ;
        Slider* add_slider(
            const std::string& caption, GLfloat& value, GLfloat vmin = 0.0f, GLfloat vmax = 1.0f
        ) ;
        Checkbox* add_toggle(const std::string& caption, GLboolean& value) ;
        TextLabel* add_separator(const std::string& text) ;
        TextLabel* add_string(const std::string& text) ;
        Spinbox* add_enum(const std::string& caption, GLenum& value, const std::vector<std::string>& labels) ;
      
    protected:
        TextLabel* caption_ ;
        GLfloat y_ ;
        GLfloat x_caption_ ;
        GLfloat x_widget_ ;
    } ;

//_____________________________________________________________________________________________
   
    class GLUT_VIEWER_API ViewerProperties : public PropertyPage {
    public:
        ViewerProperties(GLfloat x_left, GLfloat y_top) ;
        void apply() ;
        virtual void draw() ;
    private:
    } ;
   
//_____________________________________________________________________________________________   

    class GLUT_VIEWER_API Image : public Widget {
    public:
        Image(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLint texture = 0, GLint target = GL_TEXTURE_2D) ;
        void set_texture(GLint x) { texture_ = x ; }
        GLint texture() const { return texture_ ; }
        void set_texture_target(GLint x) { texture_target_ = x ; }
        GLint texture_target() const { return texture_target_ ; }
        virtual void draw() ;
    private:
        GLint texture_ ;
        GLint texture_target_ ;
    } ;

//_____________________________________________________________________________________________   
   
}



#endif

