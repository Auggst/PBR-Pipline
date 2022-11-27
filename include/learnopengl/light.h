#ifndef LIGHT_H
#define LIGHT_H

#include <memory>

#include <learnopengl/myutils.h>
#include <learnopengl/shader_s.h>


class AbstractLight {
   virtual void SendToShader(std::shared_ptr<Shader> sh, int index) = 0;
};

class DirectionLight : public AbstractLight {
 public:
   DirectionLight();
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