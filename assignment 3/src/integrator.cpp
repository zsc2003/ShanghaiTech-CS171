#include "integrator.h"
#include <omp.h>
#include <iostream>
#include <utility>
#include <vector>
#include <cstdlib>

std::vector<std::pair<float,float> > simple()
{
  std::vector< std::pair<float,float> > mo;
  mo.push_back(std::make_pair(0.5f,0.5f));
  return mo;
}

std::vector<std::pair<float,float> > multi_sample()
{
  std::vector< std::pair<float,float> > mo;
  for(int i=0;i<4;++i)
    for(int j=0;j<4;++j)
      mo.push_back(std::make_pair(0.1f + 0.25f * (float)i, 0.1f + 0.25f * (float)j));
  return mo;
}

std::vector<std::pair<float,float> > rotation(float angle)
{
  std::vector< std::pair<float,float> > m = multi_sample();
  std::vector< std::pair<float,float> > mo;
  Eigen::Matrix2f R;
  angle = -angle / 180.0f;
  R<< cos(angle), -sin(angle),
      sin(angle), cos(angle);
  for(auto d : m)
  {
      Vec2f dd(d.first,d.second);
      dd = R * dd;
      dd.x() += 0.16f;
      dd.y() += 0.16f;
      if(dd.x() < 0.0f)
        dd.x() = 0.0f;
      if(dd.y() < 0.0f)
        dd.y() = 0.0f;
      mo.push_back(std::make_pair(dd.x(), dd.y()));
      // std::cout<<dd.x()<<' '<<dd.y()<<'\n';
  }
  return mo;
}

std::vector<std::pair<float,float> > random_sample()
{
  std::vector< std::pair<float,float> > mo;
  srand(19260817);
  
  for(int i=1;i<=16;++i)
  {
    float dx = (float)rand() / (float)RAND_MAX;
    float dy = (float)rand() / (float)RAND_MAX;
    //std::cout<<dx<<' '<<dy<<'\n';
    mo.push_back(std::make_pair(dx, dy));
  }
  
  return mo;
}

std::vector<std::pair<float,float> > stratified_random_sample()
{
  std::vector< std::pair<float,float> > mo;
  srand(19260817);
  
  for(int i=0;i<4;++i)
    for(int j=0;j<4;++j)
    {
      float dx = (float)rand() / (float)RAND_MAX / 4.0f;
      float dy = (float)rand() / (float)RAND_MAX / 4.0f;
      dx += 1.0f / 4.0f *(float)i;
      dy += 1.0f / 4.0f *(float)j;
      //std::cout<<dx<<' '<<dy<<'\n';
      mo.push_back(std::make_pair(dx, dy));
    }
  return mo;
}

float phi(int base,int i)
{
  float f = 1.0f;
  float r = 0.0f;
  while(i > 0)
  {
      f = f / base;
      r += f * (i % base);
      i = i / base;
  }
  return r;
}

std::vector<std::pair<float,float> > Halton()
{
  std::vector< std::pair<float,float> > mo;
  int prime[2] = {2,3};
  for(int i=0;i<4;++i)
    for(int j=0;j<4;++j)
      {
        float dx = phi(prime[0],i);
        float dy = phi(prime[1],j);
        // std::cout<<dx<<' '<<dy<<'\n';
        mo.push_back(std::make_pair(dx, dy));
      }
  return mo;
}

PhongLightingIntegrator::PhongLightingIntegrator(std::shared_ptr<Camera> cam,
                                                 std::shared_ptr<Scene> scene)
    : camera(std::move(cam)), scene(std::move(scene)) {
}

