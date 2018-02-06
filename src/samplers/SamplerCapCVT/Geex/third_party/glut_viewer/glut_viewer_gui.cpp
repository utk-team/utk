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

#include "glut_viewer_gui.h"
#include <GLsdk/gl_stuff.h>
#include <GL/glut.h>

#include <iostream>

#include <stdarg.h>
#include <math.h>
#include <stdio.h>

namespace GlutViewerGUI {

    // ------------------- Primitives for internal use --------------------------------------
   
    static void printf_xy(GLfloat x, GLfloat y, const char *format, ...) {
        va_list args;
        char buffer[1024], *p;
        va_start(args, format);
        vsprintf(buffer, format, args);
        va_end(args);
        glPushMatrix();
        glTranslatef(x, y, 0);
        for (p = buffer; *p; p++) {
            glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *p);
        }
        glPopMatrix();
    }
   
    static void circle_arc_vertices(
        GLfloat x, GLfloat y, GLfloat r1, GLfloat r2, GLfloat theta1, GLfloat theta2
    ) {
        const GLfloat delta_theta = 1.0f ;
        if(theta2 > theta1) {
            for(GLfloat theta = theta1; theta <= theta2; theta += delta_theta) {
                GLfloat theta_rad = theta * 3.14159f / 200.0f ;
                glVertex2f(x + r1 * cos(theta_rad), y + r2 * sin(theta_rad)) ;
            }
        } else {
            for(GLfloat theta = theta1; theta >= theta2; theta -= delta_theta) {
                GLfloat theta_rad = theta * 3.14159f / 200.0f ;
                glVertex2f(x + r1 * cos(theta_rad), y + r2 * sin(theta_rad)) ;
            }
        }
    }

    static void circle_arc_vertices(
        GLfloat x, GLfloat y, GLfloat r, GLfloat theta1, GLfloat theta2
    ) {
        circle_arc_vertices(x,y,r,r,theta1,theta2) ;
    }
   
    static void circle(GLfloat x, GLfloat y, GLfloat r) {
        glBegin(GL_LINE_LOOP) ;
        circle_arc_vertices(x,y,r,0.0f,400.0f) ;
        glEnd() ;
    }
   
    static void fill_circle(GLfloat x, GLfloat y, GLfloat r) {
        glBegin(GL_POLYGON) ;
        circle_arc_vertices(x,y,r,0.0f,400.0f) ;
        glEnd() ;
    }
   
    static void round_rectangle_vertices(
        GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat r
    ) {
        glVertex2f(x1+r,y2) ;
        glVertex2f(x2-r,y2) ;
        circle_arc_vertices(x2-r, y2-r, r, 100.0f, 0.0f) ;
        glVertex2f(x2,y2-r) ;
        glVertex2f(x2,y1+r) ;
        circle_arc_vertices(x2-r, y1+r, r, 0.0f, -100.0f) ;
        glVertex2f(x2-r,y1) ;
        glVertex2f(x1+r,y1) ;
        circle_arc_vertices(x1+r, y1+r, r, -100.0f, -200.0f) ;
        glVertex2f(x1,y1+r) ;
        glVertex2f(x1,y2-r) ;
        circle_arc_vertices(x1+r, y2-r, r, -200.0f, -300.0f) ;
    }
   
    static void round_rectangle(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat r) {
        glBegin(GL_LINE_LOOP) ;
        round_rectangle_vertices(x1, y1, x2, y2, r) ;
        glEnd() ;
    }
   
    static void fill_round_rectangle(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat r) {
        glBegin(GL_POLYGON) ;
        round_rectangle_vertices(x1, y1, x2, y2, r) ;
        glEnd() ;
    }

    static void arrow_vertices(Direction dir, GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) {
        GLfloat x12 = 0.5 * (x1 + x2) ;
        GLfloat y12 = 0.5 * (y1 + y2) ;
        switch(dir) {
        case DOWN:
            glVertex2f(x1,y2) ;
            glVertex2f(x2,y2) ;
            glVertex2f(x12,y1) ;
            break ;
        case UP:
            glVertex2f(x1,y1) ;
            glVertex2f(x2,y1) ;
            glVertex2f(x12,y2) ;
            break ;
        case LEFT:
            glVertex2f(x2,y2) ;
            glVertex2f(x2,y1) ;
            glVertex2f(x1,y12) ;
            break ;
        case RIGHT:
            glVertex2f(x1,y2) ;
            glVertex2f(x1,y1) ;
            glVertex2f(x2,y12) ;
            break ;
        }
    }

    static void arrow(Direction dir, GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) {
        glBegin(GL_LINE_LOOP) ;
        arrow_vertices(dir, x1, y1, x2, y2) ;
        glEnd() ;
    }

    static void fill_arrow(Direction dir, GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) {
        glBegin(GL_POLYGON) ;
        arrow_vertices(dir, x1, y1, x2, y2) ;
        glEnd() ;
    }

    // ------------------- Widget class --------------------------------------   
   
    Widget::Widget(
        GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2
    ) : style_(BlueStyle), visible_(true), highlight_(false) {
        set_geometry(x1, y1, x2, y2) ;
        r_ = 100.0f ;
    }
   
    Widget::~Widget() {
    }
   
    void Widget::glColor(ColorRole role_in) {
        
        ColorRole role = role_in ;
        if(highlight_) {
            switch(role_in) {
            case Background:
                role = Foreground ; break ;
            case Middleground:
                role = Middleground ; break ;
            case Foreground:
                role = Foreground ; break ;
            }
        }

        switch(style_) {
        case RedStyle: {
            switch(role) {
            case Background:
                glColor4f(0.5f, 0.0f, 0.0f, 0.5f) ;
                break ;
            case Middleground:
                glColor4f(1.0f, 0.5f, 0.5f, 1.0f) ;	     
                break ;
            case Foreground:
                glColor4f(5.0f, 5.0f, 5.0f, 1.0f) ;
                break ;
            }
        } break ;
        case GreenStyle: {
            switch(role) {
            case Background:
                glColor4f(0.0f, 0.5f, 0.0f, 0.5f) ;
                break ;
            case Middleground:
                glColor4f(0.5f, 1.0f, 0.5f, 1.0f) ;	     
                break ;
            case Foreground:
                glColor4f(5.0f, 5.0f, 5.0f, 1.0f) ;
                break ;
            }
        } break ;
        case BlueStyle: {
            switch(role) {
            case Background:
                glColor4f(0.0f, 0.0f, 0.5f, 0.5f) ;
                break ;
            case Middleground:
                glColor4f(0.5f, 0.5f, 1.0f, 1.0f) ;	     
                break ;
            case Foreground:
                glColor4f(5.0f, 5.0f, 5.0f, 1.0f) ;
                break ;
            }
        } break ; 
        case BWStyle: {
            switch(role) {
            case Background:
                glColor4f(5.0f, 5.0f, 5.0f, 0.5f) ;
                break ;
            case Middleground:
                glColor4f(0.2f, 0.2f, 0.2f, 1.0f) ;	     
                break ;
            case Foreground:
                glColor4f(0.0f, 0.0f, 0.0f, 1.0f) ;
                break ;
            }
        } break ;
        }
    }
   

    GLboolean Widget::process_mouse_event(float x, float y, int button, GlutViewerEvent event) {
        return contains(int(x),int(y)) ;
    }

    void Widget::draw() {
        if(!visible()) { return ; }
        draw_background() ;
        draw_border() ;
    }

    void Widget::draw_background() {
        glEnable(GL_BLEND) ;
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;
        glColor(Background) ;
        fill_round_rectangle(x1_, y1_, x2_, y2_, r_) ;
        glDisable(GL_BLEND) ;      
    }
   
    void Widget::draw_border() {
        glColor(Foreground) ;
        glLineWidth(2.0) ;
        round_rectangle(x1_, y1_, x2_, y2_, r_) ;
    }
   
