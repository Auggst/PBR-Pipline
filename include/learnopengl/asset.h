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
   /*重构
   *-------------------------------------
   *全局内存管理器，本身属于高层，全部抽象
   * Shader, Model, unsigned int Camera, AbstractLight, Mesh
   * 数据结构: 无序map
   * 接口: 每个类型提供一个查询接口，如果找得到就返回该类型，否则创建目标并加入内存，然后再返回。
   */
   std::unordered_map<std::string, Shader> um_shaders;
   std::unordered_map<std::string, Model> um_models;
   std::unordered_map<std::string, unsigned int> um_textures;
   std::unordered_map<std::string, Camera>
   um_cameras;
   std::unordered_map<std::string, AbstractLight*> um_lights;
   std::unordered_map<std::string, Renderable*> um_meshes;
};

#endif