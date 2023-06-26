#include "mesh.h"
#include "object.h"
#include "transform.h"

Object::Object(std::shared_ptr<Mesh> mesh,
               std::shared_ptr<Shader> shader) :
  mesh(std::move(mesh)),
  shader(std::move(shader)),
  transform(std::make_unique<Transform>()),
  color(0, 0, 0) {
}

Object::Object(std::shared_ptr<Mesh> mesh,
               std::shared_ptr<Shader> shader,
               const Transform& transform) :
  mesh(std::move(mesh)),
  shader(std::move(shader)),
  transform(std::make_unique<Transform>(transform)),
  color(0, 0, 0) {
}

void Object::FixedUpdate(Vec3 mouse, bool left, bool mid) const {
  if (mesh)
    mesh->FixedUpdate(mouse, left, mid);
}

bool Object::DragUpdate(Vec3 mouse, Vec3 CameraPos)
{
  if (mesh)
    return this->mesh->DragUpdate(mouse, CameraPos);
  return false;
}

void Object::move_fix(Vec3 mouse, bool middle, Vec3 CameraPos)
{
  if (mesh)
    this->mesh->move_fix(mouse, middle, CameraPos);
}