//______________________________________________________________________________________________________

    Container* Container::main_widget_ = NULL ;
   
    Container::~Container() {
        if(main_widget_ == this) {
            main_widget_ = NULL ;
        }
        for(size_t i=0; i<children_.size(); i++) {
            delete children_[i] ;
        }
    }

    void Container::draw() {
        if(!visible()) {
            return ; 
        }
        for(size_t i=0; i<children_.size(); i++) {
            children_[i]->draw() ;
        }
    }
   
    GLboolean Container::process_mouse_event(float x, float y, int button, GlutViewerEvent event) {
        if(!visible()) { return GL_FALSE ; }
        switch(event) {
        case GLUT_VIEWER_DOWN: {
            for(size_t i=0; i<children_.size(); i++) {
                if(children_[i]->contains(x,y) && children_[i]->process_mouse_event(x, y, button, event)) {
                    active_child_ = children_[i] ;
                    return GL_TRUE ;
                }
            }
        } break ;
        case GLUT_VIEWER_MOVE: {
            if(active_child_ != NULL) {
                return active_child_->process_mouse_event(x, y, button, event) ;
            }
        } break ;
        case GLUT_VIEWER_UP: {
            if(active_child_ != NULL) {
                Widget* w = active_child_ ;
                active_child_ = NULL ;	     
                return w->process_mouse_event(x, y, button, event) ;
            }
        } break ;
        }
        return GL_FALSE ;
    }
   
    void Container::draw_handler() {
        if(main_widget_ != NULL) {
            main_widget_->draw() ;
        }
    }
   
    GLboolean Container::mouse_handler(float x, float y, int button, enum GlutViewerEvent event) {
        if(main_widget_ != NULL) {
            return main_widget_->process_mouse_event(x, y, button, event) ;
        }
        return GL_FALSE ;
    }
   
    void Container::set_as_main_widget() {
        main_widget_ = this ;
        glut_viewer_set_overlay_func(draw_handler) ;
        glut_viewer_set_mouse_func(mouse_handler) ;
    }
   
   
