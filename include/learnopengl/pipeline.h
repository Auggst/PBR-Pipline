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

enum Pipeline_TYPE
{
  FORWARDSHADING,
  DEFERREDSHADING,
  PBRSHADING
};

class Pipeline {
 public:
   Pipeline(Pipeline_TYPE _type) {type = _type;}
   virtual void Init() = 0;
   virtual void Render() = 0;
   virtual void RenderUI() = 0;
   inline Pipeline_TYPE GetID() {return this->type;}
 public:
   Pipeline_TYPE type;
   unsigned int res_tex;
};

class ForwardShading : public Pipeline {
  public:
    ForwardShading();
    void Init() override;
    void Render() override;
    void RenderUI() override;
    unsigned int getRendered() { return this->res_tex; }
  public:
    unsigned int fbo, rbo, light_tex, env_cubemap;
    float my_color[4];
    std::shared_ptr<DirectionLight> directionLight;
    std::shared_ptr<PointLight> pointLight;
    std::shared_ptr<SpotLight> spotLight;
    std::shared_ptr<Cube> cube;
    std::shared_ptr<Floor> floor;
    std::shared_ptr<Shader> mpFloor_SH;
    std::shared_ptr<Shader> mpModel_SH;
    std::shared_ptr<Shader> mpLight_SH;
    std::shared_ptr<Shader> mpSkybox_SH;
    std::shared_ptr<Model> models;
};

class DeferredShading : public Pipeline {
 public:
   DeferredShading();
   void Init() override;
   void Render() override;
   void RenderUI() override;
   unsigned int getRendered() { return this->res_tex; }
 public:
   unsigned int fbo, rbo, env_cubemap;
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

  void Init() override;
  void HDRInit(glm::mat4 &captureProjection, std::vector<glm::mat4> &captureViews);
  void IBLInit(glm::mat4 &captureProjection, std::vector<glm::mat4> &captureViews);
  void PrefilterInit(glm::mat4 &captureProjection, std::vector<glm::mat4> &captureViews);
  void BRDFInit();
  void Render() override;
  void RenderUI() override;
  unsigned int getRendered() {return this->res_tex;}
  
public:
  unsigned int fbo, rbo, hdr_tex, env_cubemap, irradiance_cubemap, prefilter_cubemap, brdf_tex, nums, spacing;
  Cube cube_screen;
  Quad quad_screen;
  std::shared_ptr<Sphere> spheres;
  std::shared_ptr<Shader> mpHDR_SH;
  std::shared_ptr<Shader> mpIBL_SH;
  std::shared_ptr<Shader> mpPrefilter_SH;
  std::shared_ptr<Shader> mpBRDF_SH;
  std::shared_ptr<Shader> mpSkybox_SH;
  std::shared_ptr<Shader> mpPBR_SH;
  std::shared_ptr<Model> models;
};

void InitFBO(unsigned int &fbo, unsigned int &rbo, unsigned int &tex, GLsizei width, GLsizei height);

void InitBaseGBuffer(GBuffer &gbuffer, GLsizei width, GLsizei height);
#endif
