#include <learnopengl/shadow.h>
#include <string>
#include <learnopengl/pipeline.h>
#include <learnopengl/engine.h>
#include <learnopengl/myutils.h>

Shadow::Shadow() {
    type = SHADOW_TYPE::WITHOUT;
    mpSM_SH = nullptr;
    width = 1024;
    height = 1024;
    bias = 0.005;
    isFFC = false;
}

Shadow::~Shadow() {
}

void Shadow::Init() {
    std::shared_ptr<Engine> moon = Engine::getInstance();
    // if (moon->assetManager.um_tex.find("SM") == moon->assetManager.um_tex.end()) {
    //     moon->assetManager.um_tex.emplace("SM", *(this->sm_tex));
    // }
    // this->sm_tex = &(moon->assetManager.um_tex.find("SM")->second);
    // // InitFBO(this->FBO, this->RBO, *(this->sm_tex), this->width, this->height);
    // InitDepthFBO(this->FBO, this->RBO, *(this->sm_tex), this->width, this->height);

    // if (moon->assetManager.um_shaders.find("SkyBox") == moon->assetManager.um_shaders.end())
    // {
    //     std::string vsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/Shadow/shadow.vs";
    //     std::string fsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/Shadow/without.fs";
    //     Shader temp_SM(vsPath.c_str(), fsPath.c_str());
    //     moon->assetManager.um_shaders.emplace("ShadowMap", temp_SM);
    // }
    // Shader *temp_SH = &(moon->assetManager.um_shaders.find("ShadowMap")->second);
    // this->mpSM_SH = std::shared_ptr<Shader>(temp_SH);
    // temp_SH = nullptr;
}

void Shadow::RenderSM(PointLight &light) {
    GLfloat near_plane = 1.0f, far_plane = 7.5f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(light.position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpcaeMatrix = lightProjection * lightView;
    this->mpSM_SH->use();
    this->mpSM_SH->setMat4("lightSpaceMatrix", lightSpcaeMatrix);

    glViewport(0, 0, this->width, this->height);
    glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}