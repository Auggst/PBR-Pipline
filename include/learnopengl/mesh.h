#ifndef MESH_H
#define MESH_H
#include <string>
#include <vector>
#include <learnopengl/myutils.h>
#include <learnopengl/shader_s.h>

using std::vector;
using std::string;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

struct Texture {
    unsigned int id;
    string type;
    aiString path;
};

class Mesh {
 public:
   /* 函数 */
   Mesh() {}
   Mesh(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<Texture>& textures);
   void Draw(Shader& shader);

 public:
   /* 网格数据 */
   vector<Vertex> vertices;
   vector<unsigned int> indices;
   vector<Texture> textures;

 private:
   /* 渲染数据 */
   unsigned int VAO, VBO, EBO;
   /* 函数 */
   void setupMesh();
};

class Model {
 public:
   /* 公共函数 */
   Model(const string& path) {loadModel(path);}
   void Draw(Shader& shader);
 private:
   /* 私有函数 */
   void loadModel(const string& path);
   void processNode(aiNode *node, const aiScene *scene);
   Mesh processMesh(aiMesh *mesh, const aiScene *scene);
   vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType texType, string typeName);

   /* 模型数据 */
   vector<Mesh> meshes;
   vector<Texture> texture_loaded;
   string directory;
};

#endif
