#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>

#include <learnopengl/model.h>
#include <learnopengl/mesh.h>
#include <learnopengl/light.h>

class Scene {
 public:
   Scene();
   ~Scene();
   void InitScene();
 public:
   std::vector<std::shared_ptr<Model>> models;
   std::vector<std::shared_ptr<Renderable>> quads;
   std::vector<std::shared_ptr<AbstractLight>> lights;
};

#endif