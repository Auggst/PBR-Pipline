#include <learnopengl/pipeline.h>

#include <string>
#include <iostream>

#include <learnopengl/myutils.h>
#include <learnopengl/engine.h>

ForwardShading::ForwardShading() {
    this->mpModel_SH = nullptr;
    this->mpLight_SH = nullptr;
    this->mpSkybox_SH = nullptr;
}

void ForwardShading::init() {
    initFBO(512, 512);
    this->cube = std::make_shared<Cube>();

    std::string vsPath = "D:\\C++Pro\\vscode\\LearnOpenGL\\src\\shader\\Forward\\model.vs";
    std::string fsPath = "D:\\C++Pro\\vscode\\LearnOpenGL\\src\\shader\\Forward\\model.fs";
    // this->mpModel_SH = std::make_shared<Shader>(vsPath.c_str(), fsPath.c_str());
    // this->mpModel_SH->use();
    // this->mpModel_SH->setInt("material.shininess", 64.0);

    fsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/Forward/light.fs";
    this->mpLight_SH = std::make_shared<Shader>(vsPath.c_str(), fsPath.c_str());
    this->light_tex = loadTexture("D:/C++Pro/vscode/LearnOpenGL/texture/HS-Cave-Room/Mt-Washington-Cave-Room_Bg.jpg");
    this->mpLight_SH->use();
    this->mpLight_SH->setInt("diffuse", 0);

    // vsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/Skybox.vs";
    // fsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/Skybox.fs";
    // this->mpSkybox_SH = std::make_shared<Shader>(vsPath.c_str(), fsPath.c_str());

    // 模型加载
    std::string modelPath = "D:/C++Pro/data/nanosuit/nanosuit.obj";
    this->models = std::make_shared<Model>(modelPath);

    // 灯光加载
}