//______________________________________________________________________________________________________

   
    void Panel::draw() {
        Widget::draw() ;
        Container::draw() ;
    }
   
    GLboolean Panel::process_mouse_event(float x, float y, int button, GlutViewerEvent event) {
        if(!visible() || !contains(x,y)) {
            return GL_FALSE ;
        }
        return Container::process_mouse_event(x,y,button,event) ;
    }

   
//______________________________________________________________________________________________________   
   
    void Button::draw() {
        Widget::draw() ;
    }
    
    GLboolean Button::process_mouse_event(float x, float y, int button, GlutViewerEvent event) {
        if(visible() && contains(x,y) && event == GLUT_VIEWER_DOWN) {
            pressed() ;
            highlight_ = GL_TRUE ;
            return GL_TRUE ;
        }

        if(visible() && contains(x,y) && event == GLUT_VIEWER_UP) {
            highlight_ = GL_FALSE ;
            return GL_TRUE ;
        }

        return GL_FALSE ;
    }

    void Button::pressed() {
        if(callback_ != NULL) {
            callback_(client_data_) ;
        }
    }

//______________________________________________________________________________________________________   

    void Checkbox::draw() {
        if(!visible()) { return ; }
        Button::draw() ;
        glColor(Foreground) ;
      
        GLfloat x = 0.5f * (x1_ + x2_) ;
        GLfloat y = 0.5f * (y1_ + y2_) ;

        if(toggle_) {
            glColor(Middleground) ;
            fill_circle(x,y,d_) ;
            glColor(Foreground) ;
            glLineWidth(1.0f) ;
            circle(x,y,d_) ;      
        }
    }
   
    void Checkbox::pressed() {
        toggle_ = !toggle_ ;
    }

