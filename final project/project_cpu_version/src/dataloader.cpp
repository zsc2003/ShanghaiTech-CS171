#include "dataloader.h"
#include<cmath>

// construct function
// path data file path
// x dimension x
// y dimension y
// z dimension z
DataLoader::DataLoader(std::string path, int x, int y, int z, int x_size, int y_size, int z_size) : width(x), height(y), length(z), sizex(x_size), sizey(y_size), sizez(z_size) {
    VoxelData.resize(width);
    for (int i = 0; i < width; i++) {
        VoxelData[i].resize(height);
        for (int j = 0; j < height; j++) {
            VoxelData[i][j].resize(length);
        }
    }
    // get the file type
    std::string type = path.substr(path.length()-3, 3);
    if (type != "raw" && type != "RAW") {
        std::cout<<"Invalid data type."<<std::endl;
    } else {
        std::cout<<"Detect raw file, read from : "<<path<<"."<<std::endl;
    }
    lengthraw = x * y * z;
    // note that decide unint16_t or unint8_t to read file
    uint8_t temp;
    std::ifstream inFile(path, std::ios::in|std::ios::binary);
    if(!inFile) {
        std::cout << "load raw file error." << std::endl;
    }
    if (sizex >= sizey && sizex >= sizez) {
        worldsize.x() = 2.0f; worldsize.y() = (worldsize.x() / sizex) * sizey; worldsize.z() = (worldsize.x() / sizex) * sizez;
    } else if (sizey >= sizex && sizey >= sizez) {
        worldsize.y() = 2.0f; worldsize.x() = (worldsize.y() / sizey) * sizex; worldsize.z() = (worldsize.y() / sizey) * sizez;
    } else {
        worldsize.z() = 2.0f; worldsize.x() = (worldsize.z() / sizez) * sizex; worldsize.y() = (worldsize.z() / sizez) * sizey;
    }
    Vec3f unitsize{worldsize.x() / width, worldsize.y() / height, worldsize.z() / length};
    Vec3f testtemp = OriginVoxel + xVector * unitsize.x() * (0 - (length / 2) - 0.5) + yVector * unitsize.y() * (0 - (height / 2) - 0.5) + zVector * unitsize.z() * (0 - (width / 2) - 0.5);
    boundingbox = AABB(OriginVoxel + xVector * unitsize.x() * (0 - (length / 2) - 0.5) + yVector * unitsize.y() * (0 - (height / 2) - 0.5) + zVector * unitsize.z() * (0 - (width / 2) - 0.5), 
                              OriginVoxel + xVector * unitsize.x() * (length - (length / 2) + 0.5) + yVector * unitsize.y() * (height - (height / 2) + 0.5) + zVector * unitsize.z() * (width - (width / 2) + 0.5));
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            for (int k = 0; k < length; k++) {
                inFile.read((char*)&temp, sizeof(temp));
                AABB aabbtemp(OriginVoxel + xVector * unitsize.x() * (k - (length / 2) - 0.5) + yVector * unitsize.y() * (j - (height / 2) - 0.5) + zVector * unitsize.z() * (i - (width / 2) - 0.5), 
                              OriginVoxel + xVector * unitsize.x() * (k - (length / 2) + 0.5) + yVector * unitsize.y() * (j - (height / 2) + 0.5) + zVector * unitsize.z() * (i - (width / 2) + 0.5)
                );
                UnitVoxel voxel(aabbtemp);
                voxel.transfer(temp);
                VoxelData[i][j][k] = voxel;
            }
        }
    }
    delete rawdata;
}


bool DataLoader::check(const Ray &ray, float *t_min, float *t_max)
{
    bool flag = bvhHit(this->bvh, ray, t_min, t_max);
    flag = this->boundingbox.intersect(ray, t_min, t_max);
    if(flag == false)
        return false;
    
    return blockwalking(ray, t_min, t_max);
}

