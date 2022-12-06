#include <learnopengl/light.h>

AbstractLight::~AbstractLight() {

}

DirectionLight::DirectionLight() : AbstractLight(LIGHT_TYPE::DIRECTION) {
    this->direction = glm::vec3(0.0f, 0.0f, -1.0f);
    this->ambient = glm::vec3(random1f(0.0f, 0.001));
    this->diffuse = glm::vec3(random1f(0.1f, 0.5), random1f(0.1f, 0.5), random1f(0.1f, 0.5));
    this->specular = glm::vec3(random1f(0.5f, 1.0), random1f(0.5f, 1.0), random1f(0.5f, 1.0));
}

DirectionLight::~DirectionLight() {

}

void DirectionLight::SendToShader(std::shared_ptr<Shader> sh, int index) {
    sh->use();
    sh->setVec3("dtLight[" + std::to_string(index) + "].direction", this->direction);
    sh->setVec3("dtLight[" + std::to_string(index) + "].ambient", this->ambient);
    sh->setVec3("dtLight[" + std::to_string(index) + "].diffuse", this->diffuse);
    sh->setVec3("dtLight[" + std::to_string(index) + "].specular", this->specular);
}

PointLight::PointLight() : AbstractLight(LIGHT_TYPE::POINTLIGHT) {
    this->position = glm::vec3(0.0f, 10.0f, 0.0f);
    this->ambient = glm::vec3(random1f(0.01f, 0.05f));
    this->diffuse = glm::vec3(random1f(0.2, 0.5));
    this->specular = glm::vec3(random1f(0.5, 1.0));
    this->kc = 1.0f;
    this->kl = 0.09f;
    this->kq = 0.032f;
}

PointLight::~PointLight() {

}

void PointLight::SendToShader(std::shared_ptr<Shader> sh, int index) {
    sh->use();
    sh->setVec3("ptLight[" + std::to_string(index) + "].position", this->position);
    sh->setVec3("ptLight[" + std::to_string(index) + "].ambient", this->ambient);
    sh->setVec3("ptLight[" + std::to_string(index) + "].diffuse", this->diffuse);
    sh->setVec3("ptLight[" + std::to_string(index) + "].specular", this->specular);
    sh->setFloat("ptLight[" + std::to_string(index) + "].kc", this->kc);
    sh->setFloat("ptLight[" + std::to_string(index) + "].kl", this->kl);
    sh->setFloat("ptLight[" + std::to_string(index) + "].kq", this->kq);
}

SpotLight::SpotLight() : AbstractLight(LIGHT_TYPE::SPOTLIGHT), kc(1.0f), kl(0.09f), kq(0.032f)
{
    this->position = glm::vec3(0.0f);
    this->direction = glm::vec3(0.0f, 0.0f, -1.0f);
    this->ambient = glm::vec3(random1f(0.01f, 0.05f));
    this->diffuse = glm::vec3(random1f(0.2, 0.5));
    this->specular = glm::vec3(random1f(0.5, 1.0));
    this->cutOff = glm::cos(glm::radians(12.5f));
    this->outerCutOff = glm::cos(glm::radians(17.5f));
}

SpotLight::~SpotLight() {
    
}

void SpotLight::SendToShader(std::shared_ptr<Shader> sh, int index) {
    sh->use();
    sh->setVec3("spLight[" + std::to_string(index) + "].position", this->position);
    sh->setVec3("spLight[" + std::to_string(index) + "].direction", this->direction);
    sh->setVec3("spLight[" + std::to_string(index) + "].ambient", this->ambient);
    sh->setVec3("spLight[" + std::to_string(index) + "].diffuse", this->diffuse);
    sh->setVec3("spLight[" + std::to_string(index) + "].specular", this->specular);
    sh->setFloat("spLight[" + std::to_string(index) + "].cutOff", this->cutOff);
    sh->setFloat("spLight[" + std::to_string(index) + "].outerCutOff", this->outerCutOff);
    sh->setFloat("spLight[" + std::to_string(index) + "].kc", this->kc);
    sh->setFloat("spLight[" + std::to_string(index) + "].kl", this->kl);
    sh->setFloat("spLight[" + std::to_string(index) + "].kq", this->kq);
}