//______________________________________________________________________________________________________   

    ArrowButton::ArrowButton(
        Direction dir, GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2
    ) : Button(x1, y1, x2, y2), direction_(dir) {
        d_ /= 1.5 ;
        r_ /= 2.0 ;
    } 


    void ArrowButton::draw() {
        Button::draw() ;
        glColor(Middleground) ;
        fill_arrow(direction_, x1_ + d_, y1_ + d_, x2_ - d_, y2_ - d_) ;
        glColor(Foreground);
        arrow(direction_, x1_ + d_, y1_ + d_, x2_ - d_, y2_ - d_) ;
    }

//______________________________________________________________________________________________________   

    void Slider::set_value(GLfloat x, bool update) {
        if(integer_) { x = GLfloat(GLint(x)) ; }
        if(x < min_) { x = min_ ; }
        if(x > max_) { x = max_ ; }
        value_ = x ;
        if(update && callback_ != NULL) { callback_(value_) ; }
    }

    void Slider::set_range(GLfloat x1, GLfloat x2) {
         min_ = x1 ;  
         max_ = x2 ; 
         if(value_ < min_) { set_value(min_) ; }
         if(value_ > max_) { set_value(max_) ; }
    }

    void Slider::draw() {
        if(!visible()) { return ; }
        Widget::draw() ;
        glColor(Middleground) ;
        glLineWidth(2.0f) ;
        glBegin(GL_LINES) ;
        glVertex2f(x1_+d_, 0.5f*(y1_+y2_)) ;
        glVertex2f(x2_-d_, 0.5f*(y1_+y2_)) ;      
        glEnd() ;

        GLfloat w = (value_ - min_) / (max_ - min_) ;
        GLfloat x = w*(x2_ - d_) + (1.0f - w)*(x1_ + d_) ;
        GLfloat y = 0.5f*(y1_+y2_) ;
      
        glColor(Middleground) ;
        fill_circle(x,y,d_) ;
        glColor(Foreground) ;
        glLineWidth(1.0f) ;
        circle(x,y,d_) ;      
    }
   

    GLboolean Slider::process_mouse_event(float x, float y, int button, GlutViewerEvent event) {
        if(!visible()) { return GL_FALSE ; }
        if(event == GLUT_VIEWER_DOWN || event == GLUT_VIEWER_MOVE) {
            GLfloat w = GLfloat(x - x1_ - d_) / GLfloat(x2_ - x1_ - 2.0f * d_) ;
            set_value((1.0f - w) * min_ + w * max_, continuous_update_ == GL_TRUE) ;
            return GL_TRUE ;
        } else if(event == GLUT_VIEWER_UP) {
            set_value(value_) ;
        }
        return GL_FALSE ;
    }

   
