#ifndef ASSET_H
#define ASSET_H

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

#include <learnopengl/camera.h>
#include <learnopengl/mesh.h>
#include <learnopengl/model.h>
#include <learnopengl/light.h>
#include <learnopengl/shader_s.h>

class Asset{
 public:
   Asset();
   ~Asset();
   void Init();
 public:
   std::unordered_map<std::string, Shader> um_shaders;
   std::unordered_map<std::string, Model> um_models;
   std::unordered_map<std::string, unsigned int> um_skyboxes;
   std::unordered_map<std::string, unsigned int> um_tex;
   std::unordered_map<std::string, Camera> um_cameras;
   std::vector<DirectionLight> vec_DL;
   std::vector<PointLight> vec_PL;
   std::vector<SpotLight> vec_SL;
   std::vector<Quad> vec_Quad;
   std::vector<Cube> vec_Cube;
   std::vector<Sphere> vec_Sphere;
};

#endif