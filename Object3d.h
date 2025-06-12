#ifndef OBJECT3D_H
#define OBJECT3D_H

#include <vector>
#include <glm/glm.hpp>
#include "shaderprogram.h"
#include <string>

class Object3D {
public:
    std::vector<float> v, n, c, t;
    float* vertices;
    float* normals;
    float* colors;
    float* texCoords;
    int vertexCount;

    glm::vec3 position;
    glm::vec3 rotation; // w stopniach
    glm::vec3 scale;

    GLuint tex0;
    GLuint tex1;

    Object3D();
    bool loadModel(const std::string& filename);
    void draw(ShaderProgram* sp, const glm::mat4& P, const glm::mat4& V) const;
};

#endif

