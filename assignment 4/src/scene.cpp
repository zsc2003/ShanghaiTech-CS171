#include "scene.h"
#include "load_obj.h"
#include "light.h"

#include <utility>
#include <iostream>

void Scene::addObject(std::shared_ptr<TriangleMesh> &mesh) {
  objects.push_back(mesh);
}

void Scene::setLight(const std::shared_ptr<Light> &new_light) {
  light = new_light;
}
bool Scene::isShadowed(Ray &shadow_ray) {
  Interaction in;
  return intersect(shadow_ray, in) && in.type == Interaction::Type::GEOMETRY;
}

bool Scene::intersect(Ray &ray, Interaction &interaction) {
  light->intersect(ray, interaction);
  
  for (const auto& wall: this->environment) {
    // puts("working here");
    Interaction cur_it;
    if (wall->intersect(ray, cur_it) && (cur_it.dist < interaction.dist)) {
      interaction = cur_it;
      // puts("intersectd!");
      // std::cout<<interaction.texture_rgb<<'\n';
    }
  }

  if(this->is_bonus4)
  {
    Interaction cur_it;
    if (this->intersect(ray, cur_it) && (cur_it.dist < interaction.dist)) {
      interaction = cur_it;
    }
    return interaction.type != Interaction::Type::NONE;
  }

  for (const auto& obj: objects) {
    Interaction cur_it;
    if (obj->intersect(ray, cur_it) && (cur_it.dist < interaction.dist)) {
      interaction = cur_it;
    }
  }
  return interaction.type != Interaction::Type::NONE;
}

const std::shared_ptr<Light> &Scene::getLight() const {
  return light;
}

