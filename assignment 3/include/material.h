#ifndef CS171_HW3_INCLUDE_MATERIAL_H_
#define CS171_HW3_INCLUDE_MATERIAL_H_

#include "interaction.h"
#include "texture.h"

class Material {
 public:
  Material() = default;
  virtual ~Material() = default;
  [[nodiscard]] virtual InteractionPhongLightingModel evaluate(Interaction &interaction) const = 0;
};

class ConstColorMat : public Material {
 public:
  ConstColorMat();
  explicit ConstColorMat(const Vec3f &color, float sh = 16.f);
  ConstColorMat(Vec3f diff, Vec3f spec, Vec3f ambient, float sh = 16.f);
  [[nodiscard]] InteractionPhongLightingModel evaluate(Interaction &interaction) const override;
 private:
  Vec3f diffuse_color;
  Vec3f specular_color;
  Vec3f ambient_color;
  float shininess;
};

class TextureMat : public Material {
 // TODO: Bonus - texture material
 
public:
  TextureMat(const std::string &path);
  TextureMat(const std::string &path, int opt);
  TextureMat(Texture tex, float sh = 16.0f);
  TextureMat(const std::string &diff_path, const std::string &disp_path, const std::string &norm_path);
  [[nodiscard]] InteractionPhongLightingModel evaluate(Interaction &interaction) const override;
  Texture diff, disp, norm;
  float shininess;

};

#endif //CS171_HW3_INCLUDE_MATERIAL_H_
