#ifndef PIPELINE_H
#define PIPELINE_H

#include <memory>
#include <vector>
#include <string>

#include <learnopengl/shader_s.h>
#include <learnopengl/model.h>
#include <learnopengl/camera.h>
#include <learnopengl/mesh.h>
#include <learnopengl/light.h>

struct GBuffer {
  GLuint gBuffer;
  GLuint gPosition;
  GLuint gNormal;
  GLuint gColor;
  GLuint gDepthRBO;
};

class Pipeline {
 public:
   virtual void init() = 0;
   virtual void render() = 0;
   virtual void renderUI() = 0;
};

class ForwardShading : public Pipeline {
  public:
    ForwardShading();
    void init() override;
    void render() override;
    void renderUI() override;
    unsigned int getRendered() { return this->res_tex; }
  public:
    unsigned int res_tex, fbo, rbo, light_tex, env_cubemap;
    float my_color[4];
    std::shared_ptr<DirectionLight> directionLight;
    std::shared_ptr<PointLight> pointLight;
    std::shared_ptr<SpotLight> spotLight;
    std::shared_ptr<Cube> cube;
    std::shared_ptr<Shader> mpModel_SH;
    std::shared_ptr<Shader> mpLight_SH;
    std::shared_ptr<Shader> mpSkybox_SH;
    std::shared_ptr<Model> models;
};

class DeferredShading : public Pipeline {
 public:
   DeferredShading();
   void init() override;
   void render() override;
   void renderUI() override;
   unsigned int getRendered() { return this->res_tex; }
 public:
   unsigned int res_tex, fbo, rbo, env_cubemap;
   GBuffer gBuffer;
   std::shared_ptr<DirectionLight> directionLight;
   std::shared_ptr<PointLight> pointLight;
   std::shared_ptr<SpotLight> spotLight;
   std::shared_ptr<Cube> cube;
   std::shared_ptr<Quad> quad;
   std::shared_ptr<Shader> mpGeometry_SH;
   std::shared_ptr<Shader> mpLight_SH;
   std::shared_ptr<Shader> mpSkybox_SH;
   std::shared_ptr<Shader> mpModel_SH;
   std::shared_ptr<Model> models;
};

class PBR : public Pipeline {
 public:
  PBR();

  void init() override;
  void HDRInit(glm::mat4 &captureProjection, std::vector<glm::mat4> &captureViews);
  void IBLInit(glm::mat4 &captureProjection, std::vector<glm::mat4> &captureViews);
  void PrefilterInit(glm::mat4 &captureProjection, std::vector<glm::mat4> &captureViews);
  void BRDFInit();
  void render() override;
  void renderUI() override;
  unsigned int getRendered() {return this->res_tex;}
  
public:
  unsigned int fbo, rbo, res_tex, hdr_tex, env_cubemap, irradiance_cubemap, prefilter_cubemap, brdf_tex, nums, spacing;
  Cube cube_screen;
  Quad quad_screen;
  std::shared_ptr<Sphere> spheres;
  std::shared_ptr<Shader> HDR_SH;
  std::shared_ptr<Shader> IBL_SH;
  std::shared_ptr<Shader> Prefilter_SH;
  std::shared_ptr<Shader> BRDF_SH;
  std::shared_ptr<Shader> Skybox_SH;
  std::shared_ptr<Shader> PBR_SH;
  std::shared_ptr<Model> models;
};

void InitFBO(unsigned int &fbo, unsigned int &rbo, unsigned int &tex, GLsizei width, GLsizei height);

void InitBaseGBuffer(GBuffer &gbuffer, GLsizei width, GLsizei height);
#endif
