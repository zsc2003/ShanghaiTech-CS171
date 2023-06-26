#ifndef VOLUMERENDER_H_
#define VOLUMERENDER_H_

#include "camera.h"
#include "dataloader.h"
#include "accel.h"

#include <random>

class VolumeRender {
public:
    VolumeRender(std::shared_ptr<Camera> cam, std::shared_ptr<DataLoader> loa);
    ~VolumeRender() = default;
    void render() const;
private:
    std::shared_ptr<Camera> camera;
    std::shared_ptr<DataLoader> loader;
};

#endif //VOLUMERENDER_H_