#ifndef _object_H_
#define _object_H_
#include "defines.h"
#include <shader.h>
#include <vector>

struct Vertex {
  vec3 position;
  vec3 normal;
};

struct DrawMode {
  GLenum primitive_mode;
};

class Object {
 private:
  unsigned int VAO;
  unsigned int VBO;
  unsigned int EBO;

 public:
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  std::vector<std::vector<vec3>> control_points;

  DrawMode draw_mode;

  Object();
  ~Object();

  void init();

  void drawArrays() const;
  void drawArrays(const Shader& shader) const;
  void drawElements() const;
  void drawElements(const Shader& shader) const;
};
#endif