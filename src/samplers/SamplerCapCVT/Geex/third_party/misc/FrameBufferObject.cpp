#include "FrameBufferObject.h"

template <class T>
inline T clamp(T x, T min, T max)
{ 
  return (x < min) ? min : (x > max) ? max : x;
}

FrameBufferObject::FrameBufferObject()
{
  sharedStencilBuffer = false;
  sharedDepthBuffer   = false;
  stencilBufferID     =     0;
  frameBufferID       =     0;
  depthBufferID       =     0;
  height              =     0;
  width               =     0;
}

bool  FrameBufferObject::initialize(Texture &texture, 
                                    GLuint   sharedStencilBufferID,
                                    GLuint   sharedDepthBufferID,
                                    bool     depthOnly)
{
  if(frameBufferID){
	std::cerr<<"FrameBufferObject already initialized!"<<std::endl ;
    return false ; 
  }
 
  if(texture.getWidth() <= 0 || texture.getHeight() <= 0){
	  std::cerr<<"Width and Height of FBO must be positive, non-zero"<<std::endl;
    return false ;
  }

  if(texture.getTarget() != GL_TEXTURE_2D           &&
     texture.getTarget() != GL_TEXTURE_CUBE_MAP_ARB &&
	 texture.getTarget() != GL_TEXTURE_RECTANGLE_ARB){
     std::cerr<<"Current FBO implementation only supports 2D/RECT/CUBE textures"<<std::endl;
	 return false ;
  }
  
  if(!texture.getID()){
	  std::cerr<<"FBO need a valid Texture ID"<<std::endl ;
	  return false ;
  }

  sharedStencilBuffer = (sharedStencilBufferID!= 0);
  sharedDepthBuffer   = (sharedDepthBufferID  != 0);

  height              = texture.getHeight();
  width               = texture.getWidth();

  glGenFramebuffersEXT(1, &frameBufferID);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBufferID);

  if(!sharedDepthBufferID && !depthOnly)
  {
    glGenRenderbuffersEXT(1, &depthBufferID);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBufferID);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                                 GL_RENDERBUFFER_EXT, depthBufferID);
  }
  
  if(sharedDepthBufferID)
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                                 GL_RENDERBUFFER_EXT, sharedDepthBufferID);

  if(!sharedStencilBufferID)
  {
    glGenRenderbuffersEXT(1, &stencilBufferID);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, stencilBufferID);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_STENCIL_INDEX, width, height);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT,
                                 GL_RENDERBUFFER_EXT, stencilBufferID);
  }
  else
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT,
                                 GL_RENDERBUFFER_EXT, sharedStencilBufferID);


  if((texture.getTarget() == GL_TEXTURE_RECTANGLE_ARB) ||
     (texture.getTarget() == GL_TEXTURE_2D))
  {
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                              depthOnly ? GL_DEPTH_ATTACHMENT_EXT : GL_COLOR_ATTACHMENT0_EXT,
                              texture.getTarget(), 
                              texture.getID(),
                              0);
    if(depthOnly)
    {
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE); 
    }
  }
  else
  {
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                              GL_COLOR_ATTACHMENT0_EXT,
                              GL_TEXTURE_CUBE_MAP_POSITIVE_X, 
                              texture.getID(),
                              0);

  } 
  bool result = FrameBufferObject::checkStatus();
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  return result;
}

/***************************************************************************************/
/*                                                                                     */
/*This function will report the status of the currently bound FBO                      */
/*                                                                                     */
/***************************************************************************************/
bool FrameBufferObject::checkStatus()                            
{     

  GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);  

  //Our FBO is perfect, return true
  if(status == GL_FRAMEBUFFER_COMPLETE_EXT)
    return true;

  switch(status) 
  {                                          
	case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT: 
	{
	  std::cerr<<"FBO has one or several image attachments with different internal formats"<<std::endl ;
	  return false ;
	}
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: 
	{
		std::cerr<<"FBO has one or several image attachments with different dimensions"<<std::endl ;
		return false ;
	}
//    case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT: 
//	{
//		std::cerr<<"FBO has a duplicate image attachment"<<std::endl ;
//		return false ;
//	}
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT: 
	{
		std::cerr<<"FBO missing an image attachment"<<std::endl ;
		return false ;
	}
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:                   
	{
		std::cerr<<"FBO format unsupported"<<std::endl ;
		return false ;
	}
  }
  std::cerr<<"Unknown FBO error"<<std::endl ;
  return true ;
  return false ;                              
}

void FrameBufferObject::attachRenderTarget(Texture &texture, int index, int newTarget)
{
  newTarget = (newTarget == -1) ? texture.getTarget() : newTarget;
  index     = clamp(index, 0, 15);

  if(frameBufferID)
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + index,
                              newTarget, texture.getID(), 0);
  else
	  std::cerr<<"Invalid FrameBufferObject index"<<std::endl ;
}

void FrameBufferObject::detachRenderTarget(Texture &texture, int index, int newTarget)
{
  newTarget = (newTarget == -1) ? texture.getTarget() : newTarget;
  index     = clamp(index, 0, 15);

  if(frameBufferID)
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + index,
                              newTarget, 0, 0);
  else
	  std::cerr<<"Invalid FrameBufferObject index"<<std::endl ;
}

void FrameBufferObject::activate()
{
  if(frameBufferID)
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBufferID);
  else
	  std::cerr<<"Invalid FrameBufferObject index"<<std::endl ;
}

void FrameBufferObject::deactivate()
{
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

GLuint FrameBufferObject::getStencilBufferID() const { return stencilBufferID; }
GLuint FrameBufferObject::getDepthBufferID()   const { return depthBufferID;   }
GLuint FrameBufferObject::getFrameBufferID()   const { return frameBufferID;   }
GLuint FrameBufferObject::getHeight()          const { return height;          }
GLuint FrameBufferObject::getWidth()           const { return width;           }

FrameBufferObject::~FrameBufferObject()
{
  if(stencilBufferID && !sharedStencilBuffer)
    glDeleteRenderbuffersEXT(1, &stencilBufferID);

  if(depthBufferID && !sharedDepthBuffer)
    glDeleteRenderbuffersEXT(1, &depthBufferID);

  if(frameBufferID)
    glDeleteFramebuffersEXT(1, &frameBufferID);

  sharedStencilBuffer = false;
  sharedDepthBuffer   = false;
  stencilBufferID     =     0;
  frameBufferID       =     0;
  depthBufferID       =     0;
  height              =     0;
  width               =     0;
}
