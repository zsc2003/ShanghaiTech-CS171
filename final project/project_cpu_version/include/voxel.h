#ifndef VOXEL_H_
#define VOXEL_H_

#include "accel.h"
#include "Windows.h"

// a voxel include 3 args: aabb
struct UnitVoxel {
    // crop aabb for each voxel
    AABB aabb;
    // rgb for each voxel
    Vec3f RGBcolor;
    // alpha for each voxel
    float Alpha;
    // voxel if empty
    bool if_empty;
    UnitVoxel() = default;
    // construct function
    UnitVoxel(AABB bdbox);
    // test if a samplepoint is in this voxel
    bool intersectVoxel(Vec3f samplepoint);
    // get the rgb attribute for current voxel
    Vec3f getRGB();
    // get the alpha attribute for current voxel
    float getAlpha();
    // transfer function
    void transfer(uint8_t feature);
};
#endif