//______________________________________________________________________________________________________

    void CurveEditor::draw() {
        if(!visible()) { return ; }
      
        draw_background() ;
      
        // Draw grid
        glColor(Middleground) ;
        glLineWidth(1.0) ;    
        glBegin(GL_LINES) ;
        for(unsigned int i=1; i<10; i++) {
            float x = x1_ + (x2_ - x1_) * float(i) / 10.0f ;
            glVertex2f(x, y1_) ;
            glVertex2f(x, y2_) ;
        }
        for(unsigned int i=1; i<4; i++) {
            float y = y1_ + (y2_ - y1_) * float(i) / 4.0f ;
            glVertex2f(x1_, y) ;
            glVertex2f(x2_, y) ;
        }
        glEnd() ;

        // Draw curve
        glColor(Foreground) ;
        glLineWidth(2.0) ;
        glBegin(GL_LINE_STRIP) ;
        for(unsigned int i=0; i<CurveSize; i++) {
            glVertex2f(
                x1_ + (float)i * (x2_ - x1_) / (float)(CurveSize - 1),
                y1_ + curve_[i] * (y2_ - y1_)
            ) ;
        }
        glEnd() ;

        draw_border() ;
    }
   
    GLboolean CurveEditor::process_mouse_event(float x, float y, int button, GlutViewerEvent event) {
        if(!visible()) {
            return GL_FALSE ;
        }
      
        if(event == GLUT_VIEWER_DOWN && !contains(x,y)) {
            return GL_FALSE ;
        }
      
      
        int i = int((x - x1_) * (CurveSize - 1) / (x2_ - x1_)) ;
        GLfloat v = GLfloat(y - y1_) / GLfloat(y2_ - y1_) ;

        if(v < 0.0) { v = 0.0 ; }
        if(v > 1.0) { v = 1.0 ; }
        if(i < 0) { i = 0 ; }
        if(i >= CurveSize) { i = CurveSize - 1 ; }
      
        if(event == GLUT_VIEWER_DOWN) {
            last_i_ = i ;
            last_v_ = v ;
            return GL_TRUE ;
        }
      
        if(event == GLUT_VIEWER_UP) {
            if(callback_ != NULL) {
                callback_(curve_, CurveSize) ;
            }
            return GL_TRUE ;
        }
      
        if(event == GLUT_VIEWER_MOVE) {
            if(i > last_i_) {
                set_curve(last_i_, last_v_, i, v) ;
            } else {
                set_curve(i, v, last_i_, last_v_) ;
            }
        } 
        last_i_ = i ;
        last_v_ = v ;
      
        return GL_TRUE ;
    }
   
   
    void CurveEditor::set_curve(int i1, float val1, int i2, float val2) {
        if(i1 == i2) {
            curve_[i1] = val1 ;
        } else {
            for(int i=i1; i<=i2; i++) {
                curve_[i] = val1 + (float)(i - i1) * (val2 - val1) / (float)(i2 - i1) ;
            }
        }
    }
   
    void CurveEditor::set_curve(GLfloat* curve, bool update) {
        for(unsigned int i=0; i<CurveSize; i++) {
            curve_[i] = curve[i] ;
        }
        if(update && callback_ != NULL) {
            callback_(curve_, CurveSize) ;
        }
    }
   
    void CurveEditor::reset(bool update) {
        for(unsigned int i=0; i<CurveSize; i++) {
            curve_[i] = 0.5f ;
        }
        if(update && callback_ != NULL) {
            callback_(curve_, CurveSize) ;
        }
    }

    void CurveEditor::reset_ramp(bool update) {
        for(unsigned int i=0; i<CurveSize; i++) {
            curve_[i] = float(i) / float(CurveSize - 1) ;
        }
        if(update && callback_ != NULL) {
            callback_(curve_, CurveSize) ;
        }
    }
   
    GLfloat CurveEditor::value(GLfloat x) const {
        if(x < 0.0f) { x = 0.0f ; }
        if(x > 1.0f) { x = 1.0f ; }
        return curve_[int(x * (CurveSize - 1))] ;
    }
   
   
//______________________________________________________________________________________________________

    void ColormapEditor::draw() {
        if(!visible()) { return ; }
      
        draw_background() ;
      
        // Draw curve
        glColor(Foreground) ;
        glLineWidth(2.0) ;
        glBegin(GL_LINE_STRIP) ;
        for(unsigned int i=0; i<ColormapSize; i++) {
            glVertex2f(
                x1_ + (float)i * (x2_ - x1_) / (float)(ColormapSize - 1),
                y1_ + curve()[i] * (y2_ - y1_)
            ) ;
        }
        glEnd() ;

        draw_border() ;
    }

    void ColormapEditor::draw_background() {
        glEnable(GL_BLEND) ;
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;
        drawBackgroundCB_(curve(), ColormapSize) ;
        glDisable(GL_BLEND) ;      
    }

    void ColormapEditor::draw_border() {
        glColor(Foreground) ;
        glLineWidth(2.0) ;
        glBegin(GL_LINE_LOOP) ;
        glVertex2f(x1_, y1_) ;
        glVertex2f(x1_, y2_) ;
        glVertex2f(x2_, y2_) ;
        glVertex2f(x2_, y1_) ;
        glEnd() ;
    }

    void ColormapEditor::update(unsigned char* cmap_data, int size, int component) {
        for(unsigned int i = 0; i < ColormapSize; ++i) {
            int idx = (double(i) / double(ColormapSize)) * (size-1) ;
            curve()[i] = double(cmap_data[4*idx + component]) / 255.0 ;
        }
    }
  
