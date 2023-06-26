#include <iostream>
#include <chrono>

#include "volumerender.h"

int main(int argc, char *argv[]) {
    Vec2i img_resolution(800, 800);
    Vec3f cam_pos(-5, 2, 0);
    Vec3f cam_look_at(0, 0, 0);

    // init image.
    std::shared_ptr<ImageRGB> rendered_img  = std::make_shared<ImageRGB>(img_resolution.x(), img_resolution.y());
    // camera setting
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    camera->setPosition(cam_pos);
    camera->lookAt(cam_look_at);
    camera->setFov(45);
    camera->setImage(rendered_img);

    
    // std::shared_ptr<DataLoader> loader = std::make_shared<DataLoader>("../../data/raw/neghip_64x64x64_uint8.raw", 64, 64, 64);
    std::shared_ptr<DataLoader> loader = std::make_shared<DataLoader>("../../data/raw/bonsai_256x256x256_uint8.raw", 256, 256, 256);
    // std::shared_ptr<DataLoader> loader = std::make_shared<DataLoader>("../../data/raw/neghip_64x64x64_uint8.raw", 64, 64, 64);
    std::unique_ptr<VolumeRender> renderer = std::make_unique<VolumeRender>(camera, loader);
    std::cout << "Start Rendering..." << std::endl;
    auto start = std::chrono::steady_clock::now();
    // render scene
    renderer->render();
    auto end = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "\nRender Finished in " << time << "ms." << std::endl;
    rendered_img->writeImgToFile("../../result.png");
    std::cout << "Image saved to disk." << std::endl;
    return 0;
}