void initSceneFromConfig(const Config &config, std::shared_ptr<Scene> &scene) {
  // add square light to scene.
  std::shared_ptr<Light> light = std::make_shared<SquareAreaLight>(Vec3f(config.light_config.position),
                                                                   Vec3f(config.light_config.radiance),
                                                                   Vec2f(config.light_config.size));
  scene->setLight(light);

  Vec2f light_size = Vec2f(config.light_config.size);

  if(light_size.y() > 0.5f)//bonus 4
  {
    scene->is_bonus4 = true;
  }

  if(light_size.x() < 1e-3)//bonus 3
  {
    Vec2f size(3.0f,3.0f);

    std::shared_ptr<Light> nx = std::make_shared<Environment>(Vec3f(-1.5f,1.0f,0.0f),size,Vec3f(0.0f,0.0f,-1.0f),Vec3f(0.0f,1.0f,0.0f),Vec3f(1.0f,0.0f,0.0f)
      ,"D:/!!shanghaitech/computer graphic cs171/assignment-4-zsc2003/environment/Standard-Cube-Map/nx.png",0);
    std::shared_ptr<Light> px = std::make_shared<Environment>(Vec3f(1.5f,1.0f,0.0f),size,-Vec3f(0.0f,0.0f,1.0f),-Vec3f(0.0f,1.0f,0.0f),Vec3f(-1.0f,0.0f,0.0f)
      ,"D:/!!shanghaitech/computer graphic cs171/assignment-4-zsc2003/environment/Standard-Cube-Map/px.png",0);
    std::shared_ptr<Light> ny = std::make_shared<Environment>(Vec3f(0.0f,1.0f,-1.5f),size,Vec3f(-1.0f,0.0f,0.0f),-Vec3f(0.0f,1.0f,0.0f),Vec3f(0.0f,0.0f,1.0f)
      ,"D:/!!shanghaitech/computer graphic cs171/assignment-4-zsc2003/environment/Standard-Cube-Map/nz.png",0);
    std::shared_ptr<Light> py = std::make_shared<Environment>(Vec3f(0.0f,1.0f,1.5f),size,Vec3f(1.0f,0.0f,0.0f),-Vec3f(0.0f,1.0f,0.0f),Vec3f(0.0f,0.0f,-1.0f)
      ,"D:/!!shanghaitech/computer graphic cs171/assignment-4-zsc2003/environment/Standard-Cube-Map/pz.png",1);
    std::shared_ptr<Light> nz = std::make_shared<Environment>(Vec3f(0.0f,-0.5f,0.0f),size,Vec3f(1.0f,0.0f,0.0f),Vec3f(0.0f,0.0f,-1.0f),Vec3f(0.0f,1.0f,0.0f)
      ,"D:/!!shanghaitech/computer graphic cs171/assignment-4-zsc2003/environment/Standard-Cube-Map/ny.png",0);
    std::shared_ptr<Light> pz = std::make_shared<Environment>(Vec3f(0.0f,2.5f,0.0f),size,Vec3f(1.0f,0.0f,0.0f),Vec3f(0.0f,0.0f,1.0f),Vec3f(0.0f,-1.0f,0.0f)
      ,"D:/!!shanghaitech/computer graphic cs171/assignment-4-zsc2003/environment/Standard-Cube-Map/py.png",0);

/*
    Vec2f size(20.0f,20.0f);
    std::shared_ptr<Light> nx = std::make_shared<Environment>(Vec3f(-10.0f,0.0f,0.0f),size,Vec3f(0.0f,0.0f,-1.0f),Vec3f(0.0f,1.0f,0.0f),Vec3f(1.0f,0.0f,0.0f)
      ,"D:/!!shanghaitech/computer graphic cs171/assignment-4-zsc2003/environment/Standard-Cube-Map/nx.png",0);
    std::shared_ptr<Light> px = std::make_shared<Environment>(Vec3f(10.0f,0.0f,0.0f),size,-Vec3f(0.0f,0.0f,1.0f),-Vec3f(0.0f,1.0f,0.0f),Vec3f(-1.0f,0.0f,0.0f)
      ,"D:/!!shanghaitech/computer graphic cs171/assignment-4-zsc2003/environment/Standard-Cube-Map/px.png",0);
    std::shared_ptr<Light> ny = std::make_shared<Environment>(Vec3f(0.0f,0.0f,-10.0f),size,Vec3f(-1.0f,0.0f,0.0f),-Vec3f(0.0f,1.0f,0.0f),Vec3f(0.0f,0.0f,1.0f)
      ,"D:/!!shanghaitech/computer graphic cs171/assignment-4-zsc2003/environment/Standard-Cube-Map/nz.png",0);
    std::shared_ptr<Light> py = std::make_shared<Environment>(Vec3f(0.0f,0.0f,10.0f),size,Vec3f(1.0f,0.0f,0.0f),-Vec3f(0.0f,1.0f,0.0f),Vec3f(0.0f,0.0f,-1.0f)
      ,"D:/!!shanghaitech/computer graphic cs171/assignment-4-zsc2003/environment/Standard-Cube-Map/pz.png",1);
    std::shared_ptr<Light> nz = std::make_shared<Environment>(Vec3f(0.0f,-10.0f,0.0f),size,Vec3f(1.0f,0.0f,0.0f),Vec3f(0.0f,0.0f,-1.0f),Vec3f(0.0f,1.0f,0.0f)
      ,"D:/!!shanghaitech/computer graphic cs171/assignment-4-zsc2003/environment/Standard-Cube-Map/ny.png",0);
    std::shared_ptr<Light> pz = std::make_shared<Environment>(Vec3f(0.0f,10.0f,0.0f),size,Vec3f(1.0f,0.0f,0.0f),Vec3f(0.0f,0.0f,1.0f),Vec3f(0.0f,-1.0f,0.0f)
      ,"D:/!!shanghaitech/computer graphic cs171/assignment-4-zsc2003/environment/Standard-Cube-Map/py.png",0);*/

    scene->environment.push_back(nx);
    scene->environment.push_back(px);
    scene->environment.push_back(ny);
    scene->environment.push_back(py);
    scene->environment.push_back(nz);
    scene->environment.push_back(pz);
  }

  // init all materials.
  std::map<std::string, std::shared_ptr<BSDF>> mat_list;
  for (const auto &mat: config.materials) {
    std::shared_ptr<BSDF> p_mat;
    switch (mat.type) {
      case MaterialType::DIFFUSE: {
        p_mat = std::make_shared<IdealDiffusion>(Vec3f(mat.color));
        mat_list[mat.name] = p_mat;
        break;
      }
      case MaterialType::SPECULAR: {
        p_mat = std::make_shared<Specular>(Vec3f(mat.color));
        mat_list[mat.name] = p_mat;
        break;
      }
      case MaterialType::TRANSLUCENT: {
        p_mat = std::make_shared<Translucent>(Vec3f(mat.color));
        mat_list[mat.name] = p_mat;
        break;
      }

      default: {
        std::cerr << "unsupported material type!" << std::endl;
        exit(-1);
      }
    }
  }
  // add mesh objects to scene. Translation and scaling are directly applied to vertex coordinates.
  // then set corresponding material by name.
  std::cout << "loading obj files..." << std::endl;
  for (auto &object: config.objects) {
    auto mesh_obj = makeMeshObject(object.obj_file_path, Vec3f(object.translate), object.scale);
    mesh_obj->setMaterial(mat_list[object.material_name]);
    if(scene->is_bonus4)
    {
      for(auto vertex : mesh_obj->vertices)
        scene->vertices.push_back(vertex);
      for(auto normal : mesh_obj->normals)
        scene->normals.push_back(normal);
    }
    if (object.has_bvh) {
      mesh_obj->buildBVH();
    }
    scene->addObject(mesh_obj);
  }
  
}