void PhongLightingIntegrator::render() const {
  /*Ray a(camera->getPosition(), scene->getLight()->position - camera->getPosition());
  bool b;
  Interaction c;
  b = scene->intersect(a,c);
  std::cout << b << std::endl;
  return;*/

  /*Ray a(camera->getPosition(), Vec3f(0.5, 0.4, 0.4));
  bool b;
  Interaction c;
  b = scene->intersect(a,c);
  std::cout << b << std::endl;
  return;*/

  Vec2i resolution = camera->getImage()->getResolution();
  int cnt = 0;
  std::vector< std::pair<float,float> > mo;
  //mo.push_back(std::make_pair(0.33f,0.33f)),mo.push_back(std::make_pair(0.33f,0.66f)),mo.push_back(std::make_pair(0.66f,0.33f)),mo.push_back(std::make_pair(0.66f,0.66f));
  mo = simple();
  //mo = multi_sample();
  //mo = rotation(26.6f);
  //mo = rotation(-26.6f);
  //mo = random_sample();
  //mo = stratified_random_sample();
  //mo = Halton();
  

 #pragma omp parallel for schedule(guided, 2), shared(cnt)
  for (int dx = 0; dx < resolution.x(); dx++) {
 #pragma omp atomic
    ++cnt;
    printf("\r%.02f%%", cnt * 100.0 / resolution.x());
    for (int dy = 0; dy < resolution.y(); dy++) {
      Vec3f L(0.0, 0.0, 0.0);
      // TODO: Your code here.
      for(auto dd : mo)
      {
        //if(i!=2)continue;
        Ray ray = this->camera->generateRay((float)dx + dd.first, (float)dy + dd.second);
        // std::cout<<(float)dx + dd.first<<' '<<(float)dx + dd.second<<'\n';
        //printf("%d %d %d %d %d %d\n",ray.origin[0],ray.origin[1],ray.origin[2],ray.direction[0],ray.direction[1],ray.direction[2]);
        Interaction inter;
        inter.type = Interaction::Type::NONE;
        // puts("woring");
        if (this->scene->intersect(ray, inter))
        {
          // std::cout << "here";
          L += radiance(ray, inter) / (float)mo.size();
        }
        // puts("done");
      }
      
      camera->getImage()->setPixel(dx, dy, L);
    }
  }
}

Vec3f PhongLightingIntegrator::radiance(Ray &ray, Interaction &interaction) const {
  Vec3f radiance(0, 0, 0);
  // TODO: Your code here.
  //puts("000000");

  //puts("working");

  if(interaction.type == Interaction::Type::LIGHT)
  {
    // std::cout<<this->scene->getLight()->getColor().x()<<'\n';
    return this->scene->getLight()->getColor();
  }
   
  if(interaction.type == Interaction::Type::NONE)
    return radiance;
 
  // GEOMETRY
  // cos(theta) = (normal dot -ray.direction) / (norm(normal) * norm(ray.direction))
  // reflection = ray.direction + 2*norm(ray.direction)*cos(theta)*normal
  //            = 2 * normal.dot(-ray.direction) / normal.norm() * normal + ray.direction
  Vec3f reflect_direction = ray.direction + 2 * interaction.normal.dot(-ray.direction)  * interaction.normal;
  reflect_direction.normalize();
  Ray reflect_ray(interaction.pos, reflect_direction);
 // puts("finished");

  std::vector<LightSample> Lights = this->scene->getLight()->samples();
  for(auto light : Lights)
  {
    // puts("light`````````````````````");
    // ambient lighting
    Vec3f ambient = this->scene->getAmbient();

    // diffuse lighting
    Vec3f norm = interaction.normal.normalized();
    Vec3f lightDir = (light.position - interaction.pos).normalized(); 
    float diff = std::max(norm.dot(lightDir), 0.0f);
    Vec3f diffuse = diff * light.color;
    
    // specular lighting
    Vec3f viewDir = ray.direction.normalized();

    float specularStrength = 1.0;
    Vec3f reflectDir = 2 * interaction.normal.dot(lightDir) * interaction.normal - lightDir;
    reflectDir.normalize();
                
    float spec = powf(std::max(reflectDir.dot(-ray.direction), 0.0f), interaction.model.shininess);
    Vec3f specular = specularStrength * spec * light.color;  
        
    Vec3f result = ambient.cwiseProduct(interaction.model.ambient)+
                  diffuse.cwiseProduct(interaction.model.diffusion)
                 + specular.cwiseProduct(interaction.model.specular);
    //puts("1111111");
    Ray check_shadow(interaction.pos, light.position - interaction.pos);
    if(!this->scene->isShadowed(check_shadow))  
      radiance += result / (float)Lights.size();
    else
      radiance += ambient.cwiseProduct(interaction.model.ambient) / (float)Lights.size();
      //radiance += result / (float)Lights.size();
  }
  // std::cout<<radiance.x()<<' '<<radiance.y()<<' '<<radiance.z()<<'\n';
  //if(shadow)
  //  radiance += interaction.model.ambient + interaction.model.diffusion + interaction.model.specular;
  return radiance ;//+ this->scene->getAmbient();
}