//______________________________________________________________________________________________________
   
    void TextLabel::draw() {
        if(!visible()) { return ; }
        glLineWidth(textwidth_) ;
        printf_xy(x1_+10, y1_+50, (char*)text_.c_str()) ;
    }
   
   
//______________________________________________________________________________________________________

    Spinbox::Spinbox(
        GLfloat x, GLfloat y, GLenum& value, const std::vector<std::string>& labels
    ) : Container(x, y, x+3000, y+170), value_(value), labels_(labels) {

        down_ = new ArrowButton(DOWN, x, y, x+170, y+170) ;
        up_ = new ArrowButton(UP, x+200, y, x+370, y+170) ;
 
        up_->set_callback(increment_CB, this) ;
        down_->set_callback(decrement_CB, this) ;
        if(value_ < 0) { value_ = 0 ; }
        if(value_ >= int(labels_.size())) { value_ = (GLenum)(labels_.size() - 1) ; }
        text_ = new TextLabel(x+450,y,labels_[value_]) ;
        add_child(up_) ;
        add_child(down_) ;
        add_child(text_) ;
        show() ;
    }

    void Spinbox::draw() {
        Container::draw() ;
    }

    void Spinbox::increment() {
        value_++ ;
        if(value_ >= labels_.size()) { value_ = 0 ; }
        text_->set_text(labels_[value_]) ;
    }

    void Spinbox::decrement() {
        if(int(value_) - 1 < 0) { 
            value_ = (GLenum)(labels_.size() - 1) ; 
        } else {
            value_-- ;
        }
        text_->set_text(labels_[value_]) ;
    }

    void Spinbox::increment_CB(void* spinbox) {
        static_cast<Spinbox*>(spinbox)->increment() ;
    }
    
    void Spinbox::decrement_CB(void* spinbox) {
        static_cast<Spinbox*>(spinbox)->decrement() ;
    }


//______________________________________________________________________________________________________

    void MessageBox::draw() {
        if(!visible()) { return ; }
        Panel::draw() ;
        glLineWidth(2) ;
        for(unsigned int i=0; i<message_.size(); i++) {
            printf_xy(x1_+100, y2_-200-i*150, (char*)message_[i].c_str()) ;
        }
    }

//______________________________________________________________________________________________________


    PropertyPage::PropertyPage(
        GLfloat x_in, GLfloat y_in, const std::string& caption
    ) : Panel(x_in,y_in-10,x_in+Width,y_in) {
        y_ = y2_ - 200 ;
        x_caption_ = x1_ + 100 ;
        x_widget_ = x1_ + 1300 ;
        caption_ = add_separator(caption) ;
        y1_ = y_ ;
    }

   
    TextLabel* PropertyPage::add_separator(const std::string& text) {
        TextLabel* w = new TextLabel(x1_ + 400, y_, text, 2.0f) ;
        add_child(w) ;
        y_ -= 250 ;
        y1_ = y_ ;
        return w ;
    }

    TextLabel* PropertyPage::add_string(const std::string& text) {
        TextLabel* w = new TextLabel(x1_ + 200, y_, text, 1.0f) ;
        add_child(w) ;
        y_ -= 150 ;
        y1_ = y_ ;
        return w ;
    }
   
    Slider* PropertyPage::add_slider(
        const std::string& caption, GLfloat& value, GLfloat vmin, GLfloat vmax 
    ) {
        add_child(new TextLabel(x_caption_, y_, caption)) ;
        Slider* w = new Slider(x_widget_, y_, x_widget_+800, y_+200, value) ;
        w->set_range(vmin, vmax) ;
        add_child(w) ;
        y_ -= 250 ;
        y1_ = y_ ;
        return w ;
    }

    Checkbox* PropertyPage::add_toggle(
        const std::string& caption, GLboolean& value
    ) {
        add_child(new TextLabel(x_caption_, y_, caption)) ;
        Checkbox* w = new Checkbox(x_widget_, y_, x_widget_+200, y_+200, value) ;
        add_child(w) ;
        y_ -= 250 ;
        y1_ = y_ ;
        return w ;
    }

    Spinbox* PropertyPage::add_enum(
        const std::string& caption, GLenum& value, const std::vector<std::string>& labels) {
        add_child(new TextLabel(x_caption_, y_, caption)) ;
        Spinbox* w = new Spinbox(x_widget_, y_, value, labels) ;
        add_child(w) ;
        y_ -= 250 ;
        y1_ = y_ ;
        return w ;
    }