void ForwardShading::render() {
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->res_tex, 0);

    // Rendering
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    std::shared_ptr<Engine> moon = Engine::getInstance();

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = moon->cam->GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(moon->cam->Zoom), (float)moon->width / (float)moon->height, 0.1f, 100.0f);

    // 光照模型渲染
    this->mpSkybox_SH->use();
    this->mpSkybox_SH->setMat4("view", view);
    this->mpSkybox_SH->setMat4("projection", projection);
    const float pi = 3.1415926;
    for (int i = 0; i < 4; i++) {
        model = glm::mat4(1.0);
        float coff_pi = pi * i;
        glm::vec3 newPos = glm::vec3(10.0f * cos(-pi * 0.5 + coff_pi), 10.0f * (i < 2 ? 1.0 : -1.0), 10.0f);
        model = glm::translate(model, newPos);
        this->mpSkybox_SH->setMat4("model", model);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->light_tex);
        this->cube->Draw();
    }

    // // 模型渲染
    // this->mpModel_SH->use();
    // this->mpModel_SH->setMat4("view", view);
    // this->mpModel_SH->setMat4("projection", projection);
    // this->mpModel_SH->setVec3("viewPos", moon->cam->Position);
    // this->mpModel_SH->setInt("plNums", 4);
    // this->mpModel_SH->setInt("dlNums", 0);
    // this->mpModel_SH->setInt("slNums", 0);
    // for (int i = 0; i < 4; i++) {
    //     model = glm::mat4(1.0);
    //     float coff_pi = pi * i;
    //     glm::vec3 newPos = glm::vec3(10.0f * cos(-pi * 0.5 + coff_pi), 10.0f * (i < 2 ? 1.0 : -1.0), 10.0f);
    //     this->mpModel_SH->setVec3("plLight[" + std::to_string(i) + "].position", newPos);
    //     glm::vec3 am(0.05 * cos(-pi * 0.5 * i));
    //     glm::vec3 diff(0.8 * cos(-pi * 0.5 * i));
    //     glm::vec3 spec(1.0 * cos(-pi * 0.5 * i));
    //     this->mpModel_SH->setVec3("plLight[" + std::to_string(i) + "].ambient", am);
    //     this->mpModel_SH->setVec3("plLight[" + std::to_string(i) + "].diffuse", diff);
    //     this->mpModel_SH->setVec3("plLight[" + std::to_string(i) + "].specular", spec);

    //     this->mpModel_SH->setFloat("plLight[" + std::to_string(i) + "].kc", 1.0f);
    //     this->mpModel_SH->setFloat("plLight[" + std::to_string(i) + "].kl", 0.009f);
    //     this->mpModel_SH->setFloat("plLight[" + std::to_string(i) + "].kq", 0.032f);
    // }
    // this->mpModel_SH->setMat4("model", model);
    // this->models->Draw(*(this->mpModel_SH));

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ForwardShading::renderUI() {
    /* Swap front and back buffers */
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        ImGui::Begin(u8"前向渲染管线参数设置");
        ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetWindowSize(ImVec2(300, 600), ImGuiCond_Always);
        //主窗口
        ImGui::Text(u8"用于调整前向渲染管线对应参数");
        ImGui::Text(u8"帧率 %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // ImVec2 pos = ImGui::GetCursorScreenPos();
    // glViewport(0, 0, 512, 512);
    //渲染场景的窗口
    {
        ImGui::Begin(u8"渲染窗口");
        ImGui::SetWindowPos(ImVec2(300, 0), ImGuiCond_Always);
        ImGui::SetWindowSize(ImVec2(700, 600), ImGuiCond_Always);
        ImGui::Image((void *)(intptr_t)this->res_tex, ImVec2(700, 700), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ForwardShading::initFBO(GLsizei width, GLsizei height) {
    genFramebuffer(this->fbo, this->rbo, width, height);
    glGenTextures(1, &(this->res_tex));

    glBindTexture(GL_TEXTURE_2D, this->res_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

PBR::PBR(){
    this->nums = 0;
    // TODO:设置默认着色器
    this->HDR_SH = nullptr;
    this->IBL_SH = nullptr;
    this->Prefilter_SH = nullptr;
    this->BRDF_SH = nullptr;
    this->Skybox_SH = nullptr;
    this->PBR_SH = nullptr;
}

void PBR::init() {
    this->cube_screen = Cube();
    this->quad_screen = Quad();
    this->spheres = std::make_shared<Sphere>();
    this->nums += 1;
    this->spacing = 10;
    genFramebuffer(this->fbo, this->rbo, 512, 512);
    glGenTextures(1, &(this->res_tex));

    glBindTexture(GL_TEXTURE_2D, this->res_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    this->hdr_tex = loadHDR("D:\\C++Pro\\vscode\\LearnOpenGL\\texture\\HS-Cave-Room\\Mt-Washington-Cave-Room_Ref.hdr");
    genCubeMap(this->env_cubemap, 512, 512);
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

    std::string vs_fn;
    std::string fs_fn;

    vs_fn = "D:\\C++Pro\\vscode\\LearnOpenGL\\src\\shader\\Skybox.vs";
    fs_fn = "D:\\C++Pro\\vscode\\LearnOpenGL\\src\\shader\\Skybox.fs";
    this->Skybox_SH = std::make_shared<Shader>(vs_fn.c_str(), fs_fn.c_str());
    this->Skybox_SH->use();
    this->Skybox_SH->setInt("environmentMap", 0);


    vs_fn = "D:\\C++Pro\\vscode\\LearnOpenGL\\src\\shader\\VertexShader.vs";
    fs_fn = "D:\\C++Pro\\vscode\\LearnOpenGL\\src\\shader\\FragmentShader.fs";
    this->PBR_SH = std::make_shared<Shader>(vs_fn.c_str(), fs_fn.c_str());
    this->PBR_SH->use();
    this->PBR_SH->setInt("albedoMap", 0);
    this->PBR_SH->setInt("normalMap", 1);
    // this->PBR_SH->setInt("metallicMap", 2);
    // this->PBR_SH->setInt("roughnessMap", 3);
    this->PBR_SH->setInt("aoMap", 4);
    this->PBR_SH->setInt("irradianceMap", 5);
    this->PBR_SH->setInt("prefilterMap", 6);
    this->PBR_SH->setInt("brdfLUT", 7);

    // 模型加载
    std::string modelPath = "D:/C++Pro/data/nanosuit/nanosuit.obj";
    this->models = std::make_shared<Model>(modelPath);
}

void PBR::HDRInit(glm::mat4 &captureProjection, std::vector<glm::mat4> &captureViews)
{
    std::string vs_fn = "D:\\C++Pro\\vscode\\LearnOpenGL\\src\\shader\\HDR2CUBE.vs";
    std::string fs_fn = "D:\\C++Pro\\vscode\\LearnOpenGL\\src\\shader\\HDR2CUBE.fs";
    this->HDR_SH = std::make_shared<Shader>(vs_fn.c_str(), fs_fn.c_str());
    this->HDR_SH->use();
    this->HDR_SH->setInt("equirectangularMap", 0);
    this->HDR_SH->setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->hdr_tex);

    glViewport(0, 0, 512, 512);
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    for (unsigned int i = 0; i < 6; i++)
    {
        this->HDR_SH->setMat4("view", captureViews[i]);
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

    std::string vs_fn = "D:\\C++Pro\\vscode\\LearnOpenGL\\src\\shader\\HDR2CUBE.vs";
    std::string fs_fn = "D:\\C++Pro\\vscode\\LearnOpenGL\\src\\shader\\HDR2IBL.fs";
    this->IBL_SH = std::make_shared<Shader>(vs_fn.c_str(), fs_fn.c_str());
    // PBR: 辐照度贴图
    this->IBL_SH->use();
    this->IBL_SH->setInt("environmentMap", 0);
    this->IBL_SH->setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->env_cubemap);

    glViewport(0, 0, 32, 32);
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    for (unsigned int i = 0; i < 6; i++)
    {
        this->IBL_SH->setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->irradiance_cubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        this->cube_screen.Draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PBR::PrefilterInit(glm::mat4 &captureProjection, std::vector<glm::mat4> &captureViews)
{
    std::string vs_fn = "D:\\C++Pro\\vscode\\LearnOpenGL\\src\\shader\\HDR2CUBE.vs";
    std::string fs_fn = "D:\\C++Pro\\vscode\\LearnOpenGL\\src\\shader\\Prefilter.fs";
    this->Prefilter_SH = std::make_shared<Shader>(vs_fn.c_str(), fs_fn.c_str());

    // PBR: 预滤波立方体贴图
    // --------------------------------------
    genCubeMap(this->prefilter_cubemap, 128, 128);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // PBR: 拟蒙特卡洛生成环境光得预滤波贴图
    // --------------------------------------
    this->Prefilter_SH->use();
    this->Prefilter_SH->setInt("environmentMap", 0);
    this->Prefilter_SH->setMat4("projection", captureProjection);
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
        this->Prefilter_SH->setFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; i++)
        {
            this->Prefilter_SH->setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->prefilter_cubemap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            this->cube_screen.Draw();
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PBR::BRDFInit() {
    std::string vs_fn = "D:\\C++Pro\\vscode\\LearnOpenGL\\src\\shader\\brdf.vs";
    std::string fs_fn = "D:\\C++Pro\\vscode\\LearnOpenGL\\src\\shader\\brdf.fs";
    this->BRDF_SH = std::make_shared<Shader>(vs_fn.c_str(), fs_fn.c_str());

    // PBR: 生成2D LUT
    // -----------------------------------------
    glGenTextures(1, &(this->brdf_tex));

    glBindTexture(GL_TEXTURE_2D, this->brdf_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->brdf_tex, 0);

    glViewport(0, 0, 512, 512);
    this->BRDF_SH->use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    this->quad_screen.Draw();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PBR::render()
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
    
    this->PBR_SH->use();
    this->PBR_SH->setMat4("view", view);
    this->PBR_SH->setMat4("projection", projection);
    this->PBR_SH->setVec3("camPos", moon->cam->Position);
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
        this->PBR_SH->setMat4("model", model);
        this->PBR_SH->setFloat("metal", this->spheres->mental);
        this->PBR_SH->setFloat("rough", this->spheres->rough);
        //this->spheres->Draw();
        this->models->Draw(*(this->PBR_SH));
    }

    //TODO:实例化优化
    //渲染光源
    const float pi = 3.1415926;
    for (auto i = 0; i < 4; i++) {
        float coff_pi = pi * i;
        glm::vec3 newPos = glm::vec3(10.0f * cos(-pi * 0.5 + coff_pi), 10.0f * (i < 2 ? 1.0 : -1.0), 10.0f);
        this->PBR_SH->setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
        this->PBR_SH->setVec3("lightColors[" + std::to_string(i) + "]", glm::vec3(300.f));

        model = glm::mat4(1.0f);
        model = glm::translate(model, newPos);
        model = glm::scale(model, glm::vec3(0.5f));
        this->PBR_SH->setMat4("model", model);
        this->spheres->Draw();
    }

    glDepthFunc(GL_LEQUAL);
    this->Skybox_SH->use();
    this->Skybox_SH->setMat4("view", view);
    this->Skybox_SH->setMat4("projection", projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->env_cubemap);
    this->cube_screen.Draw();
    //glDepthFunc(GL_LESS);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PBR::renderUI() {
    /* Swap front and back buffers */
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        ImGui::Begin(u8"PBR渲染管线参数设置");
        ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetWindowSize(ImVec2(300, 600), ImGuiCond_Always);
        //主窗口
        ImGui::Text(u8"用于调整PBR渲染管线对应参数");
        if (this->spheres != nullptr) {
            ImGui::SliderFloat("mental", &(this->spheres->mental), 0.0f, 1.0f);
            ImGui::SliderFloat("rough", &(this->spheres->rough), 0.0f, 1.0f);
        }
        ImGui::Text(u8"帧率 %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // ImVec2 pos = ImGui::GetCursorScreenPos();
    // glViewport(0, 0, 512, 512);
    //渲染场景的窗口
    {
        ImGui::Begin(u8"渲染窗口");
        ImGui::SetWindowPos(ImVec2(300, 0), ImGuiCond_Always);
        ImGui::SetWindowSize(ImVec2(700, 600), ImGuiCond_Always);
        ImGui::Image((void *)(intptr_t)this->res_tex, ImVec2(700, 700), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
