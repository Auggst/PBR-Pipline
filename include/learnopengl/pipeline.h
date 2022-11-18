#ifndef PIPELINE_H
#define PIPELINE_H

#include <memory>
#include <vector>
#include <string>

#include <learnopengl/shader_s.h>
#include <learnopengl/model.h>
#include <learnopengl/camera.h>

class Pipeline {
public:
  virtual void init() = 0;
  virtual void render() = 0;
  virtual void renderUI() = 0;
};

class DeferredShading : public Pipeline {
public:
  void init();
  void render();
  void renderUI();
};

class PBR : public Pipeline {
 public:
  PBR();
  
  void init();
  void HDRInit(glm::mat4 &captureProjection, std::vector<glm::mat4> &captureViews);
  void IBLInit(glm::mat4 &captureProjection, std::vector<glm::mat4> &captureViews);
  void PrefilterInit(glm::mat4 &captureProjection, std::vector<glm::mat4> &captureViews);
  void BRDFInit();
  void render();
  void renderUI();
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
};

#endif
