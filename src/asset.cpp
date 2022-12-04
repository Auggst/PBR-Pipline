#include <learnopengl/asset.h>

Asset::Asset() {
    um_shaders = std::unordered_map<std::string, Shader>(5);
    um_models = std::unordered_map<std::string, Model>(5);
    um_dlLights = std::unordered_map<std::string, DirectionLight>(1);
    um_ptLights = std::unordered_map<std::string, PointLight>(16);
    um_stLights = std::unordered_map<std::string, SpotLight>(1);
    um_skyboxes = std::unordered_map<std::string, std::vector<std::string>>(2);
    um_cameras = std::unordered_map<std::string, Camera>(2);
}

Asset::~Asset() {

}