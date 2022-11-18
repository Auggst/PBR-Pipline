#include <learnopengl/engine.h>

std::shared_ptr<Engine> Engine::moon;

Engine::Engine() {
  this->width = 1280;
  this->height = 720;
  this->window_name = "Moon";
  this->window = nullptr;
  this->cam = std::make_shared<Camera>();
  this->models.clear();
}

Engine::~Engine() {
  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  //清除缓存
  glfwTerminate();
}


void Engine::CreatePBR() {
  this->pipeline = std::make_shared<PBR>();
  this->pipeline->init();
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
  glEnable(GL_LEQUAL);
}

void Engine::InitGUI() {
  // IMGUI初始化
  ImGui::CreateContext();                     // Setup Dear ImGui context
  ImGui::StyleColorsDark();                   // Setup Dear ImGui style
  ImGuiIO &io = ImGui::GetIO();               // 设置中文字体
  
  io.Fonts->AddFontFromFileTTF("c:/windows/fonts/simhei.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
  // io.Fonts->AddFontFromFileTTF("c:/windows/fonts/STXIHEI.TTF", 18.0f, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
  ImGui_ImplGlfw_InitForOpenGL(this->window, true); // Setup Platform/Renderer backends
  ImGui_ImplOpenGL3_Init("#version 430");
}

void Engine::Init() {
  this->InitOpenGL();
  this->InitGUI();
}

void Engine::Update() {
  if (this->pipeline == nullptr) {
    std::cout << "管线未初始化！" << std::endl; 
    return;
  }

  int scrWidth, scrHeight;
  glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
  glViewport(0, 0, scrWidth, scrHeight);
  while(!glfwWindowShouldClose(this->window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    // input事件
    processInput(this->window);

    // Rendering
    this->pipeline->render();
    this->pipeline->renderUI();

    //检查及调用事件和交换内容
    glfwSwapBuffers(this->window);
    glfwPollEvents();
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
