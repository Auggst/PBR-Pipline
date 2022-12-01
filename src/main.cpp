#include <memory.h>
#include <iostream>
#include <learnopengl/engine.h>

int main() {
    std::shared_ptr<Engine> instance = Engine::getInstance();
    instance->Init();
    //instance->CreatePBR();
    // instance->CreateForwardS();
    instance->CreateDeferredS();
    instance->Update();
    return 0;
}