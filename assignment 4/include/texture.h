#ifndef CS171_HW3_INCLUDE_TEXTURE_H_
#define CS171_HW3_INCLUDE_TEXTURE_H_

#include <vector>
#include <string>

#include "core.h"

class Texture {
 public:
  Texture();
  Texture(const std::string &path);
  Texture(const std::string &path, int opt);
  enum Type { DIFF, NORM, DISP };
  Type type;
  void loadTextureFromFile(const std::string &path);
  Vec3f getPixel(float u, float v) const;
  Vec3f getPixel(float u, float v, int level) const;


  int mipmap;
  void generate_mipmap();
  std::vector<std::vector<float> > mipmap_data;

 //private:
  std::vector<uint8_t> tex_data;
  unsigned int width;
  unsigned int height;
};

#endif //CS171_HW3_INCLUDE_TEXTURE_H_
