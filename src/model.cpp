#include <vector>

#include <learnopengl/model.h>
#include <learnopengl/myutils.h>

Quad::Quad() : Renderable(MESH_TYPE::QUAD) {
    float quadVertices[] = {
        // positions          // texture Coords
        -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        -1.0f, -1.0f,  0.0f,  0.0f,  0.0f,
         1.0f, -1.0f,  0.0f,  1.0f,  0.0f,

        -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         1.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    };
    // setup plane VAO
    glGenVertexArrays(1, &(this->VAO));
    glGenBuffers(1, &(this->VBO));
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Quad::Draw() {
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

Sphere::Sphere() : Renderable(MESH_TYPE::SPHERE) {
    glGenVertexArrays(1, &(this->VAO));

    unsigned int vbo, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    const float PI = 3.14159265359f;
    for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
    {
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

            positions.push_back(glm::vec3(xPos, yPos, zPos));
            uv.push_back(glm::vec2(xSegment, ySegment));
            normals.push_back(glm::vec3(xPos, yPos, zPos));
        }
    }

    bool oddRow = false;
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
    {
        if (!oddRow) // even rows: y == 0, y == 2; and so on
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                indices.push_back(y * (X_SEGMENTS + 1) + x);
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        }
        else
        {
            for (int x = X_SEGMENTS; x >= 0; --x)
            {
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.push_back(y * (X_SEGMENTS + 1) + x);
            }
        }
        oddRow = !oddRow;
    }
    count = static_cast<unsigned int>(indices.size());

    std::vector<float> data;
    for (unsigned int i = 0; i < positions.size(); ++i)
    {
        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
        if (normals.size() > 0)
        {
            data.push_back(normals[i].x);
            data.push_back(normals[i].y);
            data.push_back(normals[i].z);
        }
        if (uv.size() > 0)
        {
            data.push_back(uv[i].x);
            data.push_back(uv[i].y);
        }
    }
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    unsigned int stride = (3 + 2 + 3) * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *)(6 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    this->albedo_tex = loadTexture("D:\\C++Pro\\vscode\\LearnOpenGL\\texture\\rusted-steel-unity\\rusted-steel_albedo.png");
    this->normal_tex = loadTexture("D:\\C++Pro\\vscode\\LearnOpenGL\\texture\\rusted-steel-unity\\rusted-steel_normal-ogl.png");
    this->metallic_tex = loadTexture("D:\\C++Pro\\vscode\\LearnOpenGL\\texture\\rusted-steel-unity\\rustediron2_metallic.png");
    this->rough_tex = loadTexture("D:\\C++Pro\\vscode\\LearnOpenGL\\texture\\rusted-steel-unity\\rustediron2_roughness.png");
    this->ao_tex = loadTexture("D:\\C++Pro\\vscode\\LearnOpenGL\\texture\\rusted-steel-unity\\rusted-steel_ao.png");

    this->mental = 0.5f;
    this->rough = 0.5f;
}

void Sphere::Draw() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->albedo_tex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->normal_tex);
    // glActiveTexture(GL_TEXTURE2);
    //glBindTexture(GL_TEXTURE_2D, this->metallic_tex);
    // glActiveTexture(GL_TEXTURE3);
    // glBindTexture(GL_TEXTURE_2D, this->rough_tex);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, this->ao_tex);
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLE_STRIP, this->count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Cube::Cube() : Renderable(MESH_TYPE::CUBE) {

    float vertices[] = {
        // back face
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
        1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // bottom-right
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  // top-left
        // front face
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
        1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
        -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
        // left face
        -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
        -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-left
        -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
        -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
                                                            // right face
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
        1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,    // top-right
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
        1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,    // bottom-left
        // bottom face
        -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
        1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // top-left
        1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
        1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
        -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
        -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
        // top face
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
        1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
        -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f   // bottom-left
    };
    unsigned int vbo;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &vbo);
    // fill buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // link vertex attributes
    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Cube::Draw() {
    // render Cube
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

Floor::Floor() : Renderable(MESH_TYPE::FLOOR) {
    float vertices[] = {
        // back face
        -50.0f, -50.0f, -50.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        50.0f, 0.0f, -50.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
        50.0f, -50.0f, -50.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // bottom-right
        50.0f, 0.0f, -50.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
        -50.0f, -50.0f, -50.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -50.0f, 0.0f, -50.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  // top-left

        // front face
        -50.0f, -50.0f, 50.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
        50.0f, -50.0f, 50.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
        50.0f, 0.0f, 50.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
        50.0f, 0.0f, 50.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
        -50.0f, 0.0f, 50.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
        -50.0f, -50.0f, 50.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
        // left face
        -50.0f, 0.0f, 50.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
        -50.0f, 0.0f, -50.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-left
        -50.0f, -50.0f, -50.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -50.0f, -50.0f, -50.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -50.0f, -50.0f, 50.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
        -50.0f, 0.0f, 50.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right

        // right face
        50.0f, 0.0f, 50.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
        50.0f, -50.0f, -50.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
        50.0f, 0.0f, -50.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,    // top-right
        50.0f, -50.0f, -50.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
        50.0f, 0.0f, 50.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
        50.0f, -50.0f, 50.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,    // bottom-left

        // bottom face
        -50.0f, -50.0f, -50.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
        50.0f, -50.0f, -50.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // top-left
        50.0f, -50.0f, 50.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
        50.0f, -50.0f, 50.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
        -50.0f, -50.0f, 50.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
        -50.0f, -50.0f, -50.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right

        // top face
        -50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
        50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
        50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
        50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
        -50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
        -50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f   // bottom-left
    };

    float tangent[36 * 6] = {0.0};
    CalTanANDBitan(vertices, 36, tangent);

    unsigned int vbo, tvbo;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &tvbo);
    /*
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(DATA_VAO), &(data[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DATA_VAO), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(DATA_VAO), (void *)offsetof(DATA_VAO, DATA_VAO::Normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(DATA_VAO), (void *)offsetof(DATA_VAO, DATA_VAO::TexCoords));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(DATA_VAO), (void *)offsetof(DATA_VAO, DATA_VAO::Tangent));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(DATA_VAO), (void *)offsetof(DATA_VAO, DATA_VAO::Bitangent));
    */
    // fill buffer
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // link vertex attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, tvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tangent), &(tangent[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glBindVertexArray(0);

    this->tex = loadTexture("D:/C++Pro/vscode/LearnOpenGL/texture/brickwall.jpg");
    this->normal_tex = loadTexture("D:/C++Pro/vscode/LearnOpenGL/texture/brickwall_normal.jpg");
}

void Floor::Draw() {
    // render Floor
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->tex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->normal_tex);
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
