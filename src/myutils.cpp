#include <learnopengl/myutils.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

// utility function for generating a Framebuffer
// ---------------------------------------------------
void genFramebuffer(unsigned int &caputreFBO, unsigned int &captureRBO, GLsizei fb_width, GLsizei fb_height)
{
    glGenFramebuffers(1, &caputreFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, caputreFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fb_width, fb_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
}

// utility function for generating a Cubemap
// ---------------------------------------------------
void genCubeMap(unsigned int &envCubemap, GLsizei cube_width, GLsizei cube_height)
{
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, cube_width, cube_height, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// utility function for loading a HDR texture from file
// ---------------------------------------------------
unsigned int loadHDR(char const *path)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float *data = stbi_loadf(path, &width, &height, &nrComponents, 0);
    unsigned int hdrTexture;
    if (data)
    {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load HDR IMAGE." << std::endl;
        return 0;
    }
    return hdrTexture;
}

// utility function for loading a Skybox texture from file
unsigned int loadSkybox(std::vector<std::string>& faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

// utility function for random float number
float random1f(float minf, float maxf) {
    std::random_device seed;
    std::mt19937 eng(seed());
    std::uniform_real_distribution<> distrib(minf, maxf);
    return distrib(eng);
}

// 初始化深度FBO的功能函数
void InitDepthFBO(unsigned int &fbo, unsigned int &rbo, unsigned int &tex, GLsizei width, GLsizei height) {
    genFramebuffer(fbo, rbo, width, height);

    glGenTextures(1, &(tex));

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// 绑定深度纹理
void BindDepthTex(unsigned int &fbo, unsigned int &tex, GLsizei width, GLsizei height) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glGenTextures(1, &(tex));

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// 计算每个三角形的切线和副切线
std::vector<glm::vec3> CalTan(std::vector<glm::vec3> &pos,std::vector<glm::vec2> &uv) {
    std::vector<glm::vec3> result;
    glm::vec3 edge1 = pos[1] - pos[0];
    glm::vec3 edge2 = pos[2] - pos[0];
    glm::vec2 deltaUV1 = uv[1] - uv[0];
    glm::vec2 deltaUV2 = uv[2] - uv[0];
    GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    glm::vec3 tangent1;
    tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent1 = glm::normalize(tangent1);
    result.emplace_back(tangent1);

    glm::vec3 bitangent1;
    bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent1 = glm::normalize(bitangent1);
    result.emplace_back(bitangent1);
    return result;
}

// 计算切线与副切线
void CalTanANDBitan(float *vertices, int numbers, float *tangent) {
    std::vector<glm::vec3> pos(3);
    std::vector<glm::vec2> uv(3);
    for (int i = 0; i < numbers; i++) {
        pos[i % 3] = glm::vec3(*(vertices + 0 + i * 8), *(vertices + 1 + i * 8), *(vertices + 2 + i * 8));
        uv[i % 3] = glm::vec2(*(vertices + 6 + i * 8), *(vertices + 7 + i * 8));
        if ((i + 1) % 3 == 0) {
            std::vector<glm::vec3> temp = CalTan(pos, uv);
            *(tangent + 0 + (i - 2) * 6) = temp[0].x;
            *(tangent + 1 + (i - 2) * 6) = temp[0].y;
            *(tangent + 2 + (i - 2) * 6) = temp[0].z;
            *(tangent + 3 + (i - 2) * 6) = temp[1].x;
            *(tangent + 4 + (i - 2) * 6) = temp[1].y;
            *(tangent + 5 + (i - 2) * 6) = temp[1].z;
            *(tangent + 6 + (i - 2) * 6) = temp[0].x;
            *(tangent + 7 + (i - 2) * 6) = temp[0].y;
            *(tangent + 8 + (i - 2) * 6) = temp[0].z;
            *(tangent + 9 + (i - 2) * 6) = temp[1].x;
            *(tangent + 10 + (i - 2) * 6) = temp[1].y;
            *(tangent + 11 + (i - 2) * 6) = temp[1].z;
            *(tangent + 12 + (i - 2) * 6) = temp[0].x;
            *(tangent + 13 + (i - 2) * 6) = temp[0].y;
            *(tangent + 14 + (i - 2) * 6) = temp[0].z;
            *(tangent + 15 + (i - 2) * 6) = temp[1].x;
            *(tangent + 16 + (i - 2) * 6) = temp[1].y;
            *(tangent + 17 + (i - 2) * 6) = temp[1].z;
        }
    }
}

