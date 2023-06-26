#include "volumerender.h"
#include <omp.h>
#include <iostream>
#include <utility>
#include <vector>
#include <cstdlib>

VolumeRender::VolumeRender(std::shared_ptr<Camera> cam, std::shared_ptr<DataLoader> loa) : camera(std::move(cam)), loader(std::move(loa)) {}

std::vector<std::pair<float, float>> multi_sample();

template<typename T>
T bri_inter(T lo, T hi, float t)
{
    return lo * (1 - t) + hi * t;
}

template<typename T>
T tri(T p1, T p2, T p3, T p4, T p5, T p6, T p7, T p8, float t1, float t2, float t3)
{
    T a = bri_inter(p8, p7, t1);
    T b = bri_inter(p4, p3, t1);
    T c = bri_inter(p5, p6, t1);
    T d = bri_inter(p1, p2, t1);

    T e = bri_inter(a, b, t2);
    T f = bri_inter(c, d, t2);

    return bri_inter(e, f, t3);
}

void VolumeRender::render() const {
    Vec2i resolution = camera->getImage()->getResolution();
    int cnt = 0;
    int WIDTH = camera->getImage()->getResolution().x();
    int HEIGHT = camera->getImage()->getResolution().y();
    Vec3f bgColor(1.0f, 1.0f, 1.0f);
    std::vector<std::pair<float, float>> mssp = multi_sample();

    this->loader->buildBVH();

#pragma omp parallel for schedule(guided, 2), shared(cnt)
    for (int dy = 0; dy < resolution.y(); dy++) {
#pragma omp atomic
        ++cnt;
        printf("\r%.02f%%", cnt * 100.0 / resolution.x());
        for (int dx = 0; dx < resolution.x(); dx++) {
            Vec3f Lres(0.0f, 0.0f, 0.0f);
            // TODO: Your code here.
            int ssp = 0;
            for (auto rayoffset : mssp) {
                ssp++;
                Ray ray = this->camera->generateRay(dx + rayoffset.first, dy + rayoffset.second);
                Vec3f color = Vec3f(0.0f, 0.0f, 0.0f);
                float alpha = 0.0f;
                float t_min = 114514, t_max = 114514;
                bool add = 0;
                //if (loader->boundingbox.intersect(ray, &t_min, &t_max)) {
                if (loader->check(ray, &t_min, &t_max)) {

                    for (float t = t_min; t <= t_max; t += 0.001f) {

                        bool work = loader->blockwalking(ray, &t, &t_max);
                        
                        Vec3f point(ray.origin + ray.direction * t);
                        // in need of hashing to optimize this
                        // need to change this as real vector is not standard
                        int xcoor = std::ceil((float)(point - loader->OriginVoxel).x() / (2.0f / (float)loader->length)) + loader->length / 2;
                        int ycoor = std::ceil((float)(point - loader->OriginVoxel).y() / (2.0f / (float)loader->width)) + loader->width / 2;
                        int zcoor = std::ceil((float)(point - loader->OriginVoxel).z() / (2.0f / (float)loader->height)) + loader->height / 2;
                        
                        
                        float t1 = (float)(point - loader->OriginVoxel).x() / (2.0f / (float)loader->length) + (float)loader->length / 2.0f;
                        float t2 = (float)(point - loader->OriginVoxel).y() / (2.0f / (float)loader->width) + (float)loader->width / 2.0f;
                        float t3 = (float)(point - loader->OriginVoxel).z() / (2.0f / (float)loader->height) + (float)loader->height / 2.0f;
                        
                        t1 = 1.0f - xcoor + t1;
                        t2 = 1.0f - ycoor + t2;
                        t3 = 1.0f - zcoor + t3;
                        // t1 = xcoor - t1;
                        // t2 = ycoor - t2;
                        // t3 = zcoor - t3;
                        
                        // std::cout<<loader->length<<'\n';
                        // std::cout<<t1<<' '<<xcoor<<'\n';
                        // std::cout<<t1<<'\n';
                        if(t1 < 0.0f || t2 < 0.0f || t3 < 0.0f || t1 > 1.0f || t2 > 1.0f || t3 > 1.0f)
                        {
                            std::cout<<"error\n\n";
                            system("pause");
                        }

                        int x2 = xcoor - 1, y2 = ycoor - 1, z2 = zcoor - 1;
                        if(x2 < 0){
                            x2 = 0;
                        }
                        if(y2 < 0){
                            y2 = 0;
                        }
                        if(z2 < 0){
                            z2 = 0;
                        }
                        if (x2 + 1 >= loader->length - 1) {
                            x2 = loader->length - 2;
                        }
                        if (y2 + 1  >= loader->width - 1) {
                            y2 = loader->width - 2;
                        }
                        if (z2 + 1  >= loader->height - 1) {
                            z2 = loader->height - 2;
                        }
        
                        if (xcoor >= loader->length - 1) {
                            xcoor = loader->length - 1;
                        }
                        if (ycoor >= loader->width - 1) {
                            ycoor = loader->width - 1;
                        }
                        if (zcoor >= loader->height - 1) {
                            zcoor = loader->height - 1;
                        }

                        bool flag = 0;
                        if (loader->VoxelData[xcoor][ycoor][zcoor].if_empty) {
                            flag = 1;
                            // continue;
                        }

                        if(loader->VoxelData[x2][y2][z2].if_empty || loader->VoxelData[x2+1][y2][z2].if_empty || loader->VoxelData[x2][y2+1][z2].if_empty || loader->VoxelData[x2][y2][z2+1].if_empty || loader->VoxelData[x2+1][y2+1][z2].if_empty || loader->VoxelData[x2+1][y2][z2+1].if_empty || loader->VoxelData[x2][y2+1][z2+1].if_empty || loader->VoxelData[x2+1][y2+1][z2+1].if_empty)
                        {
                            flag = 1;
                            // continue;
                        }

/*{puts("111");loader->VoxelData[x2][y2][z2].getRGB();loader->VoxelData[x2+1][y2][z2].getRGB();loader->VoxelData[x2][y2+1][z2].getRGB();loader->VoxelData[x2][y2][z2+1].getRGB();loader->VoxelData[x2+1][y2+1][z2].getRGB();loader->VoxelData[x2+1][y2][z2+1].getRGB();loader->VoxelData[x2][y2+1][z2+1].getRGB();loader->VoxelData[x2+1][y2+1][z2+1].getRGB();puts("222");// puts("111");loader->VoxelData[x2][y2][z2].getAlpha();loader->VoxelData[x2+1][y2][z2].getAlpha();loader->VoxelData[x2][y2+1][z2].getAlpha();loader->VoxelData[x2][y2][z2+1].getAlpha();loader->VoxelData[x2+1][y2+1][z2].getAlpha();loader->VoxelData[x2+1][y2][z2+1].getAlpha();loader->VoxelData[x2][y2+1][z2+1].getAlpha();puts("222");std::cout<<x2<<' '<<y2<<' '<<z2<<' ';loader->VoxelData[x2+1][y2+1][z2+1].getAlpha();std::cout<<"  yes\n";}*/


                        // puts("1111");

                        Vec3f tri_rgb;
                        float tri_alpha;
                        if(flag == 1)
                        {
                            // tri_rgb = Vec3f(255.0f, 255.0f, 255.0f);
                            continue;
                            tri_rgb = Vec3f(0.01f, 0.01f, 0.01f);
                            tri_alpha = 1.0f;
                        }
                        else
                        {
                            tri_rgb = tri(loader->VoxelData[x2][y2][z2].getRGB(), loader->VoxelData[x2+1][y2][z2].getRGB(), loader->VoxelData[x2+1][y2][z2+1].getRGB(), loader->VoxelData[x2][y2][z2+1].getRGB(), loader->VoxelData[x2][y2+1][z2].getRGB(), loader->VoxelData[x2+1][y2+1][z2].getRGB(), loader->VoxelData[x2+1][y2+1][z2+1].getRGB(), loader->VoxelData[x2][y2+1][z2+1].getRGB(), t1, t2, t3);
                            tri_alpha = tri(loader->VoxelData[x2][y2][z2].getAlpha(), loader->VoxelData[x2+1][y2][z2].getAlpha(), loader->VoxelData[x2+1][y2][z2+1].getAlpha(), loader->VoxelData[x2][y2][z2+1].getAlpha(), loader->VoxelData[x2][y2+1][z2].getAlpha(), loader->VoxelData[x2+1][y2+1][z2].getAlpha(), loader->VoxelData[x2+1][y2+1][z2+1].getAlpha(), loader->VoxelData[x2][y2+1][z2+1].getAlpha(), t1, t2, t3);
                        }
                        
                        // puts("2222");

                        // color += ((1 - alpha) * loader->VoxelData[xcoor][ycoor][zcoor].getRGB());
                        // alpha += ((1 - alpha) * (1.0f - loader->VoxelData[xcoor][ycoor][zcoor].getAlpha()));
                        add = 1;
                        color += ((1 - alpha) * tri_rgb);
                        alpha += ((1 - alpha) * (1.0f - tri_alpha));

                        /// if sum of alpha gt 1, simply break
                        if (alpha > 0.999f) {
                            break;
                        }
                    }
                    // Blend background
                    color = alpha * color + (1 - alpha) * bgColor;
                    Lres += color;
                }
                /*else if(add == 0)
                {
                    Lres += Vec3f(1.0f, 1.0f, 1.0f);
                }*/
                else {
                    Lres += Vec3f(1.0f, 1.0f, 1.0f);
                }
            }
            camera->getImage()->setPixel(dx, dy, Lres / (float)ssp);
        }
    }
}

std::vector<std::pair<float, float>> multi_sample() {
  std::vector<std::pair<float,float>> sp;
  for(int i=0;i<4;++i) {
        for(int j=0;j<4;++j) {
            sp.push_back(std::make_pair(0.1f + 0.25f * (float)i, 0.1f + 0.25f * (float)j));
        }
  }
  return sp;
}