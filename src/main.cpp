#include <memory.h>
#include <iostream>
#include <learnopengl/engine.h>

int main() {
    std::shared_ptr<Engine> instance = Engine::getInstance();
    instance->Init();
    instance->CreatePBR();
    std::cout << "初始化完成！" << std::endl;
    instance->Update();
    return 0;
}