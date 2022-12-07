#include <learnopengl/scene.h>
#include <learnopengl/engine.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Scene::Scene(){
}

Scene::~Scene() {
}

void Scene::ForwardScene() {
    std::shared_ptr<Engine> moon = Engine::getInstance();

    /* 创建相机 */
    Camera *temp_Cam = &(moon->assetManager.um_cameras.find("Main")->second);
    vec_cams.emplace_back(std::shared_ptr<Camera>(temp_Cam));
    temp_Cam = nullptr;

    /* 创建灯光和模型 */
    // 灯光加载
    AbstractLight *tempAL = moon->assetManager.um_lights.find("DirectionLight1")->second;
    vec_dlLights.emplace_back(std::dynamic_pointer_cast<DirectionLight>(std::shared_ptr<AbstractLight>(tempAL)));

    tempAL = moon->assetManager.um_lights.find("PointLight1")->second;
    vec_ptLights.emplace_back(std::dynamic_pointer_cast<PointLight>(std::shared_ptr<AbstractLight>(tempAL)));

    tempAL = moon->assetManager.um_lights.find("SpotLight1")->second;
    vec_stLights.emplace_back(std::dynamic_pointer_cast<SpotLight>(std::shared_ptr<AbstractLight>(tempAL)));
    tempAL = nullptr;

    // 模型加载
    Renderable *tempRender = moon->assetManager.um_meshes.find("Cube")->second;
    vec_cubes.emplace_back(std::dynamic_pointer_cast<Cube>(std::shared_ptr<Renderable>(tempRender)));

    tempRender = moon->assetManager.um_meshes.find("Floor")->second;
    vec_floores.emplace_back(std::dynamic_pointer_cast<Floor>(std::shared_ptr<Renderable>(tempRender)));
    tempRender = nullptr;

    Model *nanosuit = &(moon->assetManager.um_models.find("Nanosuit")->second);
    vec_models.emplace_back(std::shared_ptr<Model>(nanosuit));
    nanosuit = nullptr;

    // 天空盒加载
    this->skybox = moon->assetManager.um_textures.find("BlueSky")->second;
}

void Scene::UpdateLight() {
    // 光照参数
    const float pi = 3.1415926;
    float timeVal = glfwGetTime();
    float greenVal = (sinf(timeVal) / 2) + 0.5f; // 约束到[0, 1]
    float redVal = (cosf(timeVal) / 2) + 0.5f;
    float blueVal = (cosf(timeVal * pi) / 2) + 0.5f;
    glm::vec3 am(0.05f, 0.01f, 0.01f);
    glm::vec3 diff(redVal, greenVal, blueVal);
    glm::vec3 spec(0.5f, 0.5f, 0.5f);

    // 方向光
    int lightNr = this->vec_dlLights.size();
    for (int i = 0; i < lightNr; i++) {
        float coff_pi = pi * i;
        glm::vec3 newPos = glm::vec3(-2.0f * cosf(coff_pi), 4.0f * cosf(std::clamp(coff_pi - pi, 0.0f, pi)), 10.0f);
        this->vec_dlLights[i]->direction = glm::vec3(0.0f) - newPos;
        this->vec_dlLights[i]->diffuse = diff;
    }

    // 点光源
    lightNr = this->vec_ptLights.size();
    for (int i = 0; i < lightNr; i++) {
        this->vec_ptLights[i]->position = glm::vec3(0.0f, 15.0f, 0.0f);
        this->vec_ptLights[i]->diffuse = diff;
    }

    // 聚光灯
    lightNr = this->vec_stLights.size();
    for (int i = 0; i < lightNr; i++) {
        this->vec_stLights[i]->position = this->vec_cams[0]->Position;
        this->vec_stLights[i]->direction = this->vec_cams[0]->Front;
        this->vec_stLights[i]->ambient = glm::vec3(0.001f);
        this->vec_stLights[i]->diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
        this->vec_stLights[i]->specular = glm::vec3(1.0f, 1.0f, 1.0f);
        this->vec_stLights[i]->kc = 1.0f;
        this->vec_stLights[i]->kl = 0.09f;
        this->vec_stLights[i]->kq = 0.0032f;
    }
} 

