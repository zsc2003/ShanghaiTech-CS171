#include "integrator.h"
#include "utils.h"
#include <omp.h>

#include <utility>
#include <iostream>
#include <vector>

std::vector<Vec2f> sample_per_pixel(int sample_times)
{
  float sample_time = sqrt((float)sample_times);
  std::vector<Vec2f> spp;
  float delta = 1.0f / sample_time;
  for (int i = 0; i < sample_time; ++i)
    for (int j = 0; j < sample_time; ++j)
      spp.push_back(Vec2f(i*delta, j*delta));
  return spp;
}

Integrator::Integrator(std::shared_ptr<Camera> cam,
                       std::shared_ptr<Scene> scene, int spp, int max_depth)
    : camera(std::move(cam)), scene(std::move(scene)), spp(spp), max_depth(max_depth) {
}

void Integrator::render() const {
  Vec2i resolution = camera->getImage()->getResolution();
  int cnt = 0;
  Sampler sampler;
  std::vector<Vec2f> sample = sample_per_pixel(this->spp);
  std::cout<<"this->spp = "<<this->spp<<'\n';
  std::cout<<"this->max_depth = "<<this->max_depth<<'\n';

  

#pragma omp parallel for schedule(dynamic), shared(cnt), private(sampler)
  for (int dy = 0; dy < resolution.y(); dy++) {
#pragma omp atomic
    ++cnt;
    printf("\r%.02f%%", cnt * 100.0 / resolution.x());
    sampler.setSeed(omp_get_thread_num());
    // std::cout<<sampler.get1D()<<'\n';
    // std::cout<<sampler.get1D()<<'\n';

    for (int dx = 0; dx < resolution.x(); dx++) {
      Vec3f L(0, 0, 0);
      // TODO: generate #spp rays for each pixel and use Monte Carlo integration to compute radiance.

      // for (auto delta : sample)
      // {
      //   Ray ray = this->camera->generateRay(dx + delta.x(), dy + delta.y());
      //   L += this->radiance(ray, sampler) / (float)this->spp;
      // }
      for (int s = 0; s < this->spp; ++s) {
        auto delta = sampler.get2D();
        Ray ray = this->camera->generateRay(dx + delta.x(), dy + delta.y());
        L += this->radiance(ray, sampler) / (float)this->spp;
      }
      camera->getImage()->setPixel(dx, dy, L);
    }
  }
}

Vec3f Integrator::radiance(Ray &ray, Sampler &sampler) const {
  Vec3f L(0, 0, 0);
  Vec3f beta(1, 1, 1);
  bool isDelta = false;
  //whether the bsdf is perfect transparent or perfect reflection
  
  Interaction inter;
  Ray ray_now = ray;
  for (int i = 0; i < max_depth; ++i) {
    /// Compute radiance (direct + indirect)
    if(!this->scene->intersect(ray_now, inter))
      break;
    
    inter.wo = -ray_now.direction;
    if(inter.type == Interaction::Type::LIGHT)
    {
      // puts("intersection with light");
      // L += this->scene->getLight()->emission(inter.pos, inter.wo);
      if (i == 0)// || isDelta)
      {
        L += beta.cwiseProduct(this->scene->getLight()->emission(inter.pos, inter.wo, inter));
      }
      break;
    }

    if(inter.type == Interaction::Type::ENVIRONMENT)
    {
      // puts("intersection with environment");

      /*if (i == 0 || isDelta)
      {
        L += inter.texture_rgb;
        std::cout<< inter.texture_rgb<<'\n';
        // std::cout<<cosine * inter.texture_rgb<<'\n';
      }*/
      L += inter.texture_rgb;

      break;
    }
      
    if(inter.material == nullptr)
      break;
    // puts("111");
    isDelta = inter.material->isDelta();
    // puts("222");
    // isDelta = false;

    // puts("intersection with object");
    // std::cout<<inter.dist<<'\n';

    //direct lighting
    if(!isDelta)
      L += beta.cwiseProduct(this->directLighting(inter, sampler));

    //undirect lighting
    float brdf_pdf = inter.material->sample(inter, sampler);
    if(!isDelta)
      beta = beta.cwiseProduct(inter.material->evaluate(inter)) * inter.normal.dot(inter.wi) / brdf_pdf;
    
    inter.wi.normalize();
    ray_now.origin = inter.pos;
    ray_now.direction = inter.wi;

    // puts("222");
    //beta = beta.cwiseProduct(inter.material->evaluate(inter)) * fabs(inter.normal.dot(ray_now.direction)) / brdf_pdf;
    // std::cout<<brdf_pdf<<'\n';
    // std::cout<<beta<<'\n';
  }
  // std::cout<<L<<'\n';
  return L;
}

Vec3f Integrator::directLighting(Interaction &interaction, Sampler &sampler) const {
  Vec3f L(0, 0, 0);
  // Compute direct lighting.
  float light_pdf = 1.0f;
  Vec3f light_pos = this->scene->getLight()->sample(interaction, &light_pdf, sampler);
  Ray ray_to_light(interaction.pos, interaction.wi);
  if(this->scene->isShadowed(ray_to_light))
    return L;
  // float dis = (light_pos - interaction.pos).norm();
  Vec3f Li = this->scene->getLight()->emission(light_pos, -interaction.wi, interaction);
  Vec3f fr = interaction.material->evaluate(interaction);
  float pdf = this->scene->getLight()->pdf(interaction, light_pos);
  // L = fabs(interaction.normal.dot(interaction.wi)) * pdf * Li.cwiseProduct(fr) / light_pdf / (dis * dis);
  // L = pdf * Li.cwiseProduct(fr) / light_pdf;
  L = interaction.normal.dot(interaction.wi) * pdf * Li.cwiseProduct(fr) / light_pdf;

  // std::cout<<pdf<<' '<<light_pdf<<'\n';
  // std::cout<<dis<<'\n';
  return L;
}