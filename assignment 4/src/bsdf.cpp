#include "bsdf.h"
#include "utils.h"

#include <utility>
#include <iostream>

IdealDiffusion::IdealDiffusion(const Vec3f &color) : color(color) {}

Vec3f IdealDiffusion::evaluate(Interaction &interaction) const {
  // TODO: your implementation here.
  // return {0,0,0};
  // return this->pdf(interaction) * this->color;
  // std::cout<<this->pdf(interaction)<<' '<<INV_PI<<'\n';
  return this->color * INV_PI ;/// 2.0f;
}

float IdealDiffusion::pdf(Interaction &interaction) const {
  // TODO: your implementation here
  // s = 1/2*(4*pi*1^2) = 2pi
  return 1.0f / 2.0f / PI;
}

float IdealDiffusion::sample(Interaction &interaction, Sampler &sampler) const {
  // TODO: your implementation here
  Vec2f sample = sampler.get2D();
  // std::cout<<sample.x()<<' '<<sample.y()<<'\n';

  // theta = 1/2*arccos(1-2*s.x), phi = 2*PI*s.y
  // x = sin(theta)cos(phi)
  // y = sin(theta)sin(phi)
  // z = cos(theta) = s.x
  float theta = 0.5f * acos(1.0f - 2.0f * sample.x());
  float phi = 2*PI*sample.y();
  float x = sin(theta) * cos(phi);
  float y = sin(theta) * sin(phi);
  float z = cos(theta);
  // float r = sqrt(sample.x());
  // phi = sample.y() * 2.0f * PI;
  // float x = r*cos(phi), y = r*sin(phi), z = sqrt(1-sample.x());


  // std::cout<<x<<' '<<y<<' '<<z<<' '<<x*x+y*y+z*z<<'\n';
  Vec3f wi(x, y, z);
  wi.normalize();
  Vec3f normal(0.0f, 0.0f, 1.0f);
  Mat3f rotation = Eigen::Quaternionf::FromTwoVectors(normal, interaction.normal).toRotationMatrix();
  interaction.wi = rotation * wi;
  interaction.wi.normalize();

  //p(w) = c cos(theta) = cos(theta)/pi
  float sample_pdf = cos(theta) / PI;
  // float sample_pdf = sqrt(1-sample.x()) / PI;
  // std::cout<<sample_pdf <<' '<<sqrt(1-sample.x()) / PI<<'\n';
  // std::cout<<sample_pdf <<' '<<1.0f / 2.0f / PI<<'\n';
  return sample_pdf;
}
/// return whether the bsdf is perfect transparent or perfect reflection
bool IdealDiffusion::isDelta() const {
  return false;
}


//---------------------------------Specular---------------------------------------

Specular::Specular(const Vec3f &color) : color(color) {}

Vec3f Specular::evaluate(Interaction &interaction) const {
  return Vec3f(1.0f, 1.0f, 1.0f);
}

float Specular::pdf(Interaction &interaction) const {
  return 1.0f;
}

float Specular::sample(Interaction &interaction, Sampler &sampler) const {
  interaction.wi = - interaction.wo + 2.0f * interaction.normal.dot(interaction.wo) * interaction.normal;
  // puts("Specular sampling");
  return 1.0f;
}
/// return whether the bsdf is perfect transparent or perfect reflection
bool Specular::isDelta() const {
  return true;
}


//---------------------------------Translucent---------------------------------------

Translucent::Translucent(const Vec3f &color) : color(color) {}

Vec3f Translucent::evaluate(Interaction &interaction) const {
  return Vec3f(1.0f, 1.0f, 1.0f);
}

float Translucent::pdf(Interaction &interaction) const {
  return 1.0f;
}

float Translucent::sample(Interaction &interaction, Sampler &sampler) const {
  // puts("Translucent sampling");

  float eta_i = 1.0f , eta_t = 1.5f;
 /*if(interaction.is_inside == 0)//from outside to inside
  {
    eta_i = 1.0f;
    eta_t = 1.5f;
  }
  else
  {
    eta_i = 1.5f;
    eta_t = 1.0f;
    // interaction.normal = -interaction.normal;
  }*/ 
  interaction.wo.normalize();

  float cosine_i = interaction.wo.dot(interaction.normal);
  /*if(fabs(cosine_i - 1.0f ) < 1e-7f)
  {
    interaction.wi = - interaction.wo;
    // puts("6666666666666");
    return 1.0f;
  }*/
  if(cosine_i < 0.0f)
  {
    // interaction.normal = -interaction.normal;
    // cosine_i = interaction.wo.dot(interaction.normal);
    cosine_i = fabs(cosine_i);
    eta_i = 1.5f;
    eta_t = 1.0f;
  }

  float sine_i = sqrt(std::max(0.0f, 1.0f - cosine_i * cosine_i));
  float sine_t =  eta_i / eta_t * sine_i;
  if(sine_t >= 1.0f)
    sine_t = 1.0f;
  
  // std::cout<<sine_t<<'\n';
  float cosine_t = std::max(0.0f, sqrt(1.0f - sine_t * sine_t));

  Vec3f refract = eta_i / eta_t * (-interaction.wo) + (eta_i / eta_t * (interaction.wo.dot(interaction.normal) - cosine_t) * interaction.normal);
  if(interaction.wo.dot(interaction.normal) < 0.0f)
    refract = eta_i / eta_t * (-interaction.wo) + (eta_i / eta_t * (interaction.wo.dot(-interaction.normal) - cosine_t) * (-interaction.normal));
  refract.normalize();

  float r1 = (eta_t * cosine_i - eta_i * cosine_t) / (eta_t * cosine_i + eta_i * cosine_t);
  float r2 = (eta_i * cosine_i - eta_t * cosine_t) / (eta_i * cosine_i + eta_t * cosine_t);

  float fr = (r1 * r1 + r2 * r2) / 2.0f;
  // std::cout<<r1<<' '<<r2<<' '<<fr<<'\n';
  // std::cout<<fr<<'\n';
  // if(fr > 1.0f) { std::cout<<sine_i<<' '<<cosine_i<<' '<<sine_t<<' '<<cosine_t<<'\n'; }
  float p = sampler.get1D();
  // std::cout<<p<<'\n';
  // fr = 0.08f;
  // fr = 1.01f;
  if(p < fr)
  {
    interaction.wi = - interaction.wo + 2.0f * interaction.normal.dot(interaction.wo) * interaction.normal;
    interaction.wi.normalize();
  } 
  else
  {
    interaction.wi = refract;
    interaction.is_inside ^= 1;
  }
  return 1.0f;
}
/// return whether the bsdf is perfect transparent or perfect reflection
bool Translucent::isDelta() const {
  return true;
}