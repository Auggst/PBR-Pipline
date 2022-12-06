#ifndef MODEL_H
#define MODEL_H

#include <memory>
#include <vector>

enum MESH_TYPE {
   QUAD, SPHERE, CUBE, FLOOR
};

class Renderable {
 public:
   Renderable(MESH_TYPE _type) {this->type = _type;}
   inline MESH_TYPE getType() {return this->type;}
   virtual void Draw() = 0;
 private: 
   MESH_TYPE type;
};

class Quad : public Renderable {
 public:
    Quad();

    void Draw();
 public:
    unsigned int VAO, VBO;
};

class Sphere : public Renderable {
 public:
    Sphere();

    void Draw();

 public:
    unsigned int VAO, count, albedo_tex, normal_tex, metallic_tex, rough_tex, ao_tex;
    float mental, rough;
};

class Cube : public Renderable {
 public:
    Cube();

    void Draw();
 public:
    unsigned int VAO;
};

class Floor : public Renderable
{
public:
   Floor();
   void Draw();
 public:
   unsigned int VAO, tex, normal_tex;
};

#endif