void Scene::buildbvh()
{

  puts("start building bvh tree");
  Triangle tri;
  
  this->bvh = new BVHNode();
  
  this->bvh->l = 0;
  this->bvh->r = this->bvh->triangles.size() - 1;
  AABB aabb(this->vertices[this->bvh->triangles[0].v_idx[0]], this->vertices[this->bvh->triangles[0].v_idx[1]], this->vertices[this->bvh->triangles[0].v_idx[2]]);
  this->bvh->aabb = aabb;
  for(int i = 1; i < this->bvh->triangles.size(); ++i)
  {
    aabb = AABB(this->vertices[this->bvh->triangles[i].v_idx[0]], this->vertices[this->bvh->triangles[i].v_idx[1]], this->vertices[this->bvh->triangles[i].v_idx[2]]);
    this->bvh->aabb = AABB(this->bvh->aabb, aabb);
  }

  puts("getting the function");
  this->build(this->bvh, this->bvh);
  puts("finished building bvh tree");
}

void Scene::build(BVHNode* root, BVHNode* bvh)
{
  if(bvh->r - bvh->l  + 1 <= 8)
  {
    if(bvh->triangles.empty()){
      for(int i = bvh->l; i <= bvh->r; ++i)
        bvh->triangles.push_back(root->triangles[i]);
    }
    return;
  }
  Vec3f minn(1e7, 1e7, 1e7);
  Vec3f maxn(-1e7, -1e7, -1e7);
  for(int i = bvh->l; i <= bvh->r ; ++i)
  {
    minn = minn.cwiseMin(root->triangles[i].center);
    maxn = maxn.cwiseMax(root->triangles[i].center);
  }
  Vec3f delta = maxn - minn;
  if(delta[0] > delta[1] && delta[0] > delta[2])
    std::sort(this->bvh->triangles.begin() + bvh->l, this->bvh->triangles.begin() + bvh->r + 1, [](Triangle a, Triangle b){return a.center[0] < b.center[0];});
  else if (delta[1] > delta[0] && delta[1] > delta[2])
    std::sort(this->bvh->triangles.begin() + bvh->l, this->bvh->triangles.begin() + bvh->r + 1, [](Triangle a, Triangle b){return a.center[1] < b.center[1];});
  else
    std::sort(this->bvh->triangles.begin() + bvh->l, this->bvh->triangles.begin() + bvh->r + 1, [](Triangle a, Triangle b){return a.center[2] < b.center[2];});

  int mid = (bvh->l + bvh->r) / 2;

  BVHNode* left = new BVHNode();
  left->l = bvh->l;
  left->r = mid;

  BVHNode* right = new BVHNode();
  right->l = mid + 1;
  right->r = bvh->r;

  AABB aabb(this->vertices[root->triangles[left->l].v_idx[0]], this->vertices[root->triangles[left->l].v_idx[1]], this->vertices[root->triangles[left->l].v_idx[2]]);
  left->aabb = aabb;
  for(int i = left->l + 1; i <= left->r; ++i)
  {
    aabb = AABB(this->vertices[root->triangles[i].v_idx[0]], this->vertices[root->triangles[i].v_idx[1]], this->vertices[root->triangles[i].v_idx[2]]);
    left->aabb = AABB(left->aabb, aabb);
  }
  
  aabb = AABB(this->vertices[root->triangles[right->l].v_idx[0]], this->vertices[root->triangles[right->l].v_idx[1]], this->vertices[root->triangles[right->l].v_idx[2]]);
  right->aabb = aabb;
  for(int i = right->l + 1; i <= right->r; ++i)
  {
    aabb = AABB(this->vertices[root->triangles[i].v_idx[0]], this->vertices[root->triangles[i].v_idx[1]], this->vertices[root->triangles[i].v_idx[2]]);
    right->aabb = AABB(right->aabb, aabb);
  }
  
  bvh->left = left;
  bvh->right = right;

  build(root, left);
  build(root, right);
}

