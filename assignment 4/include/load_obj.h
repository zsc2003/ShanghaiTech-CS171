#ifndef LOAD_OBJ_H_
#define LOAD_OBJ_H_

#include "core.h"
#include "geometry.h"

#include <string>

static bool loadObj(const std::string &path, std::vector<Vec3f> &vertices,
                    std::vector<Vec3f> &normals, std::vector<int> &v_index, std::vector<int> &n_index);

std::shared_ptr<TriangleMesh> makeMeshObject(std::string path_to_obj, Vec3f translation, float scale);

#endif //LOAD_OBJ_H_