void Scene::DeferredScene() {
    std::shared_ptr<Engine> moon = Engine::getInstance();

    /* 创建相机 */
    Camera *temp_Cam = &(moon->assetManager.um_cameras.find("Main")->second);
    vec_cams.emplace_back(std::shared_ptr<Camera>(temp_Cam));
    temp_Cam = nullptr;

    /* 创建灯光和模型 */
    // 灯光加载
    AbstractLight *tempAL = moon->assetManager.um_lights.find("DirectionLight1")->second;
    vec_dlLights.emplace_back(std::dynamic_pointer_cast<DirectionLight>(std::shared_ptr<AbstractLight>(tempAL)));

    tempAL = moon->assetManager.um_lights.find("PointLight1")->second;
    vec_ptLights.emplace_back(std::dynamic_pointer_cast<PointLight>(std::shared_ptr<AbstractLight>(tempAL)));

    tempAL = moon->assetManager.um_lights.find("SpotLight1")->second;
    vec_stLights.emplace_back(std::dynamic_pointer_cast<SpotLight>(std::shared_ptr<AbstractLight>(tempAL)));
    tempAL = nullptr;

    // 模型加载
    Renderable *tempRender = moon->assetManager.um_meshes.find("Cube")->second;
    vec_cubes.emplace_back(std::dynamic_pointer_cast<Cube>(std::shared_ptr<Renderable>(tempRender)));

    tempRender = moon->assetManager.um_meshes.find("ScreenQuad")->second;
    vec_quads.emplace_back(std::dynamic_pointer_cast<Quad>(std::shared_ptr<Renderable>(tempRender)));

    tempRender = moon->assetManager.um_meshes.find("Floor")->second;
    vec_floores.emplace_back(std::dynamic_pointer_cast<Floor>(std::shared_ptr<Renderable>(tempRender)));
    tempRender = nullptr;

    Model *nanosuit = &(moon->assetManager.um_models.find("Nanosuit")->second);
    vec_models.emplace_back(std::shared_ptr<Model>(nanosuit));
    nanosuit = nullptr;

    // 天空盒加载
    this->skybox = moon->assetManager.um_textures.find("BlueSky")->second;
}

void Scene::PBRScene() {
    std::shared_ptr<Engine> moon = Engine::getInstance();

    /* 创建相机 */
    Camera *temp_Cam = &(moon->assetManager.um_cameras.find("Main")->second);
    vec_cams.emplace_back(std::shared_ptr<Camera>(temp_Cam));
    temp_Cam = nullptr;

    /* 创建灯光和模型 */
    // 灯光加载
    AbstractLight *tempAL = moon->assetManager.um_lights.find("DirectionLight1")->second;
    vec_dlLights.emplace_back(std::dynamic_pointer_cast<DirectionLight>(std::shared_ptr<AbstractLight>(tempAL)));

    tempAL = moon->assetManager.um_lights.find("PointLight1")->second;
    vec_ptLights.emplace_back(std::dynamic_pointer_cast<PointLight>(std::shared_ptr<AbstractLight>(tempAL)));

    tempAL = moon->assetManager.um_lights.find("SpotLight1")->second;
    vec_stLights.emplace_back(std::dynamic_pointer_cast<SpotLight>(std::shared_ptr<AbstractLight>(tempAL)));
    tempAL = nullptr;

    // 模型加载
    Renderable *tempRender = moon->assetManager.um_meshes.find("Cube")->second;
    vec_cubes.emplace_back(std::dynamic_pointer_cast<Cube>(std::shared_ptr<Renderable>(tempRender)));

    tempRender = moon->assetManager.um_meshes.find("ScreenQuad")->second;
    vec_quads.emplace_back(std::dynamic_pointer_cast<Quad>(std::shared_ptr<Renderable>(tempRender)));

    tempRender = moon->assetManager.um_meshes.find("Floor")->second;
    vec_floores.emplace_back(std::dynamic_pointer_cast<Floor>(std::shared_ptr<Renderable>(tempRender)));

    tempRender = moon->assetManager.um_meshes.find("Sphere")->second;
    vec_spheres.emplace_back(std::dynamic_pointer_cast<Sphere>(std::shared_ptr<Renderable>(tempRender)));
    tempRender = nullptr;

    Model *nanosuit = &(moon->assetManager.um_models.find("Nanosuit")->second);
    vec_models.emplace_back(std::shared_ptr<Model>(nanosuit));
    nanosuit = nullptr;

    // 天空盒加载
    this->skybox = moon->assetManager.um_textures.find("BlueSky")->second;
}
