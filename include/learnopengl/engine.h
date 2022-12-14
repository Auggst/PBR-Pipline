#ifndef ENGINE_H
#define ENGINE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <memory>
#include <vector>
#include <string>
#include <iostream>

#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <learnopengl/pipeline.h>
#include <learnopengl/asset.h>

class Engine {
 public:
   Engine();
   ~Engine();

   static std::shared_ptr<Engine> getInstance() {
     if (moon == nullptr) moon = std::make_shared<Engine>();
     return moon;
   }
   void CreatePBR();
   void CreateForwardS();
   void CreateDeferredS();
   void InitOpenGL();
   void InitGUI();
   void InitAsset();
   void Init();
   void Update();
   void RenderGUI();
   void RenderSceneGUI();
   void PipelineGUI(const Pipeline_TYPE& type);
 public:
   int width, height;
   std::string window_name;
   GLFWwindow *window;
   std::shared_ptr<Pipeline> pipeline;
   Asset assetManager;
   std::shared_ptr<Scene> scene;
   std::shared_ptr<Camera> cam;
 private:
   static std::shared_ptr<Engine> moon;
};

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

#endif
