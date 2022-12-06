#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>

#include <learnopengl/model.h>
#include <learnopengl/mesh.h>
#include <learnopengl/light.h>
#include <learnopengl/camera.h>

class Scene {
 public:
   Scene();
   ~Scene();
   void ForwardScene();
   void UpdateLight();

   public:
   // Shader, Model, unsigned int Camera, AbstractLight, Mesh
   unsigned int skybox;
   std::vector<std::shared_ptr<Model>> vec_models;
   std::vector<std::shared_ptr<Camera>> vec_cams;
   // 灯光
   std::vector<std::shared_ptr<DirectionLight>> vec_dlLights;
   std::vector<std::shared_ptr<PointLight>> vec_ptLights;
   std::vector<std::shared_ptr<SpotLight>> vec_stLights;
   // Mesh
   std::vector<std::shared_ptr<Quad>> vec_quads;
   std::vector<std::shared_ptr<Sphere>> vec_spheres;
   std::vector<std::shared_ptr<Cube>> vec_cubes;
   std::vector<std::shared_ptr<Floor>> vec_floores;
};

#endif