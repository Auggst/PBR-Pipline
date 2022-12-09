#include <learnopengl/engine.h>

std::shared_ptr<Engine> Engine::moon;

static float lastX = 1320.0f / 2.0;
static float lastY = 720.0f / 2.0;
static bool firstMouse = true;

//时间设置
static float deltaTime = 0.0f;
static float lastFrame = 0.0f;

Engine::Engine() {
  this->width = 1320;
  this->height = 720;
  this->window_name = "Moon";
  this->window = nullptr;
  this->cam = nullptr;
}

Engine::~Engine() {
  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  //清除缓存
  glfwTerminate();

  this->assetManager.~Asset();
}

void Engine::CreatePBR() {
  this->pipeline = std::make_shared<PBR>();
  this->pipeline->Init();
  if (this->assetManager.um_scene.find("PBR") == this->assetManager.um_scene.end())
  {
    Scene tempSce = Scene();
    tempSce.PBRScene();
    this->assetManager.um_scene.emplace("PBR", tempSce);
  }
  Scene *pScene = &(this->assetManager.um_scene.find("PBR")->second);
  this->scene = std::shared_ptr<Scene>(pScene);
  pScene = nullptr;
}

void Engine::CreateForwardS() {
  this->pipeline = std::make_shared<ForwardShading>();
  this->pipeline->Init();
  Scene *pScene = &(this->assetManager.um_scene.find("Forward")->second);
  this->scene = std::shared_ptr<Scene>(pScene);
  pScene = nullptr;
}

void Engine::CreateDeferredS() {
  this->pipeline = std::make_shared<DeferredShading>();
  this->pipeline->Init();
  if (this->assetManager.um_scene.find("Defer") == this->assetManager.um_scene.end())
  {
    Scene tempSce = Scene();
    tempSce.DeferredScene();
    this->assetManager.um_scene.emplace("Defer", tempSce);
  }
  Scene *pScene = &(this->assetManager.um_scene.find("Defer")->second);
  this->scene = std::shared_ptr<Scene>(pScene);
  pScene = nullptr;
}

void Engine::InitOpenGL()
{
  //初始化glfw，确保glfw版本和core模式
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  //创建窗口
  this->window = glfwCreateWindow(this->width, this->height, this->window_name.c_str(), NULL, NULL);
  if (this->window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return;
  }
  glfwMakeContextCurrent(this->window);
  glfwSetFramebufferSizeCallback(this->window, framebuffer_size_callback);
  glfwSetCursorPosCallback(this->window, mouse_callback);
  glfwSetScrollCallback(this->window, scroll_callback);
  glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

  //利用glad函数加载当前系统的OpenGL函数指针
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return;
  }

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
}