bool Scene::intersectOneTriangle(Ray &ray,
                                        Interaction &interaction,
                                        const Vec3i &v_idx,
                                        const Vec3i &n_idx) const {
  Vec3f v0 = vertices[v_idx[0]];
  Vec3f v1 = vertices[v_idx[1]];
  Vec3f v2 = vertices[v_idx[2]];
  Vec3f v0v1 = v1 - v0;
  Vec3f v0v2 = v2 - v0;
  Vec3f pvec = ray.direction.cross(v0v2);
  float det = v0v1.dot(pvec);

  float invDet = 1.0f / det;

  Vec3f tvec = ray.origin - v0;
  float u = tvec.dot(pvec) * invDet;
  if (u < 0 || u > 1) return false;
  Vec3f qvec = tvec.cross(v0v1);
  float v = ray.direction.dot(qvec) * invDet;
  if (v < 0 || u + v > 1) return false;
  float t = v0v2.dot(qvec) * invDet;
  if (t < ray.t_min || t > ray.t_max) return false;

  interaction.dist = t;
  interaction.pos = ray(t);
  interaction.normal = (u * normals[n_idx[1]] + v * normals[n_idx[2]]
      + (1 - u - v) * normals[n_idx[0]]).normalized();
  // interaction.material = bsdf;
  interaction.type = Interaction::Type::GEOMETRY;
  return true;
}


void Scene::bvhHit(BVHNode *p, Interaction &interaction,
                          Ray &ray) const {
  if (p == nullptr)
    return;
  float tin, tout;
  if(!p->aabb.intersect(ray, &tin, &tout))
    return;
  if(p->r - p->l +1 <= 8)
  {
    for(int i = 0; i < p->triangles.size();++i)
    {
      Interaction temp;
      if (intersectOneTriangle(ray, temp, p->triangles[i].v_idx, p->triangles[i].n_idx) && (temp.dist < interaction.dist)) {
        interaction = temp;
      }
    }
    return;
  }
  Interaction inter_l, inter_r;
  bvhHit(p->left, inter_l, ray);
  bvhHit(p->right, inter_r, ray);
  if(inter_l.type == Interaction::Type::NONE && inter_r.type == Interaction::Type::NONE)
    return;
  if(inter_l.dist < inter_r.dist)
    interaction = inter_l;
  else
    interaction = inter_r;
}