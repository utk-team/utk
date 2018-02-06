#include "Texture.h"



Texture::Texture(GLuint target_)
{
  currUnit       = -1;
  target         = target_;
  height         = 0;
  width          = 0;
  depth          = 0;
  id             = 0;
}

bool Texture::finalizeLoading(const char* string)
{
  if(!id){
	  std::cerr<<"Failed to load texture <"<<">"<<std::endl ;
	  return false ;
  }
  return true;
}

bool Texture::activate(int unit)
{
  if(!id)
    return false;

  if(unit > -1){
    glActiveTextureARB(GL_TEXTURE0_ARB + unit);
  }

  glEnable(target);
  glBindTexture(target, id);
 
  currUnit = unit;
  return true;
}

void Texture::copyCurrentBuffer(int newTarget)
{
  if(!id)
    return;

  newTarget = (newTarget == -1) ? target : newTarget;
  activate();
  glCopyTexSubImage2D(newTarget, 0, 0, 0, 0, 0, width, height);
  deactivate();
}

bool Texture::deactivate()
{
  if(id > 0 )
  {
    if(currUnit > -1)
      glActiveTextureARB(GL_TEXTURE0_ARB + currUnit);

    glDisable(target);
    return true;
  }
  return false;
}

GLuint Texture::getID() const 
{ 
  return id;
}


void Texture::setTarget(GLuint target_)
{
  target = target_;
}

GLuint  Texture::getTarget() const { return target; }

GLuint Texture::getHeight() const { return height;  }
GLuint Texture::getWidth()  const { return width;   }
GLuint Texture::getDepth()  const { return depth;   } 

int Texture::getValidWrapMode(int clamp)
{
  return (clamp == GL_CLAMP          ) ? GL_CLAMP           :
         (clamp == GL_REPEAT         ) ? GL_REPEAT          :
         (clamp == GL_CLAMP_TO_EDGE  ) ? GL_CLAMP_TO_EDGE   :
         (clamp == GL_CLAMP_TO_BORDER) ? GL_CLAMP_TO_BORDER : GL_REPEAT;
}

int Texture::getValidMagFilter(int filter)
{
  return (filter == GL_NEAREST ) ? GL_NEAREST  : GL_LINEAR;
}

int Texture::getValidMinFilter(int filter)
{
  return (filter == GL_NEAREST                ) ? GL_NEAREST                :
         (filter == GL_LINEAR_MIPMAP_LINEAR   ) ? GL_LINEAR_MIPMAP_LINEAR   :
         (filter == GL_NEAREST_MIPMAP_LINEAR  ) ? GL_NEAREST_MIPMAP_LINEAR  :
         (filter == GL_LINEAR_MIPMAP_NEAREST  ) ? GL_LINEAR_MIPMAP_NEAREST  :
         (filter == GL_NEAREST_MIPMAP_NEAREST ) ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR;
}

void Texture::destroy()
{
  //TextureInfo *textureInfo = TexturesManager::getTextureInfo(id);

  //if(textureInfo)
  //  textureInfo->decreaseUserCount();
  //else
    glDeleteTextures(1, &id);
 
  //TexturesManager::flushUnusedTextures();

  height         = 0;
  width          = 0;
  depth          = 0;
  id             = 0;
}

Texture::~Texture()
{
  destroy();
}



bool Texture::createCubeShell(const char* name,
                              GLuint size     ,
                              GLuint inFormat , GLuint format,
                              GLuint clampS   , GLuint clampT, 
                              GLuint magFilter, GLuint minFilter)
{
  if(!name){
	  std::cerr<<"Cube Texture Shell must have a valid name"<<std::endl ;
	  return false ;
  }

  destroy();
  target = GL_TEXTURE_CUBE_MAP;

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, id);

  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, getValidMagFilter(magFilter));
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, getValidMinFilter(minFilter));
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S,     getValidWrapMode(clampS));
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T,     getValidWrapMode(clampT));

  std::cerr<<"Loading Cube Shell: size = "<<int(size)<<std::endl;

  for(int i = 0; i < 6; i++)
   glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + i,
                0, 
                inFormat,
                size, 
                size,
                0,
                format, 
                GL_UNSIGNED_BYTE, 
                NULL);

  this->height = size;
  this->width  = size;
  depth        = 1;

  return finalizeLoading(name);
}

