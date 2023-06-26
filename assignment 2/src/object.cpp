#include <object.h>
#include <shader.h>
#include <utils.h>
#include <fstream>
#include <vector>

Object::Object() {}
Object::~Object() {}

/**
 * TODO: initialize VAO, VBO, VEO and set the related varialbes
 */
void Object::init() {
    glGenVertexArrays(1, &this->VAO);// generate vertex array
    glGenBuffers(1, &this->VBO);// 顶点缓冲对象 Vertex Buffer Objects
    glGenBuffers(1, &this->EBO);

    glBindVertexArray(this->VAO); //bind to the state machine
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO); //bind to the state machine (above vao)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // 顶点位置
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // 顶点法线
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

}

/**
* 
 * TODO: draw object with VAO and VBO
 * You can choose to implement either one or both of the following functions.
 */

/* Implement this one if you do not use a shader */
void Object::drawArrays() const {
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glBindVertexArray(0);

  //  for(int i=0;i< draw_vector.size();++i)
    //    std::cout<< draw_vector[i].
    // std::cout << vertices.size()<<'\n';

}

/* Implement this one if you do use a shader */
void Object::drawArrays(const Shader& shader) const {
    // !DELETE THIS WHEN YOU FINISH THIS FUNCTION
    UNIMPLEMENTED;
}

/**
 * TODO: draw object with VAO, VBO, and VEO
 * You can choose to implement either one or both of the following functions.
 */

/* Implement this one if you do not use a shader */
void Object::drawElements() const {
  // !DELETE THIS WHEN YOU FINISH THIS FUNCTION
  UNIMPLEMENTED;
}

/* Implement this one if you do use a shader */
void Object::drawElements(const Shader& shader) const {
    UNIMPLEMENTED;

}