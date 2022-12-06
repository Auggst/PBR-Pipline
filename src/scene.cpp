#include <learnopengl/scene.h>
#include <learnopengl/engine.h>

Scene::Scene(){
}

Scene::~Scene() {
}

void Scene::InitScene() {
    std::shared_ptr<Engine> moon = Engine::getInstance();

    // // 初始化模型
    // Model *nanosuit = &(moon->assetManager.um_models.find("nanosuit")->second);
    // this->models.emplace_back(std::shared_ptr<Model>(nanosuit));
    // nanosuit = nullptr;
    // // 初始化Mesh
    // Floor *floor = &(moon->assetManager.um_models.find("floor")->second);
    // this->quads.emplace_back(std::shared_ptr<Floor>(floor));
    // floor = nullptr;
    // // 初始化灯光
}
