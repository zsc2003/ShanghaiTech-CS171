#include "voxel.h"
#include<iostream>

UnitVoxel::UnitVoxel(AABB bdbox) {
    aabb = bdbox;
}

// test if a samplepoint is in this voxel
bool UnitVoxel::intersectVoxel(Vec3f samplepoint) {
    if (samplepoint.x() < aabb.low_bnd.x() && samplepoint.x() > aabb.upper_bnd.x()
     && samplepoint.y() < aabb.low_bnd.y() && samplepoint.y() > aabb.upper_bnd.y()
     && samplepoint.z() < aabb.low_bnd.z() && samplepoint.z() > aabb.upper_bnd.z()) {
        return true;
    }
    else {
        return false;
    }
};
// get the rgb attribute for current voxel
Vec3f UnitVoxel::getRGB() {
    return RGBcolor;
};
// get the alpha attribute for current voxel
float UnitVoxel::getAlpha() {
    return Alpha;
};

//transfer function 
void UnitVoxel::transfer(uint8_t feature) {
    if (feature == '\0') {
        if_empty = true;
        // std::cout<<"empty\n";
        return;
    } else {
        // std::cout<<"nonempty\n";
        if_empty = false;
    }
    // std::cout<<(int)feature<<'\n';

    Vec3f high{1.0f, 1.0f, 1.0f};
    Vec3f low{0.0f, 0.0f, 0.0f};
    float f = (float) feature / 255.0f;
    float alpha = (std::exp(f) - 1.0) / (std::exp(1.0) - 1.0);
    RGBcolor = f * high + (1.0 - f) * low;
    Alpha = alpha;
};