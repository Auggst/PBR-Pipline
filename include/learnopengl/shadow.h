#ifndef SHODOW_H
#define SHODOW_H

#include <memory>

#include <learnopengl/shader_s.h>
#include <learnopengl/light.h>

enum SHADOW_TYPE{
    WITHOUT, SM, PCF, PCSS, CASCADE
};

class Shadow {
 public:
   Shadow();
   ~Shadow();
   virtual void Init();
   virtual void RenderSM(PointLight& light);
 public:
   bool isFFC;
   unsigned int* sm_tex;
   unsigned int FBO, RBO, width, height;
   float bias;
   SHADOW_TYPE type;
   std::shared_ptr<Shader> mpSM_SH;
};

#endif