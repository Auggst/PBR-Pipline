#ifndef MODEL_H
#define MODEL_H

#include <memory>
#include <vector>

class Renderable
{
    virtual void draw() = 0;
};

class Quad : public Renderable {
 public:
    Quad();

    void draw();
 public:
    unsigned int VAO;
};

class Sphere : public Renderable {
 public:
    Sphere();

    void draw();

 public:
    unsigned int VAO, count, albedo_tex, normal_tex, metallic_tex, rough_tex, ao_tex;
    float mental, rough;
};

class Cube : public Renderable {
 public:
    Cube();

    void draw();
 public:
    unsigned int VAO;
};

#endif
