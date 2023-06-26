#ifndef DATALOADER_H_
#define DATALOADER_H_

#include "voxel.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

/// todo : loader from raw data, turn it to voxels

class DataLoader {
public:
    /// define a 3D data, x: width, y: height, z: length
    int width, height, length;
    /// define ratio for each voxel, value belongs to (0, 1], sizex : sizey : sizez define a voxel's shape
    int sizex, sizey, sizez;
    /// using vector to store the 3D data
    std::vector<std::vector<std::vector<UnitVoxel>>> VoxelData;
    /// define the bounding box for the 3-dimension Vector data
    AABB boundingbox;
    /// define the origin of the whole Voxexls
    Vec3f OriginVoxel{0.0f, 0.0f, 0.0f};
    /// define the three axis vector
    Vec3f xVector{1.0f, 0.0f, 0.0f};
    Vec3f yVector{0.0f, 0.0f, 1.0f};
    Vec3f zVector{0.0f, -1.0f, 0.0f};
    /// define the actually length of the whole data
    Vec3f worldsize{2.0f, 2.0f, 2.0f};
    /// define length as x*y*z
    int lengthraw;
    /// readed raw file as unint8_t || unint16_t
    void *rawdata = NULL;
    //  construct function
    //  path data file path only support for raw file
    //  x dimension x
    //  y dimension y
    //  z dimension z
    // x_size, y_size, z_size default value 1
    DataLoader(std::string path, int x, int y, int z, int x_size = 1, int y_size = 1, int z_size = 1);

    bool check(const Ray &ray, float *t_in, float *t_out);
    bool blockwalking(const Ray &ray, float *t_in, float *t_out);

    BVHNode* bvh;
    void buildBVH();
    void build(BVHNode* root, BVHNode* bvh);
    bool bvhHit(BVHNode *p, Ray ray, float *t_in, float *t_out);

};
#endif