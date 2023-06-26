#include "camera.h"
#include <iostream>

Camera::Camera()
    : position(0, -1, 0), fov(45), focal_len(1)
{
  lookAt({0, 0, 0}, {0, 0, 1});
}

Ray Camera::generateRay(float dx, float dy) {
  // TODO: Your code here
  // You need to generate ray according to screen coordinate (dx, dy)
  // return Ray(Vec3f(0,0,0), Vec3f(0,0,0));
  float width = this->getImage()->getResolution().x(), height = this->getImage()->getResolution().y();
  Vec2f screen_coord = Vec2f(dx, dy);
  Vec3f screen_center = this->position + focal_len * this->forward;
  
  // std::cout<<position.x()<<' '<<position.y()<<' '<<position.z()<<' '<<forward.x()<<' '<<forward.y()<<' '<<forward.z()<<'\n';
  // aspect ratio = 400 / 400 = 1
  float ratio = this->getImage()->getResolution().x() / this->getImage()->getResolution().y();
  Vec3f world_coord = screen_center 
                    + ((2 * screen_coord.x() / width - 1) * ratio * focal_len * tanf(fov * PI / 180.0f / 2.0f)) * right
                    + ((2 * screen_coord.y() / height - 1) * focal_len * tanf(fov * PI / 180.0f / 2.0f) * focal_len) * up;
  Vec3f direction = world_coord - position;
  // float ratio = tan(this->fov/(float)2.0)*focal_len*(float)2.0/height;  // world_height/screen_height

  return Ray(this->position, direction.normalized());
}

void Camera::lookAt(const Vec3f &look_at, const Vec3f &ref_up) {
  // TODO: Your code here
  this->forward = look_at - this->position;
  this->forward.normalize();
  this->right = this->forward.cross(ref_up).normalized();
  this->up = this->right.cross(this->forward).normalized();
}

void Camera::setPosition(const Vec3f &pos) {
  position = pos;
}
Vec3f Camera::getPosition() const {
  return position;
}
void Camera::setFov(float new_fov) {
  fov = new_fov;
}
float Camera::getFov() const {
  return fov;
}

void Camera::setImage(std::shared_ptr<ImageRGB> &img) {
  image = img;
}
std::shared_ptr<ImageRGB> &Camera::getImage() {
  return image;
}
