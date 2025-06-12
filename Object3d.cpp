#include "Object3D.h"
#include "load.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Object3D::Object3D() :
    vertices(nullptr),
    normals(nullptr),
    colors(nullptr),
    texCoords(nullptr),
    vertexCount(0),
    position(0.0f),
    rotation(0.0f),
    scale(1.0f),
    tex0(0),
    tex1(0) {
}

bool Object3D::loadModel(const std::string& filename) {
    bool success = loadObj(filename.c_str(), this->v, this->n, this->c, this->t, this->vertexCount);
    if (!success) return false;

    this->vertices = this->v.data();
    this->normals = this->n.data();
    this->colors = this->c.data();
    this->texCoords = this->t.data();

    return true;
}


void Object3D::draw(ShaderProgram* sp, const glm::mat4& P, const glm::mat4& V) const {
    glm::mat4 M = glm::mat4(1.0f);
    M = glm::translate(M, position);
    M = glm::rotate(M, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    M = glm::rotate(M, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    M = glm::rotate(M, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    M = glm::scale(M, scale);

    sp->use();
    glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));

    glEnableVertexAttribArray(sp->a("vertex"));
    glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices);

    glEnableVertexAttribArray(sp->a("color"));
    glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, colors);

    glEnableVertexAttribArray(sp->a("normal"));
    glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals);

    glEnableVertexAttribArray(sp->a("texCoord0"));
    glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoords);

    glUniform1i(sp->u("textureMap0"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);

    glUniform1i(sp->u("textureMap1"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex1);

    glDrawArrays(GL_TRIANGLES, 0, vertexCount);

    glDisableVertexAttribArray(sp->a("vertex"));
    glDisableVertexAttribArray(sp->a("color"));
    glDisableVertexAttribArray(sp->a("normal"));
    glDisableVertexAttribArray(sp->a("texCoord0"));
}
