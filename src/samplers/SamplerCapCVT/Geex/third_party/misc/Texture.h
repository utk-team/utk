#ifndef TEXTURE_H
#define TEXTURE_H

#include <GLsdk/gl_stuff.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <misc/linkage.h>

using namespace std;

class GEEX_API Texture {
  public:
    Texture(GLuint target = GL_TEXTURE_2D);
   ~Texture();

    bool createCubeShell(const char* name,
                         GLuint size      = 128,
                         GLuint inFormat  = GL_RGBA8,
                         GLuint format    = GL_RGBA, 
                         GLuint clampS    = GL_CLAMP_TO_EDGE, 
                         GLuint clampT    = GL_CLAMP_TO_EDGE, 
                         GLuint magFilter = GL_LINEAR,
                         GLuint minFilter = GL_LINEAR);

    void copyCurrentBuffer(int newTarget = -1);

    bool activate(int unit = -1);
    bool deactivate();

    void   setID(GLuint textureID);
    GLuint  getID() const;
    void   setTarget(GLuint target);

    GLuint  getTarget() const;
    GLuint  getHeight() const;
    GLuint  getWidth()  const;
    GLuint  getDepth()  const;

    void    destroy();

  protected:
    GLuint                 height,
                           width,
                           depth;
  private:

  
    GLuint                 target,
                           id;
    int                    currUnit;

    bool finalizeLoading(const char* string);
    bool checkForRepeat (const char* string);
 
    //int getXMLMagFilter(const String &value);
    //int getXMLMinFilter(const String &value);
    //int getXMLWrapMode (const String &value);
    //int getXMLType     (const String &value);

    int getValidWrapMode (int clamp);
    int getValidMagFilter(int filter);
    int getValidMinFilter(int filter);
};
#endif
