#include <learnopengl/asset.h>

#include <learnopengl/myutils.h>

Asset::Asset() {
    um_shaders = std::unordered_map<std::string, Shader>(5);
    um_models = std::unordered_map<std::string, Model>(5);
    um_textures = std::unordered_map<std::string, unsigned int>(10);
    um_cameras = std::unordered_map<std::string, Camera>(1);
}

Asset::~Asset() {
    for (auto it : um_lights) {
        delete (it.second);
        it.second = nullptr;
    }

    for (auto it : um_meshes) {
        delete (it.second);
        it.second = nullptr;
    }

    std::cout << "全局资源析构结束!" << std::endl;
}

void Asset::Init()
{
    // Shader加载
    std::string vsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/Forward/model.vs";
    std::string fsPath = "D:/C++Pro/vscode/LearnOpenGL/src/shader/Forward/model.fs";
    this->um_shaders.emplace(std::make_pair(std::string("model_basic"), Shader(vsPath.c_str(), fsPath.c_str())));

    // Model加载
    std::string modelPath = "D:/C++Pro/vscode/LearnOpenGL/data/nanosuit/nanosuit.obj ";
    this->um_models.emplace(std::make_pair(std::string("Nanosuit"), Model(modelPath)));

    // Skybox加载
    std::vector<std::string> faces{
        "D:/C++Pro/vscode/LearnOpenGL/texture/skybox/skybox/right.jpg",
        "D:/C++Pro/vscode/LearnOpenGL/texture/skybox/skybox/left.jpg",
        "D:/C++Pro/vscode/LearnOpenGL/texture/skybox/skybox/top.jpg",
        "D:/C++Pro/vscode/LearnOpenGL/texture/skybox/skybox/bottom.jpg",
        "D:/C++Pro/vscode/LearnOpenGL/texture/skybox/skybox/front.jpg",
        "D:/C++Pro/vscode/LearnOpenGL/texture/skybox/skybox/back.jpg"};
    this->um_textures.emplace(std::make_pair(std::string("BlueSky"), loadSkybox(faces)));

    // 纹理加载
    this->um_textures.emplace(std::make_pair(std::string("Container"), loadTexture("D:/C++Pro/vscode/LearnOpenGL/texture/container2.png")));

    // Camera加载
    this->um_cameras.emplace(std::make_pair(std::string("Main"), Camera(0.0f, 10.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f)));

    // Light加载
    this->um_lights.emplace("DirectionLight1", new DirectionLight());
    this->um_lights.emplace("PointLight1", new PointLight());
    this->um_lights.emplace("SpotLight1", new SpotLight());

    // Mesh加载
    this->um_meshes.emplace("ScreenQuad", new Quad());
    this->um_meshes.emplace("Cube", new Cube());
    this->um_meshes.emplace("Sphere", new Sphere());
    this->um_meshes.emplace("Floor", new Floor());
}