#include "material.h"
#include "texture.h"

#include <iostream>
#include <utility>

ConstColorMat::ConstColorMat() : diffuse_color({1, 1, 1}),
                                 specular_color({1, 1, 1}),
                                 ambient_color(1, 1, 1),
                                 shininess(16.f) {}

ConstColorMat::ConstColorMat(const Vec3f &color, float sh)
    : diffuse_color(color), specular_color(color), ambient_color(color), shininess(sh) {}

ConstColorMat::ConstColorMat(Vec3f diff, Vec3f spec, Vec3f ambient, float sh)
    : diffuse_color(std::move(diff)),
      specular_color(std::move(spec)),
      ambient_color(std::move(ambient)),
      shininess(sh) {}

InteractionPhongLightingModel ConstColorMat::evaluate(Interaction &interaction) const {
  InteractionPhongLightingModel m;
  // TODO: Your code here.
  m.ambient = this->ambient_color;
  // std::cout<<m.ambient<<'\n';

  m.diffusion = this->diffuse_color;
  m.specular = this->specular_color;
  m.shininess = this->shininess;
  return m;
}

TextureMat::TextureMat(const std::string &diff_path, const std::string &disp_path, const std::string &norm_path)
            : diff(diff_path), disp(disp_path), norm(norm_path), shininess(16.0f) {}

TextureMat::TextureMat(const std::string &path) : diff(path), shininess(16.0f) {}

TextureMat::TextureMat(Texture tex, float sh) : diff(tex), shininess(sh) {}

TextureMat::TextureMat(const std::string &path, int opt) : diff(path, opt), shininess(16.0f) {}

InteractionPhongLightingModel TextureMat::evaluate(Interaction &interaction) const {

  InteractionPhongLightingModel m;
  if(this->diff.mipmap == 1)//bonus3 mipmap
  {
    
    // float dist_uv = - 2 * in.entry_dist * in.cam_factor / in.normal.dot(in.in_direction);
    // float dist_tex = (float)in.tex_width / texture->getDimension()[0];
    
    
    // float dist_tex = (float)this->diff.width / this->diff.getDimension()[0];
    float dist_uv = - 2 * interaction.dist * 0.1f / interaction.normal.dot(interaction.ray_direction);
    float dist_tex = 104.0f * 5.0f / this->diff.width;
    
    // std::cout<< (dist_uv / dist_tex) <<'\n';
    float l = dist_uv / dist_tex;
    float d = log2f(l);
    if (d < 0.0f)
      d = 0.0f;

    size_t level = (int)(d);
    float in = d - level;
    level = std::min(level, this->diff.mipmap_data.size() - 1);
    // puts("woring");

    size_t level2 = std::min(level + 1, this->diff.mipmap_data.size() - 1);
    // std::cout<<level;
    Vec3f rgb1 = this->diff.getPixel(interaction.uv[0], interaction.uv[1], level);// / 255.0f;
    Vec3f rgb2 = this->diff.getPixel(interaction.uv[0], interaction.uv[1], level2);// / 255.0f;
    // puts("finihed");

    m.ambient = (1.0f - in) * rgb1 + in * rgb2;
    m.diffusion = m.ambient;
    m.specular = m.ambient;

    return m;
  }
  if(this->disp.tex_data.size() == 0)// bonus 1
  {
    // puts("woring here");
    // m.ambient = this->texture;
    // puts("woring in TextureMat::evaluate");
    //std::cout<<interaction.uv[0]<<' '<< interaction.uv[1]<<'\n';
    m.ambient = this->diff.getPixel(interaction.uv[0], interaction.uv[1]) / 255.0f;
    //std::cout<<m.ambient<<'\n';
    m.diffusion = m.ambient;
    m.specular = m.ambient;

    // puts("finished");
    m.shininess = this->shininess;
    return m; 
  }
  m.ambient = this->diff.getPixel(interaction.uv[0], interaction.uv[1]) / 255.0f;
  m.diffusion = m.ambient;
  m.specular = m.ambient;
  m.shininess = this->shininess;

  //interaction.normal = this->norm.getPixel(interaction.uv[0], interaction.uv[1]).normalized();
  // std::cout<<this->disp.getPixel(interaction.uv[0], interaction.uv[1]) / 255.0f <<'\n';
  Eigen::Matrix<float,3,3> R,I,V;
  //Vec3f n1 = interaction.normal;
  Vec3f n1(0.0f, 0.0f, 1.0f);
  Vec3f n2 = interaction.normal.normalized();
  Vec3f v = n1.cross(n2);
  float s = v.norm();
  float c = n1.dot(n2);

  I<< 1.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 1.0f;
  V<< 0.0f, -v.z(), v.y(),
      v.z(), 0.0f, -v.x(),
      -v.y(), v.x(), 0.0f;
  R = I + V + V * V * (1.0f / (1.0f + c));
  // puts("working");
 /* float cosA = c;
  float k = 1.0f / (1.0f + cosA);
  R << 
    v.x() * v.x() * k + cosA, v.y() * v.x() * k - v.z(), v.z() * v.x() * k + v.y(),
    v.x() * v.y() * k + v.z(), v.y() * v.y() * k + cosA, v.z() * v.y() * k - v.x(),
    v.x() * v.z() * k - v.y(), v.y() * v.z() * k + v.x(), v.z() * v.z() * k + cosA;

  if(n1 == -n2)
    R = Eigen::Matrix3f::Identity();*/

  // std::cout<<(R.inverse()*n2)<<'\n';
      /*R << n1.dot(n2),-n1.cross(n2).norm(),0.0f,
       n1.cross(n2).norm(),n1.dot(n2),0.0f,
       0.0f,0.0f,1.0f;*/
  // puts("working:(");
  //  std::cout<<"-------------------\n"<<interaction.normal.normalized()<<"\n\n";

  /*Vec3f new_norm = this->norm.getPixel(interaction.uv[0], interaction.uv[1]) / 255.0f;
  new_norm = new_norm * 2.0f;
  Vec3f id(1.0f, 1.0f, 1.0f);
  new_norm = new_norm - id;
  interaction.normal = R * new_norm;
  interaction.normal/=interaction.normal.norm();*/
  
  interaction.normal = R * (this->norm.getPixel(interaction.uv[0], interaction.uv[1]) / 255.0f * 2.0f - Vec3f(1.0f, 1.0f, 1.0f));
  interaction.pos += this->disp.getPixel(interaction.uv[0], interaction.uv[1]).x() / 255.0f *interaction.normal;
  //  std::cout<<interaction.normal.normalized()<<"\n\n";
  //interaction.normal.normalize();
  // interaction.pos += this->disp.getPixel(interaction.uv[0], interaction.uv[1]).x() / 255.0f * interaction.normal;
  // interaction.dist += this->disp.getPixel(interaction.uv[0], interaction.uv[1]).x() / 255.0f / interaction.ray_direction.norm();
  // std::cout<<this->disp.getPixel(interaction.uv[0], interaction.uv[1])<<'\n';
  // std::cout<<interaction.pos<<'\n';
  return m;
}
