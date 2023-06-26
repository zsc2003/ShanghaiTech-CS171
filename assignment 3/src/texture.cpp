#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>

#include "texture.h"

void Texture::loadTextureFromFile(const std::string &path) {
  int w, h, ch;
  stbi_uc *img = stbi_load(path.c_str(), &w, &h, &ch, 0);
  tex_data.assign(img, img + w * h * ch);
  //std::cout<<path<<'\n';
  //std::cout<<w<<' '<<h<<' '<<ch<<' '<<tex_data.size()<<'\n';
  stbi_image_free(img);
  this->width = w;
  this->height = h;
}

Texture::Texture() : width(0), height(0), mipmap(0) {}

Texture::Texture(const std::string &path) : width(0), height(0), mipmap(0) {
  loadTextureFromFile(path);
}

Vec3f Texture::getPixel(float u, float v) const
{
  int x = (int)(u * width);
  int y = (int)(v * height);
  if (x == width)
    --x;
  if (y == height)
    --y;
  
  // std::cout<<width<<' '<<height<<' '<<x<<' '<<y<<'\n';
  Vec3f rgb((float)tex_data[(x + width * y) * 3], (float)tex_data[(x + width * y) * 3 + 1], (float)tex_data[(x + width * y) * 3 + 2]);
  return rgb;
}

void Texture::generate_mipmap()
{
  puts("generating mipmap");
  this->mipmap = 1;
  int new_w = width;
  int new_h = height;
  int w = width;
  int h = height;
  int layer = 0;

  std::vector<float> transition;
  for(auto element : this->tex_data)
  {
    transition.push_back((float)(int)element / 255.0f);
    // std::cout<<(float)element<<' '<<(float)element / 255.0f<<'\n';
  }
  this->mipmap_data.push_back(transition);
  
  // std::cout<<mipmap_data[0].size()<<' '<<tex_data.size()<<'\n';
  // std::cout<<width<<' '<<height<<'\n';
  // puts("finished copy");
  // puts("begin working");
  
  while((new_h > 1) && (new_w > 1))
  {
      new_w >>= 1;
      new_h >>= 1;
     // if(new_h == 1 || new_w == 1)
      //  break;
      // std::cout<<new_w<<'\n';

      std::vector<float> new_layer;
      // std::cout<<mipmap_data.size()<<'\n';

      for (int i = 0; i < new_h; ++i)
      {
        // puts("in the first cycle");
        for (int j = 0; j < new_w; ++j)
        {
        // puts("in the second cycle");
          
          float r = (float)mipmap_data[layer][((2 * i) * new_w * 2 + 2 * j) * 3] + (float)mipmap_data[layer][((2 * i) * new_w * 2 + 2 * j + 1) * 3]
                  + (float)mipmap_data[layer][((2 * i + 1) * new_w * 2 + 2 * j) * 3] + (float)mipmap_data[layer][((2 * i + 1) * new_w * 2 + 2 * j + 1) * 3];
          r /= 4.0f;
          new_layer.push_back(r);

          float g = (float)mipmap_data[layer][((2 * i) * new_w * 2 + 2 * j) * 3 + 1] + (float)mipmap_data[layer][((2 * i) * new_w * 2 + 2 * j + 1) * 3 + 1]
                  + (float)mipmap_data[layer][((2 * i + 1) * new_w * 2 + 2 * j) * 3 + 1] + (float)mipmap_data[layer][((2 * i + 1) * new_w * 2 + 2 * j + 1) * 3 + 1];
          g /= 4.0f;
          new_layer.push_back(g);

          float b = (float)mipmap_data[layer][((2 * i) * new_w * 2 + 2 * j) * 3 + 2] + (float)mipmap_data[layer][((2 * i) * new_w * 2 + 2 * j + 1) * 3 + 2]
                  + (float)mipmap_data[layer][((2 * i + 1) * new_w * 2 + 2 * j) * 3 + 2] + (float)mipmap_data[layer][((2 * i + 1) * new_w * 2 + 2 * j + 1) * 3 + 2];
          b /= 4.0f;
          new_layer.push_back(b);

         // std::cout<<r<<' '<<g<<' '<<b<<'\n';
        // puts("out the second cycle");

        }
        // puts("out the first cycle");

      }
      ++layer;
      this->mipmap_data.push_back(new_layer);
      // std::cout<<mipmap_data.size()<<'\n';
  }
  // puts("finished working");

}

Vec3f Texture::getPixel(float u, float v, int level) const
{
  // puts("woring");
  int w, h;
  int scaler = (int)powf(2, level);
  w = (int)((float)width / (float)scaler);
  h = (int)((float)height / (float)scaler);
  int x = (int)(u * w);
  int y = (int)(v * h);
  if (x == width)
    --x;
  if (y == height)
    --y;
  
  // std::cout<<w<<' '<<h<<' '<<x<<' '<<y<<'\n';
  Vec3f rgb((float)mipmap_data[level][(x + w * y) * 3], (float)mipmap_data[level][(x + w * y) * 3 + 1], (float)mipmap_data[level][(x + w * y) * 3 + 2]);
  
  // puts("finished");
  
  return rgb;
}

Texture::Texture(const std::string &path, int opt) : width(0), height(0), mipmap(0) {
  loadTextureFromFile(path);
  if(opt == 1)
    this->generate_mipmap();
}