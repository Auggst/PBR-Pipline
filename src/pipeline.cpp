#include <learnopengl/pipeline.h>

#include <string>
#include <iostream>
#include <algorithm>

#include <learnopengl/myutils.h>
#include <learnopengl/engine.h>

/* 前向着色 */
// -----------------------------------------------------
ForwardShading::ForwardShading() : Pipeline(Pipeline_TYPE::FORWARDSHADING)
{
    this->mpModel_SH = nullptr;
    this->mpLight_SH = nullptr;
    this->mpSkybox_SH = nullptr;
    for (int i = 0; i < 4; i++) {
        my_color[i] = 0.0f;
    }
}

void ForwardShading::Init() {
    /* 初始化FBO和RT */
    my_color[3] = 1.0f;
    InitFBO(this->fbo, this->rbo, this->res_tex, 720, 720);

    std::shared_ptr<Engine> moon = Engine::getInstance();

    /* 创建灯光和模型 */
    // 灯光加载
    AbstractLight *tempAL = moon->assetManager.um_lights.find("DirectionLight1")->second;
    this->directionLight = std::dynamic_pointer_cast<DirectionLight>(std::shared_ptr<AbstractLight>(tempAL));
    tempAL = moon->assetManager.um_lights.find("PointLight1")->second;
    this->pointLight = std::dynamic_pointer_cast<PointLight>(std::shared_ptr<AbstractLight>(tempAL));
    tempAL = moon->assetManager.um_lights.find("SpotLight1")->second;
    this->spotLight = std::dynamic_pointer_cast<SpotLight>(std::shared_ptr<AbstractLight>(tempAL));
    tempAL = nullptr;

    // 模型加载
    Renderable *tempRender = moon->assetManager.um_meshes.find("Cube")->second;
    this->cube = std::dynamic_pointer_cast<Cube>(std::shared_ptr<Renderable>(tempRender));

    tempRender = moon->assetManager.um_meshes.find("Floor")->second;
    this->floor = std::dynamic_pointer_cast<Floor>(std::shared_ptr<Renderable>(tempRender));
    tempRender = nullptr;

    Model *nanosuit = &(moon->assetManager.um_models.find("Nanosuit")->second);
    this->models = std::shared_ptr<Model>(nanosuit);
    nanosuit = nullptr;

    // 天空盒加载
    this->env_cubemap = moon->assetManager.um_textures.find("BlueSky")->second;

    /* 着色器加载 */
    if (moon->assetManager.um_shaders.find("Floor") == moon->assetManager.um_shaders.end())
    {
        std::string vsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/Forward/floor.vs";
        std::string fsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/Forward/floor.fs";
        Shader temp_Model(vsPath.c_str(), fsPath.c_str());
        moon->assetManager.um_shaders.emplace("Floor", temp_Model);
    }
    Shader *temp_SH = &(moon->assetManager.um_shaders.find("Floor")->second);
    this->mpFloor_SH = std::shared_ptr<Shader>(temp_SH);
    temp_SH = nullptr;
    this->mpFloor_SH->use();
    this->mpFloor_SH->setInt("diffuseMap", 0);
    this->mpFloor_SH->setInt("normalMap", 1);
    this->mpFloor_SH->setBool("normalMapping", true);

    if (moon->assetManager.um_shaders.find("Model") == moon->assetManager.um_shaders.end())
    {
        std::string vsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/Forward/model.vs";
        std::string fsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/Forward/model.fs";
        Shader temp_Model(vsPath.c_str(), fsPath.c_str());
        moon->assetManager.um_shaders.emplace("Model", temp_Model);
    }
    temp_SH = &(moon->assetManager.um_shaders.find("Model")->second);
    this->mpModel_SH = std::shared_ptr<Shader>(temp_SH);
    temp_SH = nullptr;
    this->mpModel_SH->use();
    this->mpModel_SH->setInt("material.shininess", 64.0);

    if (moon->assetManager.um_shaders.find("LightModelTex") == moon->assetManager.um_shaders.end())
    {
        std::string vsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/Forward/light.vs";
        std::string fsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/Forward/light.fs";
        Shader temp_LM(vsPath.c_str(), fsPath.c_str());
        moon->assetManager.um_shaders.emplace("LightModelTex", temp_LM);
    }
    temp_SH = &(moon->assetManager.um_shaders.find("LightModelTex")->second);
    this->mpLight_SH = std::shared_ptr<Shader>(temp_SH);
    temp_SH = nullptr;
    this->mpLight_SH->use();
    this->mpLight_SH->setInt("diffuse", 0);

    // 加载贴图
    if (moon->assetManager.um_textures.find("Container") == moon->assetManager.um_textures.end())
    {
        std::string texPath = "D:/C++Pro/vscode/LearnOpenGL/texture/container2.png";
        moon->assetManager.um_textures.emplace(std::make_pair(std::string("Container"), loadTexture(texPath.c_str())));
    }
    this->light_tex = moon->assetManager.um_textures.find("Container")->second;

    if (moon->assetManager.um_shaders.find("SkyBox") == moon->assetManager.um_shaders.end())
    {
        std::string vsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/Skybox/Skybox.vs";
        std::string fsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/Skybox/Skybox.fs";
        Shader temp_Skybox(vsPath.c_str(), fsPath.c_str());
        moon->assetManager.um_shaders.emplace("SkyBox", temp_Skybox);
    }
    temp_SH = &(moon->assetManager.um_shaders.find("SkyBox")->second);
    this->mpSkybox_SH = std::shared_ptr<Shader>(temp_SH);
    temp_SH = nullptr;
    this->mpSkybox_SH->use();
    this->mpSkybox_SH->setInt("environmentMap", 0);
}

