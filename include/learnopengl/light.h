#ifndef LIGHT_H
#define LIGHT_H

#include <memory>

#include <learnopengl/myutils.h>
#include <learnopengl/shader_s.h>

enum LIGHT_TYPE {
  DIRECTION, POINTLIGHT, SPOTLIGHT
};

class AbstractLight {
 public:
   AbstractLight(LIGHT_TYPE _type) {this->type = _type;}
   virtual ~AbstractLight();
   virtual void SendToShader(std::shared_ptr<Shader> sh, int index) = 0;
 public:
   LIGHT_TYPE type;
};

class DirectionLight : public AbstractLight {
 public:
   DirectionLight();
   ~DirectionLight();
   void SendToShader(std::shared_ptr<Shader> sh, int index);
 public:
   glm::vec3 direction;
   glm::vec3 ambient;
   glm::vec3 diffuse;
   glm::vec3 specular;
};

class PointLight : public AbstractLight {
 public:
   PointLight();
   ~PointLight();
   void SendToShader(std::shared_ptr<Shader> sh, int index);
 public:
   glm::vec3 position;
   glm::vec3 ambient;
   glm::vec3 diffuse;
   glm::vec3 specular;

   float kc;
   float kl;
   float kq;
};

class SpotLight : public AbstractLight {
 public:
   SpotLight();
   ~SpotLight();
   void SendToShader(std::shared_ptr<Shader> sh, int index);
 public:
   glm::vec3 position;
   glm::vec3 direction;
   glm::vec3 ambient;
   glm::vec3 diffuse;
   glm::vec3 specular;
   float cutOff;
   float outerCutOff;
   float kc, kl, kq;
};

#endif