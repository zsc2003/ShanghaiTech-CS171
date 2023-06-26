#include "geometry.h"
#include "accel.h"

#include <utility>
#include <iostream>
TriangleMesh::TriangleMesh(std::vector<Vec3f> vertices, std::vector<Vec3f> normals,
                           std::vector<int> v_index, std::vector<int> n_index) :
    vertices(std::move(vertices)),
    normals(std::move(normals)),
    v_indices(std::move(v_index)),
    n_indices(std::move(n_index)),
    bvh(nullptr) {}

bool TriangleMesh::intersect(Ray &ray, Interaction &interaction) const {
  if (bvh != nullptr) {
    // puts("bvh start hitting");
    // std::cout<<bvh->l<<' '<<bvh->r<<'\n';
    bvhHit(bvh, interaction, ray);
    // puts("bvh finished hitting");
  }
  else {
    // If you did not implement BVH
    // directly loop through all triangles in the mesh and test intersection for each triangle.
    for (int i = 0; i < v_indices.size() / 3; i++) {
      Vec3i v_idx(v_indices[3 * i], v_indices[3 * i + 1], v_indices[3 * i + 2]);
      Vec3i n_idx(n_indices[3 * i], n_indices[3 * i + 1], n_indices[3 * i + 2]);
      Interaction temp;
      if (intersectOneTriangle(ray, temp, v_idx, n_idx) && (temp.dist < interaction.dist)) {
        // std::cout<<temp.dist <<'\n';
        interaction = temp;
      }
    }
  }
  return interaction.type != Interaction::Type::NONE;
}

void TriangleMesh::setMaterial(std::shared_ptr<BSDF> &new_bsdf) {
  bsdf = new_bsdf;
}

void TriangleMesh::build(BVHNode* root, BVHNode* bvh)
{
  // std::cout<<bvh->l<<' '<<bvh->r<<'\n';
  // std::cout<<bvh->aabb.low_bnd<<'\n'<<bvh->aabb.upper_bnd<<'\n'<<'\n'<<'\n';
  if(bvh->r - bvh->l  + 1 <= 8)
  {
    // std::cout<<bvh->l<<"   "<<bvh->r <<'\n';

    if(bvh->triangles.empty()){
      for(int i = bvh->l; i <= bvh->r; ++i)
        bvh->triangles.push_back(root->triangles[i]);
    }
    return;
  }
  // if(bvh->l == bvh->r)
  // {
  //   bvh->triangles.push_back(root->triangles[bvh->l]);
  //   return;
  // }

  Vec3f minn(1e7, 1e7, 1e7);
  Vec3f maxn(-1e7, -1e7, -1e7);
  for(int i = bvh->l; i <= bvh->r ; ++i)
  {
    minn = minn.cwiseMin(root->triangles[i].center);
    maxn = maxn.cwiseMax(root->triangles[i].center);
  }
  Vec3f delta = maxn - minn;
  if(delta[0] > delta[1] && delta[0] > delta[2])
  {
    std::sort(this->bvh->triangles.begin() + bvh->l, this->bvh->triangles.begin() + bvh->r + 1, [](Triangle a, Triangle b){return a.center[0] < b.center[0];});
  }

  else if (delta[1] > delta[0] && delta[1] > delta[2])
  {
    std::sort(this->bvh->triangles.begin() + bvh->l, this->bvh->triangles.begin() + bvh->r + 1, [](Triangle a, Triangle b){return a.center[1] < b.center[1];});
  }
  else
  {
    std::sort(this->bvh->triangles.begin() + bvh->l, this->bvh->triangles.begin() + bvh->r + 1, [](Triangle a, Triangle b){return a.center[2] < b.center[2];});
  }


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

  // puts("111");
  // bvh->aabb = AABB(left->aabb, right->aabb);
  // puts("222");
}

void TriangleMesh::buildBVH() {
  // TODO: your implementation
  puts("start building bvh tree");
  Triangle tri;
  
  this->bvh = new BVHNode();
  // puts("111");
  // std::cout<<(this->bvh == nullptr)<<'\n';
  // std::cout<<this->bvh->triangles.size()<<'\n';
  // puts("222");

  for (int i = 0; i < this->v_indices.size() / 3; i++)
  {
    tri.v_idx = Vec3i(v_indices[3 * i], v_indices[3 * i + 1], v_indices[3 * i + 2]);
    tri.n_idx = Vec3i(n_indices[3 * i], n_indices[3 * i + 1], n_indices[3 * i + 2]);
    tri.center = (this->vertices[tri.v_idx[0]] + this->vertices[tri.v_idx[1]] + this->vertices[tri.v_idx[2]]) / 3.0f;
    this->bvh->triangles.push_back(tri);
  }

  /*Vec3f minn(1e7, 1e7, 1e7);
  Vec3f maxn(-1e7, -1e7, -1e7);
  for(auto tri : bvh->triangles)
  {
    minn = minn.cwiseMin(tri.center);
    maxn = maxn.cwiseMax(tri.center);
  }
  Vec3f delta = maxn - minn;
  int dim;
  if(delta[0] > delta[1] && delta[0] > delta[2])
  {
    dim = 0;
    std::sort(this->bvh->triangles.begin(), this->bvh->triangles.end(), [](Triangle a, Triangle b){return a.center[0] < b.center[0];});
  }

  else if (delta[1] > delta[0] && delta[1] > delta[2])
  {
    dim = 1;
    std::sort(this->bvh->triangles.begin(), this->bvh->triangles.end(), [](Triangle a, Triangle b){return a.center[1] < b.center[1];});
  }
  else
  {
    dim = 2;
    std::sort(this->bvh->triangles.begin(), this->bvh->triangles.end(), [](Triangle a, Triangle b){return a.center[2] < b.center[2];});
  }*/
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
  build(this->bvh, this->bvh);
  puts("finished building bvh tree");
}

bool TriangleMesh::intersectOneTriangle(Ray &ray,
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
  interaction.material = bsdf;
  interaction.type = Interaction::Type::GEOMETRY;
  return true;
}


void TriangleMesh::bvhHit(BVHNode *p, Interaction &interaction,
                          Ray &ray) const {
  // TODO: traverse through the bvh and do intersection test efficiently.
  // puts("bvh hit");
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
  // if(p->l == p->r)
  // {
  //   Interaction temp;
  //   if (intersectOneTriangle(ray, temp, p->triangles[0].v_idx, p->triangles[0].n_idx) && (temp.dist < interaction.dist)) {
  //     interaction = temp;
  //   }
  //   return;
  // }
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