void ForwardShading::Render() {

    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->res_tex, 0);

    // Rendering
    glClearColor(my_color[0], my_color[1], my_color[2], my_color[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    std::shared_ptr<Engine> moon = Engine::getInstance();

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = moon->cam->GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(moon->cam->Zoom), (float)moon->width / (float)moon->height, 0.1f, 100.0f);

    // 光照模型渲染
    this->mpLight_SH->use();
    this->mpLight_SH->setMat4("view", view);
    this->mpLight_SH->setMat4("projection", projection);
    const float pi = 3.1415926;
    for (int i = 0; i < 1; i++) {
        model = glm::mat4(0.5);
        // float coff_pi = pi * i;
        glm::vec3 newPos = glm::vec3(0.0f, 15.0f, 0.0f);
        model = glm::translate(model, newPos);
        this->mpLight_SH->setMat4("model", model);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->light_tex);
        this->cube->Draw();
    }

    // 模型渲染
    this->mpModel_SH->use();
    this->mpModel_SH->setMat4("view", view);
    this->mpModel_SH->setMat4("projection", projection);
    this->mpModel_SH->setVec3("viewPos", moon->cam->Position);

    // 光照参数
    float timeVal = glfwGetTime();
    float greenVal = (sinf(timeVal) / 2) + 0.5f;    // 约束到[0, 1]
    float redVal = (cosf(timeVal) / 2) + 0.5f;
    float blueVal = (cosf(timeVal * pi) / 2) + 0.5f;
    glm::vec3 am(0.05f, 0.01f, 0.01f);
    glm::vec3 diff(redVal, greenVal, blueVal);
    glm::vec3 spec(0.5f, 0.5f, 0.5f);

    for (int i = 0; i < 4; i++) {
        model = glm::mat4(1.0);
        float coff_pi = pi * i;
        glm::vec3 newPos = glm::vec3(-2.0f * cosf(coff_pi), 4.0f * cosf(std::clamp(coff_pi - pi, 0.0f, pi)), 10.0f);

        /* 方向光加载 */
        directionLight->direction = glm::vec3(0.0f) - newPos;
        directionLight->diffuse = diff;
        directionLight->SendToShader(this->mpModel_SH, i);
        directionLight->SendToShader(this->mpFloor_SH, i);

        /* 点光源加载 */
        pointLight->position = glm::vec3(0.0f, 15.0f, 0.0f);
        pointLight->diffuse = diff;
        pointLight->SendToShader(this->mpModel_SH, i);
        pointLight->SendToShader(this->mpFloor_SH, i);
    }

    /* 聚光灯加载 */
    spotLight->position = moon->cam->Position;
    spotLight->direction = moon->cam->Front;
    spotLight->ambient = glm::vec3(0.001f);
    spotLight->diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    spotLight->specular = glm::vec3(1.0f, 1.0f, 1.0f);
    spotLight->kc = 1.0f;
    spotLight->kl = 0.09f;
    spotLight->kq = 0.0032f;
    spotLight->SendToShader(mpModel_SH, 0);

    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -10.0f));
    for (int i = 0; i < 2; i++) {
        this->mpModel_SH->setMat4("model", glm::scale(model, glm::vec3(0.5f)));
        this->models->Draw(*(this->mpModel_SH));
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0f));
    }

    // 地板渲染
    model = glm::mat4(1.0);
    this->mpFloor_SH->use();
    this->mpFloor_SH->setMat4("view", view);
    this->mpFloor_SH->setMat4("projection", projection);
    this->mpFloor_SH->setMat4("model", model);
    this->mpFloor_SH->setVec3("viewPos", moon->cam->Position);
    spotLight->SendToShader(mpFloor_SH, 0);
    this->floor->Draw();

    // 天空盒渲染
    glDepthFunc(GL_LEQUAL);
    this->mpSkybox_SH->use();
    this->mpSkybox_SH->setMat4("view", view);
    this->mpSkybox_SH->setMat4("projection", projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->env_cubemap);
    this->cube->Draw();
    glDepthFunc(GL_LESS);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ForwardShading::RenderScene(const Scene& scene) {
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->res_tex, 0);

    // Rendering
    glClearColor(my_color[0], my_color[1], my_color[2], my_color[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    std::shared_ptr<Engine> moon = Engine::getInstance();

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = scene.vec_cams[0]->GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(scene.vec_cams[0]->Zoom), (float)moon->width / (float)moon->height, 0.1f, 100.0f);

    // 光照模型渲染
    this->mpLight_SH->use();
    this->mpLight_SH->setMat4("view", view);
    this->mpLight_SH->setMat4("projection", projection);
    int lightNr = scene.vec_ptLights.size();
    for (int i = 0; i < lightNr; i++) {
        model = glm::mat4(1.0);
        model = glm::translate(model, scene.vec_ptLights[i]->position);
        this->mpLight_SH->setMat4("model", model);
        this->mpLight_SH->setVec3("lightColor", scene.vec_ptLights[i]->diffuse);
        scene.vec_cubes[0]->Draw();
    }

    /* 光照数据加载 */
    // 方向光
    lightNr = scene.vec_dlLights.size();
    for (int i = 0; i < lightNr; i++) {
        scene.vec_dlLights[i]->SendToShader(this->mpFloor_SH, i);
        scene.vec_dlLights[i]->SendToShader(this->mpModel_SH, i);
    }

    // 点光源
    lightNr = scene.vec_ptLights.size();
    for (int i = 0; i < lightNr; i++) {
        scene.vec_ptLights[i]->SendToShader(this->mpFloor_SH, i);
        scene.vec_ptLights[i]->SendToShader(this->mpModel_SH, i);
    }
    
    // 聚光灯
    lightNr = scene.vec_stLights.size();
    for (int i = 0; i < lightNr; i++) {
        scene.vec_stLights[i]->SendToShader(this->mpFloor_SH, i);
        scene.vec_stLights[i]->SendToShader(this->mpModel_SH, i);
    }

    // 模型渲染
    this->mpModel_SH->use();
    this->mpModel_SH->setMat4("view", view);
    this->mpModel_SH->setMat4("projection", projection);
    this->mpModel_SH->setVec3("viewPos", scene.vec_cams[0]->Position);

    model = glm::mat4(1.0);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -10.0f));
    for (int i = 0; i < 2; i++)
    {
        this->mpModel_SH->setMat4("model", glm::scale(model, glm::vec3(0.5f)));
        this->models->Draw(*(this->mpModel_SH));
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0f));
    }

    // 地板渲染
    model = glm::mat4(1.0);
    this->mpFloor_SH->use();
    this->mpFloor_SH->setMat4("view", view);
    this->mpFloor_SH->setMat4("projection", projection);
    this->mpFloor_SH->setMat4("model", model);
    this->mpFloor_SH->setVec3("viewPos", scene.vec_cams[0]->Position);
    int meshNr = scene.vec_floores.size();
    for (int i = 0; i < meshNr; i++) {
        scene.vec_floores[i]->Draw();
    }

    // 天空盒渲染
    glDepthFunc(GL_LEQUAL);
    this->mpSkybox_SH->use();
    this->mpSkybox_SH->setMat4("view", view);
    this->mpSkybox_SH->setMat4("projection", projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, scene.skybox);
    scene.vec_cubes[0]->Draw();
    glDepthFunc(GL_LESS);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ForwardShading::RenderUI() {
    /* Swap front and back buffers */
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //渲染场景的窗口
    {
        ImGui::Begin(u8"渲染窗口");
        ImGui::SetWindowPos(ImVec2(300, 0), ImGuiCond_Always);
        ImGui::SetWindowSize(ImVec2(600, 600), ImGuiCond_Always);
        ImGui::Image((void *)(intptr_t)this->res_tex, ImVec2(512, 512), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
    }
    {
        ImGui::Begin(u8"前向渲染管线参数设置");
        ImGui::SetWindowPos(ImVec2(900, 0), ImGuiCond_Always);
        ImGui::SetWindowSize(ImVec2(300, 600), ImGuiCond_Always);
        //主窗口
        ImGui::Text(u8"用于调整前向渲染管线对应参数");
        // 编辑颜色 (stored as ~4 floats)
        ImGui::ColorEdit4("Color", my_color);
        ImGui::End();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/* 延迟着色 */
// -----------------------------------------------------
DeferredShading::DeferredShading() : Pipeline(Pipeline_TYPE::DEFERREDSHADING)
{
    this->mpGeometry_SH = nullptr;
    this->mpLight_SH = nullptr;
    this->mpSkybox_SH = nullptr;
    this->mpModel_SH = nullptr;
}

static std::vector<glm::vec3> objectPos;
static std::vector<glm::vec3> lightPos;
static std::vector<glm::vec3> lightCol;

void DeferredShading::Init() {
    /* 初始化FBO 和 RT */
    InitFBO(this->fbo, this->rbo, this->res_tex, 720, 720);
    InitBaseGBuffer(this->gBuffer, 720, 720);

    std::shared_ptr<Engine> moon = Engine::getInstance();

    /* 创建灯光和模型 */
    // 灯光加载
    AbstractLight *tempAL = moon->assetManager.um_lights.find("DirectionLight1")->second;
    this->directionLight = std::dynamic_pointer_cast<DirectionLight>(std::shared_ptr<AbstractLight>(tempAL));
    tempAL = moon->assetManager.um_lights.find("PointLight1")->second;
    this->pointLight = std::dynamic_pointer_cast<PointLight>(std::shared_ptr<AbstractLight>(tempAL));
    tempAL = moon->assetManager.um_lights.find("SpotLight1")->second;
    this->spotLight = std::dynamic_pointer_cast<SpotLight>(std::shared_ptr<AbstractLight>(tempAL));
    tempAL = nullptr;

    // 模型加载
    // 模型加载
    Renderable *tempRender = moon->assetManager.um_meshes.find("Cube")->second;
    this->cube = std::dynamic_pointer_cast<Cube>(std::shared_ptr<Renderable>(tempRender));

    tempRender = moon->assetManager.um_meshes.find("ScreenQuad")->second;
    this->quad = std::dynamic_pointer_cast<Quad>(std::shared_ptr<Renderable>(tempRender));
    tempRender = nullptr;

    Model *nanosuit = &(moon->assetManager.um_models.find("nanosuit")->second);
    this->models = std::shared_ptr<Model>(nanosuit);
    nanosuit = nullptr;

    // 光照为位置偏移
    objectPos.push_back(glm::vec3(-3.0, -3.0, -3.0));
    objectPos.push_back(glm::vec3(0.0, -3.0, -3.0));
    objectPos.push_back(glm::vec3(3.0, -3.0, -3.0));
    objectPos.push_back(glm::vec3(-3.0, -3.0, 0.0));
    objectPos.push_back(glm::vec3(0.0, -3.0, 0.0));
    objectPos.push_back(glm::vec3(3.0, -3.0, 0.0));
    objectPos.push_back(glm::vec3(-3.0, -3.0, 3.0));
    objectPos.push_back(glm::vec3(0.0, -3.0, 3.0));
    objectPos.push_back(glm::vec3(3.0, -3.0, 3.0));

    std::random_device seed;
    std::mt19937 eng(seed());
    std::uniform_real_distribution<> distrib(0.0f, 1.0f);
    for (size_t i = 0; i < 32; i++)
    {
        // 光照位置
        GLfloat xPos = distrib(eng) * 6.0f - 3.0f;
        GLfloat yPos = distrib(eng) * 6.0f - 3.0f;
        // GLfloat yPos = 5.0f;
        GLfloat zPos = distrib(eng) * 6.0f - 3.0f;
        lightPos.push_back(glm::vec3(xPos, yPos, zPos));

        // 光照颜色
        GLfloat rCol = distrib(eng);
        GLfloat gCol = distrib(eng);
        GLfloat bCol = distrib(eng);
        lightCol.push_back(glm::vec3(rCol, gCol, bCol));
    }

    // 天空盒加载
    this->env_cubemap = moon->assetManager.um_textures.find("BlueSky")->second;

    /* 着色器加载 */
    if (moon->assetManager.um_shaders.find("Geometry_DS") == moon->assetManager.um_shaders.end()) {
        std::string vsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/Deferred/geometry.vs";
        std::string fsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/Deferred/geometry.fs";
        Shader temp_Geometry(vsPath.c_str(), fsPath.c_str());
        moon->assetManager.um_shaders.emplace("Geometry_DS", temp_Geometry);
    }
    Shader *temp_SH = &(moon->assetManager.um_shaders.find("Geometry_DS")->second);
    this->mpGeometry_SH = std::shared_ptr<Shader>(temp_SH);
    temp_SH = nullptr;

    if (moon->assetManager.um_shaders.find("Light_DS") == moon->assetManager.um_shaders.end())
    {
        std::string vsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/Deferred/light.vs";
        std::string fsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/Deferred/light.fs";
        Shader temp_Light(vsPath.c_str(), fsPath.c_str());
        moon->assetManager.um_shaders.emplace("Light_DS", temp_Light);
    }

    temp_SH = &(moon->assetManager.um_shaders.find("Light_DS")->second);
    this->mpLight_SH = std::shared_ptr<Shader>(temp_SH);
    temp_SH = nullptr;
    mpLight_SH->use();
    mpLight_SH->setInt("gPosition", 0);
    mpLight_SH->setInt("gNormal", 1);
    mpLight_SH->setInt("gColorSpec", 2);

    if (moon->assetManager.um_shaders.find("SkyBox") == moon->assetManager.um_shaders.end())
    {
        std::string vsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/Skybox/Skybox.vs";
        std::string fsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/Skybox/Skybox.fs";
        Shader temp_Skybox(vsPath.c_str(), fsPath.c_str());
        moon->assetManager.um_shaders.emplace("SkyBox", temp_Skybox);
    }
    temp_SH = &(moon->assetManager.um_shaders.find("SkyBox")->second);
    this->mpSkybox_SH = std::shared_ptr<Shader>(temp_SH);
    temp_SH = nullptr;
    this->mpSkybox_SH->use();
    this->mpSkybox_SH->setInt("environmentMap", 0);

    if (moon->assetManager.um_shaders.find("LightModel") == moon->assetManager.um_shaders.end())
    {
        std::string vsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/Forward/model.vs";
        std::string fsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/Forward/light.fs";
        Shader temp_LM(vsPath.c_str(), fsPath.c_str());
        moon->assetManager.um_shaders.emplace("LightModel", temp_LM);
    }
    temp_SH = &(moon->assetManager.um_shaders.find("LightModel")->second);
    this->mpModel_SH = std::shared_ptr<Shader>(temp_SH);
    temp_SH = nullptr;
}

void DeferredShading::Render() {

    glViewport(0, 0, 720, 720);
    // 1. 几何阶段
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    std::shared_ptr<Engine> moon = Engine::getInstance();

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = moon->cam->GetViewMatrix();
    glm::mat4 projection = glm::perspective(moon->cam->Zoom, 1.0f, 0.1f, 100.0f);
    glm::vec3 viewPos = moon->cam->Position;

    this->mpGeometry_SH->use();
    this->mpGeometry_SH->setMat4("projection", projection);
    this->mpGeometry_SH->setMat4("view", view);
    for (size_t i = 0; i < 9; i++) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, objectPos[i]);
        model = glm::scale(model, glm::vec3(0.25f));
        this->mpGeometry_SH->setMat4("model", model);
        this->models->Draw(*(this->mpGeometry_SH));
    }

    // 2. 光照阶段
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->mpLight_SH->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gBuffer.gPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gBuffer.gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gBuffer.gColor);

    // 光照参数
    for (size_t i = 0; i < 32; i++) {
        this->mpLight_SH->setVec3("ptLight[" + std::to_string(i) + "].position", lightPos[i]);
        this->mpLight_SH->setVec3("ptLight[" + std::to_string(i) + "].diffuse",lightCol[i]);
        this->mpLight_SH->setVec3("ptLight[" + std::to_string(i) + "].specular", lightCol[i]);
        const GLfloat kc = 1.0f;
        const GLfloat kl = 0.7f;
        const GLfloat kq = 1.8f;
        this->mpLight_SH->setFloat("ptLight[" + std::to_string(i) + "].kc", kc);
        this->mpLight_SH->setFloat("ptLight[" + std::to_string(i) + "].kl", kl);
        this->mpLight_SH->setFloat("ptLight[" + std::to_string(i) + "].kq", kq);
    }
    this->mpLight_SH->setVec3("viewPos", viewPos);
    this->quad->Draw();

    // 2.5 将几何的深度缓冲复制到fbo中
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.gBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->fbo);
    glBlitFramebuffer(0, 0, 720, 720, 0, 0, 720, 720, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);


    // 3.0 前向渲染发光物
    this->mpModel_SH->use();
    this->mpModel_SH->setMat4("projection", projection);
    this->mpModel_SH->setMat4("view", view);
    for (size_t i = 0; i < lightPos.size(); i++) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos[i]);
        model = glm::scale(model, glm::vec3(0.25f));
        this->mpModel_SH->setMat4("model", model);
        this->mpModel_SH->setVec3("lightColor", lightCol[i]);
        this->cube->Draw();
    }
    
    // 天空盒渲染
    glDepthFunc(GL_LEQUAL);
    this->mpSkybox_SH->use();
    this->mpSkybox_SH->setMat4("view", view);
    this->mpSkybox_SH->setMat4("projection", projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->env_cubemap);
    this->cube->Draw();
    glDepthFunc(GL_LESS);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredShading::RenderScene(const Scene& scene) {

}

