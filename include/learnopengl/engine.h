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
   void InitOpenGL();
   void InitGUI();
   void Init();
   void Update();
 public:
   int width, height;
   std::string window_name;
   GLFWwindow *window;
   std::shared_ptr<Camera> cam;
   std::shared_ptr<Pipeline> pipeline;
   std::vector<std::shared_ptr<Renderable>> models;
 private:
   static std::shared_ptr<Engine> moon;
};

static float lastX = 1280.0f / 2.0;
static float lastY = 720.0f / 2.0;
static bool firstMouse = true;

//时间设置
static float deltaTime = 0.0f;
static float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

#endif
