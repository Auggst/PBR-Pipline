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

class asset{
 public:
   asset();
   ~asset();
 public:
   std::unordered_map<std::string, Shader> um_shaders;
   std::unordered_map<std::string, Model> um_models;
   std::unordered_map<std::string, DirectionLight> um_dlLights;
   std::unordered_map<std::string, PointLight> um_ptLights;
   std::unordered_map<std::string, SpotLight> um_stLights;
   std::unordered_map<std::string, std::vector<std::string>> um_skyboxes;
   std::unordered_map<std::string, Camera> um_cameras;
};

#endif