void DeferredShading::RenderUI() {
    /* Swap front and back buffers */
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //渲染场景的窗口
    {
        ImGui::Begin(u8"渲染窗口");
        ImGui::SetWindowPos(ImVec2(300, 0), ImGuiCond_Always);
        ImGui::SetWindowSize(ImVec2(600, 600), ImGuiCond_Always);
        ImGui::Image((void *)(intptr_t)this->res_tex, ImVec2(512, 512), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
    }
    {
        ImGui::Begin(u8"延迟渲染管线参数设置");
        ImGui::SetWindowPos(ImVec2(900, 0), ImGuiCond_Always);
        ImGui::SetWindowSize(ImVec2(300, 600), ImGuiCond_Always);
        //主窗口
        ImGui::Text(u8"用于调整延迟渲染管线对应参数");
        ImGui::End();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/* 基于物理的着色 */
// -----------------------------------------------------
PBR::PBR() : Pipeline(Pipeline_TYPE::PBRSHADING)
{
    this->nums = 0;
    // TODO:设置默认着色器
    this->mpHDR_SH = nullptr;
    this->mpIBL_SH = nullptr;
    this->mpPrefilter_SH = nullptr;
    this->mpBRDF_SH = nullptr;
    this->mpSkybox_SH = nullptr;
    this->mpPBR_SH = nullptr;
}

void PBR::Init() {
    this->cube_screen = Cube();
    this->quad_screen = Quad();
    this->spheres = std::make_shared<Sphere>();
    this->nums += 1;
    this->spacing = 10;

    InitFBO(this->fbo, this->rbo, this->res_tex, 720, 720);

    std::shared_ptr<Engine> moon = Engine::getInstance();
    if (moon->assetManager.um_textures.find("RoomHDR") == moon->assetManager.um_textures.end())
    {
        std::string HDR_Path = "D:/C++Pro/vscode/LearnOpenGL/texture/HS-Cave-Room/Mt-Washington-Cave-Room_Ref.hdr";
        moon->assetManager.um_textures.emplace("RoomHDR", loadHDR(HDR_Path.c_str()));
    }
    this->hdr_tex = moon->assetManager.um_textures.find("RoomHDR")->second;
    genCubeMap(this->env_cubemap, 720, 720);
    genCubeMap(this->irradiance_cubemap, 32, 32);

    //设置投影和视角矩阵
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    std::vector<glm::mat4> captureViews =
        {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))

        };

    HDRInit(captureProjection, captureViews);
    IBLInit(captureProjection, captureViews);
    PrefilterInit(captureProjection, captureViews);
    BRDFInit();

    if (moon->assetManager.um_shaders.find("SkyBox") == moon->assetManager.um_shaders.end())
    {
        std::string vsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/Skybox/Skybox.vs";
        std::string fsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/Skybox/Skybox.fs";
        Shader temp_Skybox(vsPath.c_str(), fsPath.c_str());
        moon->assetManager.um_shaders.emplace("SkyBox", temp_Skybox);
    }
    Shader* temp_SH = &(moon->assetManager.um_shaders.find("SkyBox")->second);
    this->mpSkybox_SH = std::shared_ptr<Shader>(temp_SH);
    temp_SH = nullptr;
    this->mpSkybox_SH->use();
    this->mpSkybox_SH->setInt("environmentMap", 0);

    if (moon->assetManager.um_shaders.find("PBR") == moon->assetManager.um_shaders.end())
    {
        std::string vsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/PBR/VertexShader.vs";
        std::string fsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/PBR/FragmentShader.Fs";
        Shader temp_Skybox(vsPath.c_str(), fsPath.c_str());
        moon->assetManager.um_shaders.emplace("PBR", temp_Skybox);
    }
    temp_SH = &(moon->assetManager.um_shaders.find("PBR")->second);
    this->mpPBR_SH = std::shared_ptr<Shader>(temp_SH);
    temp_SH = nullptr;
    this->mpPBR_SH->use();
    this->mpPBR_SH->setInt("albedoMap", 0);
    this->mpPBR_SH->setInt("normalMap", 1);
    // this->mpPBR_SH->setInt("metallicMap", 2);
    // this->mpPBR_SH->setInt("roughnessMap", 3);
    this->mpPBR_SH->setInt("aoMap", 4);
    this->mpPBR_SH->setInt("irradianceMap", 5);
    this->mpPBR_SH->setInt("prefilterMap", 6);
    this->mpPBR_SH->setInt("brdfLUT", 7);

    // 模型加载
    Model *nanosuit = &(moon->assetManager.um_models.find("nanosuit")->second);
    this->models = std::shared_ptr<Model>(nanosuit);
}

void PBR::HDRInit(glm::mat4 &captureProjection, std::vector<glm::mat4> &captureViews)
{
    std::shared_ptr<Engine> moon = Engine::getInstance();
    if (moon->assetManager.um_shaders.find("HDR") == moon->assetManager.um_shaders.end())
    {
        std::string vsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/PBR/HDR2CUBE.vs";
        std::string fsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/PBR/HDR2CUBE.Fs";
        Shader temp_Skybox(vsPath.c_str(), fsPath.c_str());
        moon->assetManager.um_shaders.emplace("HDR", temp_Skybox);
    }
    Shader *temp_SH = &(moon->assetManager.um_shaders.find("HDR")->second);
    this->mpHDR_SH = std::shared_ptr<Shader>(temp_SH);
    temp_SH = nullptr;
    this->mpHDR_SH->use();
    this->mpHDR_SH->setInt("equirectangularMap", 0);
    this->mpHDR_SH->setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->hdr_tex);

    glViewport(0, 0, 720, 720);
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    for (unsigned int i = 0; i < 6; i++)
    {
        this->mpHDR_SH->setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->env_cubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        this->cube_screen.Draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, this->env_cubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void PBR::IBLInit(glm::mat4 &captureProjection, std::vector<glm::mat4> &captureViews)
{
    std::shared_ptr<Engine> moon = Engine::getInstance();
    if (moon->assetManager.um_shaders.find("IBL") == moon->assetManager.um_shaders.end())
    {
        std::string vsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/PBR/HDR2CUBE.vs";
        std::string fsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/PBR/HDR2IBL.Fs";
        Shader temp_Skybox(vsPath.c_str(), fsPath.c_str());
        moon->assetManager.um_shaders.emplace("IBL", temp_Skybox);
    }
    Shader *temp_SH = &(moon->assetManager.um_shaders.find("IBL")->second);
    this->mpIBL_SH = std::shared_ptr<Shader>(temp_SH);
    temp_SH = nullptr;
    this->mpIBL_SH->use();
    // PBR: 辐照度贴图
    this->mpIBL_SH->use();
    this->mpIBL_SH->setInt("environmentMap", 0);
    this->mpIBL_SH->setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->env_cubemap);

    glViewport(0, 0, 32, 32);
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    for (unsigned int i = 0; i < 6; i++)
    {
        this->mpIBL_SH->setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->irradiance_cubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        this->cube_screen.Draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PBR::PrefilterInit(glm::mat4 &captureProjection, std::vector<glm::mat4> &captureViews)
{
    std::shared_ptr<Engine> moon = Engine::getInstance();
    if (moon->assetManager.um_shaders.find("PreFilter") == moon->assetManager.um_shaders.end())
    {
        std::string vsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/PBR/HDR2CUBE.vs";
        std::string fsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/PBR/Prefilter.Fs";
        Shader temp_Skybox(vsPath.c_str(), fsPath.c_str());
        moon->assetManager.um_shaders.emplace("PreFilter", temp_Skybox);
    }
    Shader *temp_SH = &(moon->assetManager.um_shaders.find("PreFilter")->second);
    this->mpPrefilter_SH = std::shared_ptr<Shader>(temp_SH);
    temp_SH = nullptr;

    // PBR: 预滤波立方体贴图
    // --------------------------------------
    genCubeMap(this->prefilter_cubemap, 128, 128);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // PBR: 拟蒙特卡洛生成环境光得预滤波贴图
    // --------------------------------------
    this->mpPrefilter_SH->use();
    this->mpPrefilter_SH->setInt("environmentMap", 0);
    this->mpPrefilter_SH->setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->env_cubemap);
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; mip++)
    {
        // 根据mip等级来重新修改framebuffer尺寸
        unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        this->mpPrefilter_SH->setFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; i++)
        {
            this->mpPrefilter_SH->setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->prefilter_cubemap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            this->cube_screen.Draw();
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PBR::BRDFInit() {
    std::shared_ptr<Engine> moon = Engine::getInstance();
    if (moon->assetManager.um_shaders.find("BRDF") == moon->assetManager.um_shaders.end())
    {
        std::string vsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/PBR/brdf.vs";
        std::string fsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/PBR/brdf.Fs";
        Shader temp_Skybox(vsPath.c_str(), fsPath.c_str());
        moon->assetManager.um_shaders.emplace("BRDF", temp_Skybox);
    }
    Shader *temp_SH = &(moon->assetManager.um_shaders.find("BRDF")->second);
    this->mpBRDF_SH = std::shared_ptr<Shader>(temp_SH);
    temp_SH = nullptr;
    // PBR: 生成2D LUT
    // -----------------------------------------
    glGenTextures(1, &(this->brdf_tex));

    glBindTexture(GL_TEXTURE_2D, this->brdf_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 720, 720, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 720, 720);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->brdf_tex, 0);

    glViewport(0, 0, 720, 720);
    this->mpBRDF_SH->use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    this->quad_screen.Draw();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PBR::Render()
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->res_tex, 0);

    // Rendering
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    std::shared_ptr<Engine> moon = Engine::getInstance();

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = moon->cam->GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(moon->cam->Zoom), (float)moon->width / (float)moon->height, 0.1f, 100.0f);
    
    this->mpPBR_SH->use();
    this->mpPBR_SH->setMat4("view", view);
    this->mpPBR_SH->setMat4("projection", projection);
    this->mpPBR_SH->setVec3("camPos", moon->cam->Position);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->irradiance_cubemap);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->prefilter_cubemap);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, this->brdf_tex);

    //TODO:添加ui获取参数
    //渲染材质球
    for (unsigned i = 0; i < this->nums; i++) {
        model = glm::translate(model, glm::vec3((float)(i - (this->nums / 2)) * spacing, (float)(i - (this->nums / 2)) * spacing, -2.0));
        this->mpPBR_SH->setMat4("model", model);
        this->mpPBR_SH->setFloat("metal", this->spheres->mental);
        this->mpPBR_SH->setFloat("rough", this->spheres->rough);
        //this->spheres->Draw();
        this->models->Draw(*(this->mpPBR_SH));
    }

    //TODO:实例化优化
    //渲染光源
    const float pi = 3.1415926;
    for (auto i = 0; i < 4; i++) {
        float coff_pi = pi * i;
        glm::vec3 newPos = glm::vec3(10.0f * cos(-pi * 0.5 + coff_pi), 10.0f * (i < 2 ? 1.0 : -1.0), 10.0f);
        this->mpPBR_SH->setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
        this->mpPBR_SH->setVec3("lightColors[" + std::to_string(i) + "]", glm::vec3(300.f));

        model = glm::mat4(1.0f);
        model = glm::translate(model, newPos);
        model = glm::scale(model, glm::vec3(0.5f));
        this->mpPBR_SH->setMat4("model", model);
        this->spheres->Draw();
    }

    glDepthFunc(GL_LEQUAL);
    this->mpSkybox_SH->use();
    this->mpSkybox_SH->setMat4("view", view);
    this->mpSkybox_SH->setMat4("projection", projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->env_cubemap);
    this->cube_screen.Draw();
    //glDepthFunc(GL_LESS);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PBR::RenderScene(const Scene& scene) {

}

