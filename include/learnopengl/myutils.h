#ifndef MYUTILS_H
#define MYUTILS_H

#include <iostream>
#include <vector>
#include <string>
#include <random>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// utility function for generating a Framebuffer
// ---------------------------------------------------
void genFramebuffer(unsigned int &caputreFBO, unsigned int &captureRBO, GLsizei fb_width, GLsizei fb_height);

// utility function for generating a Cubemap
// ---------------------------------------------------
void genCubeMap(unsigned int &envCubemap, GLsizei cube_width, GLsizei cube_height);

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const *path);

// utility function for loading a HDR texture from file
// ---------------------------------------------------
unsigned int loadHDR(char const *path);

// utility function for loading a Skybox texture from file
// ---------------------------------------------------
unsigned int loadSkybox(std::vector<std::string> &faces);

// utility function for random float number
// ---------------------------------------------------
float random1f(float minf, float maxf);

// 初始化深度FBO的功能函数
// ---------------------------------------------------
void InitDepthFBO(unsigned int &fbo, unsigned int &rbo, unsigned int &tex, GLsizei width, GLsizei height);

// 绑定纹理
// ---------------------------------------------------
void BindDepthTex(unsigned int &fbo, unsigned int &tex, GLsizei width, GLsizei height);

// 计算切线与副切线
// ---------------------------------------------------
void CalTanANDBitan(float *vertices, int numbers, float *tangent);
#endif