//______________________________________________________________________________________________________

    ViewerProperties::ViewerProperties(GLfloat x_left, GLfloat y_top) : PropertyPage(
        x_left, y_top, "Viewer"
    ) {
        add_toggle("Rot. light", *glut_viewer_is_enabled_ptr(GLUT_VIEWER_ROTATE_LIGHT)) ;
        if(glut_viewer_is_enabled(GLUT_VIEWER_HDR)) {
            add_slider("Exposure", *glut_viewer_float_ptr(GLUT_VIEWER_HDR_EXPOSURE), 0.001, 3.0) ;
            add_slider("Gamma", *glut_viewer_float_ptr(GLUT_VIEWER_HDR_GAMMA), 0.2, 1.5) ;
            add_toggle("Vignette", *glut_viewer_is_enabled_ptr(GLUT_VIEWER_HDR_VIGNETTE)) ;
            add_slider("Blur amount", *glut_viewer_float_ptr(GLUT_VIEWER_HDR_BLUR_AMOUNT)) ;
            add_slider("Blur width", *glut_viewer_float_ptr(GLUT_VIEWER_HDR_BLUR_WIDTH), 1.0, 20.0) ;
            add_toggle("UnMsk.",  *glut_viewer_is_enabled_ptr(GLUT_VIEWER_HDR_UNSHARP_MASKING)) ;
            add_toggle("UnMsk.+", *glut_viewer_is_enabled_ptr(GLUT_VIEWER_HDR_POSITIVE_UNSHARP_MASKING)) ;
            add_slider("UnMsk. Gamm", *glut_viewer_float_ptr(GLUT_VIEWER_HDR_UNSHARP_MASKING_GAMMA), 0.2, 1.5) ;
        }

    }

    void ViewerProperties::draw() {
        if(glut_viewer_is_enabled(GLUT_VIEWER_IDLE_REDRAW)) {
            static char buff[256] ;
            sprintf(buff, " [%4d FPS]", glut_viewer_fps()) ;
            caption_->set_text("Viewer" + std::string(buff)) ;
        } else {
            caption_->set_text("Viewer") ;	   
        }
        PropertyPage::draw() ;
    }
   
    void ViewerProperties::apply() {
    }
   
   
//______________________________________________________________________________________________________   
   

    Image::Image(
        GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLint texture, GLint target
    ) : Widget(x1, y1, x2, y2), texture_(texture), texture_target_(target) {
    }

    void Image::draw() {
        if(texture_ == 0) { return ; }
        glEnable(texture_target_) ;
        glBindTexture(texture_target_, texture_) ;

        glBegin(GL_QUADS) ;

        glTexCoord2f(0.0, 0.0) ;
        glVertex2f(x1_, y1_) ;

        glTexCoord2f(1.0, 0.0) ;
        glVertex2f(x2_, y1_) ;

        glTexCoord2f(1.0, 1.0) ;
        glVertex2f(x2_, y2_) ;

        glTexCoord2f(0.0, 1.0) ;
        glVertex2f(x1_, y2_) ;

        glEnd() ;

        glDisable(texture_target_) ;
    }

//______________________________________________________________________________________________________   

}