void PBR::RenderUI() {
    /* Swap front and back buffers */
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    //渲染场景的窗口
    {
        ImGui::Begin(u8"渲染窗口");
        ImGui::SetWindowPos(ImVec2(300, 0), ImGuiCond_Always);
        ImGui::SetWindowSize(ImVec2(600, 600), ImGuiCond_Always);
        ImGui::Image((void *)(intptr_t)this->res_tex, ImVec2(600, 600), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
    }
    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        ImGui::Begin(u8"PBR渲染管线参数设置");
        ImGui::SetWindowPos(ImVec2(900, 0), ImGuiCond_Always);
        ImGui::SetWindowSize(ImVec2(300, 600), ImGuiCond_Always);
        //主窗口
        ImGui::Text(u8"用于调整PBR渲染管线对应参数");
        if (this->spheres != nullptr)
        {
            ImGui::SliderFloat("mental", &(this->spheres->mental), 0.0f, 1.0f);
            ImGui::SliderFloat("rough", &(this->spheres->rough), 0.0f, 1.0f);
        }
        ImGui::Text(u8"帧率 %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/* 全局功能函数 */
// ---------------------------------------------------------------
// 初始化FBO和RT
void InitFBO(unsigned int &fbo, unsigned int &rbo, unsigned int &tex, GLsizei width, GLsizei height)
{
    genFramebuffer(fbo, rbo, width, height);

    glGenTextures(1, &(tex));

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
}

// 初始化GBuffer
void InitBaseGBuffer(GBuffer &gbuffer, GLsizei width, GLsizei height) {
    glGenFramebuffers(1, &(gbuffer.gBuffer));
    glBindFramebuffer(GL_FRAMEBUFFER, gbuffer.gBuffer);

    // 位置颜色附件
    glGenTextures(1, &(gbuffer.gPosition));
    glBindTexture(GL_TEXTURE_2D, gbuffer.gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gbuffer.gPosition, 0);

    // 法线颜色附件
    glGenTextures(1, &(gbuffer.gNormal));
    glBindTexture(GL_TEXTURE_2D, gbuffer.gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gbuffer.gNormal, 0);

    // 颜色 + 镜面颜色附件
    glGenTextures(1, &(gbuffer.gColor));
    glBindTexture(GL_TEXTURE_2D, gbuffer.gColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gbuffer.gColor, 0);

    // 将颜色附件加入帧缓冲
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0,
                              GL_COLOR_ATTACHMENT1,
                              GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    // 深度缓冲
    glGenRenderbuffers(1, &(gbuffer.gDepthRBO));
    glBindRenderbuffer(GL_RENDERBUFFER, gbuffer.gDepthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gbuffer.gDepthRBO);

    // 检查framebuffer是否完整
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