bool DataLoader::blockwalking(const Ray &ray, float *t_min, float *t_max)
{
    float t,temp_min;
    // puts("1111");

    bool flag = false;
    
    while((*t_min) + 1e-6f < (*t_max))
    {
        Vec3f point(ray.origin + ray.direction * (*t_min + 1e-6f));

        int xcoor = std::ceil((float)(point - this->OriginVoxel).x() / (2.0f / (float)this->length)) + this->length / 2;
        int ycoor = std::ceil((float)(point - this->OriginVoxel).y() / (2.0f / (float)this->width)) + this->width / 2;
        int zcoor = std::ceil((float)(point - this->OriginVoxel).z() / (2.0f / (float)this->height)) + this->height / 2;

        xcoor--,ycoor--,zcoor--;
        // std::cout<<xcoor<<' '<<ycoor<<' '<<zcoor<<'\n';
        if (xcoor >= this->length - 1) {
            xcoor = this->length - 1;
        }
        if (ycoor >= this->width - 1) {
            ycoor = this->width - 1;
        }
        if (zcoor >= this->height - 1) {
            zcoor = this->height - 1;
        }
        if(xcoor < 0)
        {
            xcoor = 0;
        }
        if(ycoor < 0)
        {
            ycoor = 0;
        }
        if(zcoor < 0)
        {
            zcoor = 0;
        }
        break;
        if(this->VoxelData[xcoor][ycoor][zcoor].if_empty)
        {
            flag = this->VoxelData[xcoor][ycoor][zcoor].aabb.intersect(ray, &t, &temp_min);
            // std::cout<<(*t_min)<<' '<<t<<' '<<temp_min<<'\n';
            if((*t_min) < temp_min)
            {
                (*t_min) =temp_min;
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
    return true;
    // puts("2222");


    if((*t_min) + 1e-6f < (*t_max))
        return false;

    // puts("3333");
    float temp_max;
    while((*t_min) + 1e-6f < (*t_max))
    {
        Vec3f point(ray.origin + ray.direction * (*t_max - 0.0001f));

        int xcoor = std::ceil((float)(point - this->OriginVoxel).x() / (2.0f / (float)this->length)) + this->length / 2;
        int ycoor = std::ceil((float)(point - this->OriginVoxel).y() / (2.0f / (float)this->width)) + this->width / 2;
        int zcoor = std::ceil((float)(point - this->OriginVoxel).z() / (2.0f / (float)this->height)) + this->height / 2;

        // std::cout<<xcoor<<' '<<ycoor<<' '<<zcoor<<'\n';
        if (xcoor >= this->length - 1) {
            xcoor = this->length - 1;
        }
        if (ycoor >= this->width - 1) {
            ycoor = this->width - 1;
        }
        if (zcoor >= this->height - 1) {
            zcoor = this->height - 1;
        }
        if(xcoor < 0)
        {
            xcoor = 0;
        }
        if(ycoor < 0)
        {
            ycoor = 0;
        }
        if(zcoor < 0)
        {
            zcoor = 0;
        }
        break;
        // puts("5555");
        if(this->VoxelData[xcoor][ycoor][zcoor].if_empty)
        {
            // puts("6666");
            flag = this->VoxelData[xcoor][ycoor][zcoor].aabb.intersect(ray, &temp_max, &t);
            if((*t_max) > temp_max)
            {
                (*t_max) = temp_max;
            }
            else
            {
                break;
            }
        }
        else
        {
            // puts("6666");
            break;
        }
    }
    // puts("4444");

    if((*t_min) + 1e-6f < (*t_max))
        return false;
    return true;

}

void DataLoader::build(BVHNode* root, BVHNode* bvh)
{
  // std::cout<<bvh->l<<' '<<bvh->r<<'\n';
  // std::cout<<bvh->aabb.low_bnd<<'\n'<<bvh->aabb.upper_bnd<<'\n'<<'\n'<<'\n';
  if(bvh->r - bvh->l  + 1 <= 8)
  {
    // std::cout<<bvh->l<<"   "<<bvh->r <<'\n';

    if(bvh->voxels.empty()){
      for(int i = bvh->l; i <= bvh->r; ++i)
        bvh->voxels.push_back(root->voxels[i]);
    }
    return;
  }
  // if(bvh->l == bvh->r)
  // {
  //   bvh->triangles.push_back(root->triangles[bvh->l]);
  //   return;
  // }

  


  int mid = (bvh->l + bvh->r) / 2;

  BVHNode* left = new BVHNode();
  left->l = bvh->l;
  left->r = mid;

  BVHNode* right = new BVHNode();
  right->l = mid + 1;
  right->r = bvh->r;

  AABB aabb;
  left->aabb = this->VoxelData[root->voxels[left->l].x()][root->voxels[left->l].y()][root->voxels[left->l].z()].aabb;
  for(int i = left->l + 1; i <= left->r; ++i)
  {
    aabb = this->VoxelData[root->voxels[i].x()][root->voxels[i].y()][root->voxels[i].z()].aabb;
    left->aabb = AABB(left->aabb, aabb);
  }
  
  left->aabb = this->VoxelData[root->voxels[right->l].x()][root->voxels[right->l].y()][root->voxels[right->l].z()].aabb;
  for(int i = right->l + 1; i <= right->r; ++i)
  {
    aabb = this->VoxelData[root->voxels[i].x()][root->voxels[i].y()][root->voxels[i].z()].aabb;
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

void DataLoader::buildBVH() {
  // TODO: your implementation
  puts("start building bvh tree");
//   UnitVoxel voxel;
  
  this->bvh = new BVHNode();
  // puts("111");
  // std::cout<<(this->bvh == nullptr)<<'\n';
  // std::cout<<this->bvh->triangles.size()<<'\n';
  // puts("222");

  /*for (int i = 0; i < this->VoxelData.size(); i++)
  {
    // tri.v_idx = Vec3i(v_indices[3 * i], v_indices[3 * i + 1], v_indices[3 * i + 2]);
    // tri.n_idx = Vec3i(n_indices[3 * i], n_indices[3 * i + 1], n_indices[3 * i + 2]);
    // tri.center = (this->vertices[tri.v_idx[0]] + this->vertices[tri.v_idx[1]] + this->vertices[tri.v_idx[2]]) / 3.0f;
    this->bvh->voxels.push_back(this->voxels[i]);
  }*/
  for(int i = 0; i < this->length; ++i)
    for(int j = 0; j < this->width ; ++j)
        for(int k = 0; k < this->height; ++k)
        {
            this->bvh->voxels.push_back(Vec3i(i,j,k));
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
  /*this->bvh->l = 0;
  this->bvh->r = this->bvh->triangles.size() - 1;
  AABB aabb(this->vertices[this->bvh->triangles[0].v_idx[0]], this->vertices[this->bvh->triangles[0].v_idx[1]], this->vertices[this->bvh->triangles[0].v_idx[2]]);
  this->bvh->aabb = aabb;
  for(int i = 1; i < this->bvh->triangles.size(); ++i)
  {
    aabb = AABB(this->vertices[this->bvh->triangles[i].v_idx[0]], this->vertices[this->bvh->triangles[i].v_idx[1]], this->vertices[this->bvh->triangles[i].v_idx[2]]);
    this->bvh->aabb = AABB(this->bvh->aabb, aabb);
  }*/
  this->bvh->l = 0;
  this->bvh->r = this->bvh->voxels.size() - 1;
  AABB aabb = this->VoxelData[this->bvh->voxels[0].x()][this->bvh->voxels[0].y()][this->bvh->voxels[0].z()].aabb;
  this->bvh->aabb = aabb;
  for(int i = 1; i < this->bvh->voxels.size(); ++i)
  {
    aabb = this->VoxelData[this->bvh->voxels[i].x()][this->bvh->voxels[i].y()][this->bvh->voxels[i].z()].aabb;
    this->bvh->aabb = AABB(this->bvh->aabb, aabb);
  }

  puts("getting the function");
  build(this->bvh, this->bvh);
  puts("finished building bvh tree");

}


bool DataLoader::bvhHit(BVHNode *p, Ray ray, float *t_in, float *t_out) {
  // TODO: traverse through the bvh and do intersection test efficiently.
  // puts("bvh hit");
  if (p == nullptr)
    return false;
  
  float tin, tout;
  if(!p->aabb.intersect(ray, &tin, &tout))
    return false;
  
  return true;
  if(p->r - p->l + 1 <= 8)
  {
    bool inter = false;
    for(int i = 0; i < p->voxels.size(); ++i)
    {
        bool flag = this->VoxelData[this->bvh->voxels[i].x()][this->bvh->voxels[i].y()][this->bvh->voxels[i].z()].aabb.intersect(ray, &tin, &tout);
        if(flag == true)
        {
            (*t_in) = min((*t_in), tin);
            (*t_out) = max((*t_out), tout);
            inter = true;
        }
    }
    return inter;
  }
  // if(p->l == p->r)
  // {
  //   Interaction temp;
  //   if (intersectOneTriangle(ray, temp, p->triangles[0].v_idx, p->triangles[0].n_idx) && (temp.dist < interaction.dist)) {
  //     interaction = temp;
  //   }
  //   return;
  // }
  bvhHit(p->left, ray, t_in, t_out);
  bvhHit(p->right, ray, t_in, t_out);


//   if(inter_l.type == Interaction::Type::NONE && inter_r.type == Interaction::Type::NONE)
//     return false;
  
//   if(inter_l.dist < inter_r.dist)
//     interaction = inter_l;
//   else
//     interaction = inter_r;
}
