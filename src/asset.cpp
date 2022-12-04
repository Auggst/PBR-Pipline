#include <learnopengl/asset.h>

Asset::Asset() {
    um_shaders = std::unordered_map<std::string, Shader>(5);
    um_models = std::unordered_map<std::string, Model>(5);
    um_skyboxes = std::unordered_map<std::string, unsigned int>(2);
    um_cameras = std::unordered_map<std::string, Camera>(1);
    vec_DL = std::vector<DirectionLight>(1);
    vec_PL = std::vector<PointLight>(1);
    vec_SL = std::vector<SpotLight>(1);
    
}

Asset::~Asset() {

}

void Asset::Init()
{
    // Shader加载
    std::string vsPath = "D:\\C++Pro\\vscode\\LearnOpenGL\\src\\shader\\Forward\\model.vs";
    std::string fsPath = "D:\\C++Pro\\vscode\\LearnOpenGL\\src\\shader\\Forward\\model.fs";
    this->um_shaders.emplace(std::make_pair(std::string("model_basic"), Shader(vsPath.c_str(), fsPath.c_str())));

    // Model加载
    std::string modelPath = "D:/C++Pro/vscode/LearnOpenGL/data/nanosuit/nanosuit.obj ";
    this->um_models.emplace(std::make_pair(std::string("nanosuit"), Model(modelPath)));

    // Skybox加载
    std::vector<std::string> faces{
        "D:/C++Pro/vscode/LearnOpenGL/texture/skybox/skybox/right.jpg",
        "D:/C++Pro/vscode/LearnOpenGL/texture/skybox/skybox/left.jpg",
        "D:/C++Pro/vscode/LearnOpenGL/texture/skybox/skybox/top.jpg",
        "D:/C++Pro/vscode/LearnOpenGL/texture/skybox/skybox/bottom.jpg",
        "D:/C++Pro/vscode/LearnOpenGL/texture/skybox/skybox/front.jpg",
        "D:/C++Pro/vscode/LearnOpenGL/texture/skybox/skybox/back.jpg"};
    this->um_skyboxes.emplace(std::make_pair(std::string("BlueSky"), loadSkybox(faces)));

    // Camera加载
    this->um_cameras.emplace(std::make_pair(std::string("Main"), Camera(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f)));

    // Light加载
    this->vec_DL.emplace_back(DirectionLight());
    this->vec_PL.emplace_back(PointLight());
    this->vec_SL.emplace_back(SpotLight());

    // Mesh加载
    this->vec_Quad.emplace_back(Quad());
    this->vec_Cube.emplace_back(Cube());
    this->vec_Sphere.emplace_back(Sphere());
}