void Engine::InitGUI() {
  // IMGUI初始化
  ImGui::CreateContext();                     // Setup Dear ImGui context
  ImGui::StyleColorsDark();                   // Setup Dear ImGui style
  ImGuiIO &io = ImGui::GetIO();               // 设置中文字体

  io.Fonts->AddFontFromFileTTF("c:/windows/fonts/simhei.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
  // io.Fonts->AddFontFromFileTTF("c:/windows/fonts/STXIHEI.TTF", 18.0f, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
  ImGui_ImplGlfw_InitForOpenGL(this->window, true); // Setup Platform/Renderer backends
  ImGui_ImplOpenGL3_Init("#version 430 core");
}

void Engine::InitAsset() {
  this->assetManager.Init();
  Camera *temp_Cam = &(this->assetManager.um_cameras.find("Main")->second);
  this->cam = std::shared_ptr<Camera>(temp_Cam);
  temp_Cam = nullptr;
}

void Engine::Init() {
  this->InitOpenGL();
  this->InitGUI();
  this->assetManager = std::move(Asset());
  this->InitAsset();
}

void Engine::Update() {
  if (this->pipeline == nullptr) {
    std::cout << "管线未初始化！" << std::endl; 
    return;
  }

  int scrWidth, scrHeight;
  glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
  glViewport(0, 0, scrWidth, scrHeight);

  while (!glfwWindowShouldClose(this->window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    // input事件
    processInput(this->window);

    // 处理光照
    this->scene->UpdateLight();

    // Rendering
    this->pipeline->RenderScene(*(this->scene));
    RenderGUI();

    //检查及调用事件和交换内容
    glfwSwapBuffers(this->window);
    glfwPollEvents();
  }
}

void Engine::RenderGUI() {
  /* Swap front and back buffers */
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  {
    ImGui::Begin(u8"Moon引擎全局设置");
    ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(300, 720), ImGuiCond_Always);

    //主窗口
    const static ImVec4 MAINCOLOR = ImVec4(0.0f, 0.6f, 1.0f, 1.0f);
    ImGui::TextColored(MAINCOLOR, u8"用于调整全局设置");
    ImGui::TextColored(MAINCOLOR, u8"当前管线:");
    int pipeType = this->pipeline->type;
    ImGui::RadioButton("Forward", &pipeType, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Defer", &pipeType, 1);
    ImGui::SameLine();
    ImGui::RadioButton("PBR", &pipeType, 2);
    ImGui::SameLine();
    ImGui::RadioButton("NPR", &pipeType, 4);

    RenderSceneGUI();
    ImGui::End();

    if (this->pipeline == nullptr) {}
    else {
      PipelineGUI(this->pipeline->type);
    } 
  }

  ImGui::ShowDemoWindow();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Engine::RenderSceneGUI() {
  /* 黄色用于场景参数，(1.0f, 1.0f, 0.0f, 1.0f) */
  const static ImVec4 color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
  // 相机参数调整
  ImGui::TextColored(color, u8"相机参数:");
  ImGui::InputFloat3(u8"位置", &(moon->cam->Position[0]));
  ImGui::InputFloat(u8"相机移动速度: ", &(moon->cam->MovementSpeed));
  ImGui::Text(u8"帧率 %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

  // 模型参数调整
  if (this->scene->vec_models.size() > 0) {
    ImGui::TextColored(color, u8"模型参数:");
    for (size_t i = 0; i < this->scene->vec_models.size(); i++) {
      std::string modelID = u8"模型" + std::to_string(i) + ":";
      ImGui::TextColored(color, modelID.c_str());
      // ImGui::InputFloat3(u8"位置", &(this->scene->vec_models[i]));
    }
  }

  /* 灯光参数调整 */
  // 方向光
  if (this->scene->vec_dlLights.size() > 0) {
    ImGui::TextColored(color, u8"方向光参数:");
    for (size_t i = 0; i < this->scene->vec_dlLights.size(); i++)
    {
      std::string modelID = u8"方向光" + std::to_string(i) + ":";
      ImGui::TextColored(color, modelID.c_str());
      ImGui::InputFloat3(u8"方向: ", &(this->scene->vec_dlLights[i]->direction[0]));
      ImGui::ColorEdit4(u8"漫反射颜色: ", &(this->scene->vec_dlLights[i]->diffuse[0]));
      ImGui::ColorEdit4(u8"镜面反射颜色0: ", &(this->scene->vec_dlLights[i]->specular[0]));
    }
  }

  // 点光源
  if (this->scene->vec_ptLights.size() > 0) {
    ImGui::TextColored(color, u8"点光源参数:");
    for (size_t i = 0; i < this->scene->vec_ptLights.size(); i++)
    {
      std::string modelID = u8"点光源" + std::to_string(i) + ":";
      ImGui::TextColored(color, modelID.c_str());
      ImGui::InputFloat3(u8"位置: ", &(this->scene->vec_ptLights[i]->position[0]));
      ImGui::ColorEdit4(u8"漫反射颜色: ", &(this->scene->vec_ptLights[i]->diffuse[0]));
      ImGui::ColorEdit4(u8"镜面反射颜色1: ", &(this->scene->vec_ptLights[i]->specular[0]));
    }
  }

  // 聚光灯
  if (this->scene->vec_stLights.size() > 0) {
    ImGui::TextColored(color, u8"聚光灯参数:");
    for (size_t i = 0; i < this->scene->vec_stLights.size(); i++)
    {
      std::string modelID = u8"聚光灯" + std::to_string(i) + ":";
      ImGui::TextColored(color, modelID.c_str());
      ImGui::ColorEdit4(u8"漫反射颜色: ", &(this->scene->vec_stLights[i]->diffuse[0]));
      ImGui::ColorEdit4(u8"镜面反射颜色2: ", &(this->scene->vec_stLights[i]->specular[0]));
    }
  }
}

void Engine::PipelineGUI(const Pipeline_TYPE& type) {
  if (type == Pipeline_TYPE::FORWARDSHADING) {
    {
      ImGui::Begin(u8"渲染窗口");
      ImGui::SetWindowPos(ImVec2(300, 0), ImGuiCond_Always);
      ImGui::SetWindowSize(ImVec2(720, 720), ImGuiCond_Always);
      ImGui::Image((void *)(intptr_t)(this->pipeline->res_tex), ImVec2(720, 720), ImVec2(0, 1), ImVec2(1, 0));
      ImGui::End();
    }
    {
      ImGui::Begin(u8"延迟渲染管线参数设置");
      ImGui::SetWindowPos(ImVec2(1020, 0), ImGuiCond_Always);
      ImGui::SetWindowSize(ImVec2(300, 720), ImGuiCond_Always);
      //主窗口
      ImGui::Text(u8"用于调整延迟渲染管线对应参数");
      ImGui::End();
    }
  } else if (type == Pipeline_TYPE::DEFERREDSHADING) {
    {
      ImGui::Begin(u8"渲染窗口");
      ImGui::SetWindowPos(ImVec2(300, 0), ImGuiCond_Always);
      ImGui::SetWindowSize(ImVec2(720, 720), ImGuiCond_Always);
      ImGui::Image((void *)(intptr_t)(this->pipeline->res_tex), ImVec2(720, 720), ImVec2(0, 1), ImVec2(1, 0));
      ImGui::End();
    }
    {
      ImGui::Begin(u8"前向渲染管线参数设置");
      ImGui::SetWindowPos(ImVec2(1020, 0), ImGuiCond_Always);
      ImGui::SetWindowSize(ImVec2(300, 720), ImGuiCond_Always);
      //主窗口
      ImGui::Text(u8"用于调整前向渲染管线对应参数");
      // 编辑颜色 (stored as ~4 floats)
      // ImGui::ColorEdit4("Color", my_color);
      ImGui::End();
    }
  } else if (type == Pipeline_TYPE::PBRSHADING) {
    {
      ImGui::Begin(u8"渲染窗口");
      ImGui::SetWindowPos(ImVec2(300, 0), ImGuiCond_Always);
      ImGui::SetWindowSize(ImVec2(720, 720), ImGuiCond_Always);
      ImGui::Image((void *)(intptr_t)(this->pipeline->res_tex), ImVec2(720, 720), ImVec2(0, 1), ImVec2(1, 0));
      ImGui::End();
    }
    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
      ImGui::Begin(u8"PBR渲染管线参数设置");
      ImGui::SetWindowPos(ImVec2(1020, 0), ImGuiCond_Always);
      ImGui::SetWindowSize(ImVec2(300, 720), ImGuiCond_Always);
      //主窗口
      ImGui::Text(u8"用于调整PBR渲染管线对应参数");
      // if (this->spheres != nullptr)
      // {
      //   ImGui::SliderFloat("mental", &(this->spheres->mental), 0.0f, 1.0f);
      //   ImGui::SliderFloat("rough", &(this->spheres->rough), 0.0f, 1.0f);
      // }
      ImGui::End();
    }
  }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  // camer controling
  std::shared_ptr<Engine> moon = Engine::getInstance();
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    moon->cam->ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    moon->cam->ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    moon->cam->ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    moon->cam->ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse)
  {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

  lastX = xpos;
  lastY = ypos;

  std::shared_ptr<Engine> moon = Engine::getInstance();
  moon->cam->ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
  std::shared_ptr<Engine> moon = Engine::getInstance();
  moon->cam->ProcessMouseScroll(static_cast<float>(yoffset));
}
