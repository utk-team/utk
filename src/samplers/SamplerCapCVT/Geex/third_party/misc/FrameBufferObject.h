#ifndef FRAME_BUFFER_OBJECT_H
#define FRAME_BUFFER_OBJECT_H

#include "Texture.h"
#include <misc/linkage.h>

class GEEX_API FrameBufferObject
{
  private:
    GLuint stencilBufferID,
           depthBufferID,
           frameBufferID,
           height,
           width;

    bool   sharedStencilBuffer,
           sharedDepthBuffer;

  public:
     FrameBufferObject();
    ~FrameBufferObject();

     bool initialize(Texture &texture, 
                     GLuint   sharedStencilBufferID = 0,
                     GLuint   sharedDepthBufferID   = 0,
                     bool     depthOnly             = false);
     void attachRenderTarget(Texture &texture, int index = 0, int newTarget = -1);
     void detachRenderTarget(Texture &texture, int index = 0, int newTarget = -1);

     static void deactivate();
     void   activate();

     GLuint getStencilBufferID() const;
     GLuint getDepthBufferID()   const;
     GLuint getFrameBufferID()   const;
     GLuint getHeight()          const;
     GLuint getWidth()           const;

     static  bool  checkStatus();
};
#endif
