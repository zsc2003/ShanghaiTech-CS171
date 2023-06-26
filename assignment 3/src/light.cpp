#include "light.h"

constexpr uint32_t SAMPLE_NUM = 16;

Light::Light(const Vec3f &pos, const Vec3f &color) :
    position(pos), color(color) {

}
Vec3f Light::getColor() const {
  return color;
}

SquareAreaLight::SquareAreaLight(const Vec3f &pos, const Vec3f &color, const Vec2f &dimension,
                                 const Vec3f &normal,
                                 const Vec3f &tangent) :
    Light(pos, color),
    rectangle(pos, dimension, normal, tangent) {}

std::vector<LightSample> SquareAreaLight::samples() const {
  std::vector<LightSample> samples;
  // TODO: Your code here.
  int sample_time = SAMPLE_NUM/2;
  //int sample_time = SAMPLE_NUM;

  Vec3f x_direction = this->rectangle.getTangent().normalized();
  Vec3f y_direction = this->rectangle.getNormal().cross(this->rectangle.getTangent()).normalized();

  LightSample light;
  light.color = this->color;
  for(int i = -sample_time; i <= sample_time; ++i)
    for(int j = -sample_time ; j <= sample_time; ++j)
    {
      light.position = this->position + this->rectangle.getSize().x() / (float)SAMPLE_NUM * (float)i * x_direction;
                                      + this->rectangle.getSize().y() / (float)SAMPLE_NUM * (float)j * y_direction;
      samples.push_back(light);
      
      light.position = this->position + this->rectangle.getSize().x() / (float)SAMPLE_NUM * (float)i * x_direction;
                                      - this->rectangle.getSize().y() / (float)SAMPLE_NUM * (float)j * y_direction;
      samples.push_back(light);

      light.position = this->position - this->rectangle.getSize().x() / (float)SAMPLE_NUM * (float)i * x_direction;
                                      + this->rectangle.getSize().y() / (float)SAMPLE_NUM * (float)j * y_direction;
      samples.push_back(light);
      
      light.position = this->position - this->rectangle.getSize().x() / (float)SAMPLE_NUM * (float)i * x_direction;
                                      - this->rectangle.getSize().y() / (float)SAMPLE_NUM * (float)j * y_direction;
      samples.push_back(light);
      /*light.position = this->position
                + (((float)i + 0.5f) / (float)sample_time - 0.5f) * x_direction 
                + (((float)j + 0.5f) / (float)sample_time - 0.5f) * y_direction;
      samples.push_back(light);*/
      
    }
  light.position = this->position;
  samples.push_back(light);
  return samples;
}
bool SquareAreaLight::intersect(Ray &ray, Interaction &interaction) const {
  // TODO: Your code here.
  bool intersection = this->rectangle.intersect(ray, interaction);
  if(intersection)
    interaction.type = Interaction::Type::LIGHT;
  return intersection;
}