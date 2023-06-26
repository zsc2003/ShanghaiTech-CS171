#include "cloth.h"
#include "object.h"
#include "transform.h"

#include <iostream>
#include <cmath>

///////////////////
/// constructor   ///
///////////////////

RectCloth::RectCloth(Float cloth_weight,
                     const UVec2& mass_dim,
                     Float dx_local,
                     Float stiffness, Float damping_ratio, int m_bonus, Vec3 m_center) :
    Mesh(std::vector<MeshVertex>(mass_dim.x * mass_dim.y),
         std::vector<UVec3>((mass_dim.y - 1) * (mass_dim.x - 1) * 2),
         GL_STREAM_DRAW, GL_STATIC_DRAW,
         true),
    // private
    mass_dim(mass_dim),
    mass_weight(cloth_weight / Float(mass_dim.x * mass_dim.y)),
    dx_local(dx_local),
    stiffness(stiffness),
    damping_ratio(damping_ratio),
    is_fixed_masses(mass_dim.x * mass_dim.y),
    local_or_world_positions(mass_dim.x * mass_dim.y),
    world_velocities(mass_dim.x * mass_dim.y),
    world_accelerations(mass_dim.x * mass_dim.y),
    bonus(m_bonus),
    center(m_center){

  // initialize local positions
  const auto local_width = Float(mass_dim.x) * dx_local;
  const auto local_height = Float(mass_dim.y) * dx_local;

#pragma omp parallel for collapse(2)
  for (int ih = 0; ih < mass_dim.y; ++ih)
    for (int iw = 0; iw < mass_dim.x; ++iw)
      local_or_world_positions[Get1DIndex(iw, ih)] = Vec3(Float(iw) * dx_local - local_width * Float(0.5),
                                                          Float(ih) * dx_local - local_height * Float(0.5),
                                                          0);

  // initialize mesh vertices
  UpdateMeshVertices();

  // initialize mesh indices
#pragma omp parallel for collapse(2)
  for (int ih = 0; ih < mass_dim.y - 1; ++ih)
    for (int iw = 0; iw < mass_dim.x - 1; ++iw) {
      size_t i_indices = (size_t(ih) * size_t(mass_dim.x - 1) + size_t(iw)) << 1;

      auto i  = Get1DIndex(iw, ih);
      auto r  = Get1DIndex(iw + 1, ih);
      auto u  = Get1DIndex(iw, ih + 1);
      auto ru = Get1DIndex(iw + 1, ih + 1);

      indices[i_indices    ] = UVec3(i, r, u);
      indices[i_indices + 1] = UVec3(r, ru, u);
    }
  glBindVertexArray(vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(UVec3) * indices.size(), indices.data(), GL_STATIC_DRAW);
  glBindVertexArray(0);
}



//////////////////
/// interfaces ///
//////////////////

bool RectCloth::SetMassFixedOrNot(int iw, int ih, bool fixed_or_not) {
  iw = iw < 0 ? int(mass_dim.x) + iw : iw;
  ih = ih < 0 ? int(mass_dim.y) + ih : ih;

  size_t idx;
  if (!Get1DIndex(iw, ih, idx))
    return false;

  is_fixed_masses[idx] = fixed_or_not;
  return true;
}

/*override*/ void RectCloth::FixedUpdate(Vec3 mouse, bool left, bool middle) {
  // simulate
  this->mouse = mouse, this->left = left, this->middle = middle;
  Simulate(simulation_steps_per_fixed_update_time);

  // update mesh vertices
  UpdateMeshVertices();
}



/////////////////////////
/// force computation ///
/////////////////////////

Vec3 RectCloth::ComputeHookeForce(int iw_this, int ih_this,
                                  int iw_that, int ih_that,
                                  Float dx_world) const {

  /*! TODO: implement this: compute the force according to Hooke's law
   *                        applied to mass(iw_this, ih_this)
   *                                by mass(iw_that, ih_that)
   *                        `dx_world` is "the zero-force distance" in world coordinate
   *
   *        note: for invalid `iw` or `ih`, you may simply return { 0, 0, 0 }
   */
  size_t idx1 = this->Get1DIndex(iw_this, ih_this);
  size_t idx2 = this->Get1DIndex(iw_that, ih_that);
  Vec3 pos1 = this->local_or_world_positions[idx1];
  Vec3 pos2 = this->local_or_world_positions[idx2];
  Vec3 delta = pos1 - pos2;
  // std::cout<<delta.x<<' '<<delta.y<<' '<<delta.z<<"\n\n"<<glm::length(delta)<<"\n\n";
  Vec3 F = this->stiffness * (dx_world - glm::length(delta)) * delta / glm::length(delta);
  // std::cout<<F.x<<' '<<F.y<<' '<<F.z<<'\n';
  return F;
}

Vec3 RectCloth::ComputeSpringForce(int iw, int ih) const {

  const Vec3 scale = object->transform->scale;

  /*! TODO: implement this: compute the total spring force applied to mass(iw, ih)
   *                        by some other neighboring masses
   *
   *        note: you MUST consider structural, shear, and bending springs
   *              you MUST consider scaling of "the zero-force distance"
   *              you may find ComputeHookeForce() helpful
   *              for invalid `iw` or `ih`, you may simply return { 0, 0, 0 }
   *              for "fixed masses", you may also simply return { 0, 0, 0 }
   */

  Vec3 F(0.0f, 0.0f, 0.0f);

  size_t idx = this->Get1DIndex(iw, ih);
  if(idx < 0 || idx >= this->mass_dim.x * this->mass_dim.y)
    return F;
  if(this->is_fixed_masses[idx])
    return F;

  float dis = this->dx_local;

  //structure
  if(iw > 0)
    F += this->ComputeHookeForce(iw, ih, iw - 1, ih, dis);
  if(ih > 0)
    F += this->ComputeHookeForce(iw, ih, iw, ih - 1, dis);
  if(iw < this->mass_dim.x - 1)
    F += this->ComputeHookeForce(iw, ih, iw + 1, ih, dis);
  if(ih < this->mass_dim.y - 1)
    F += this->ComputeHookeForce(iw, ih, iw, ih + 1, dis);

  //shear
  if(iw > 0 && ih > 0)
    F += this->ComputeHookeForce(iw, ih, iw - 1, ih - 1, dis * sqrt(2.0f));
  if(iw < this->mass_dim.x - 1 && ih > 0)
    F += this->ComputeHookeForce(iw, ih, iw + 1, ih - 1, dis * sqrt(2.0f));
  if(iw > 0 && ih < this->mass_dim.y - 1)
    F += this->ComputeHookeForce(iw, ih, iw - 1, ih + 1, dis * sqrt(2.0f));
  if(iw < this->mass_dim.x - 1 && ih < this->mass_dim.y - 1)
    F += this->ComputeHookeForce(iw, ih, iw + 1, ih + 1, dis * sqrt(2.0f));
  
  //flexion(bending)
  if(iw > 1)
    F += this->ComputeHookeForce(iw, ih, iw - 2, ih, dis * 2.0f);
  if(ih > 1)
    F += this->ComputeHookeForce(iw, ih, iw, ih - 2, dis * 2.0f);
  if(iw < this->mass_dim.x - 2)
    F += this->ComputeHookeForce(iw, ih, iw + 2, ih, dis * 2.0f);
  if(ih < this->mass_dim.y - 2)
    F += this->ComputeHookeForce(iw, ih, iw, ih + 2, dis * 2.0f);

  return F;
}



///////////////////////////
/// simulation pipeline ///
///////////////////////////

void RectCloth::LocalToWorldPositions() {

  const Mat4 model_matrix = object->transform->ModelMat();

  /*! TODO: implement this: transform mass positions from local coordinate to world coordinate
   *
   *        note: you may find `class Transform` in `transform.h` helpful
   */

  // std::cout<<center.x<<' '<<center.y<<' '<<center.z<<'\n';

  // this->center = Vec3(model_matrix * Vec4(this->center, 1.0f));

  for(int iw = 0; iw < this->mass_dim.x; ++iw)
    for(int ih = 0; ih < this->mass_dim.y; ++ih)
    {
      size_t idx = this->Get1DIndex(iw, ih);
      this->local_or_world_positions[idx] = Vec3(model_matrix * Vec4(this->local_or_world_positions[idx], 1.0f));
      // if(this->is_fixed_masses[idx]==1)std::cout<<this->local_or_world_positions[idx].x<<' '<<this->local_or_world_positions[idx].y<<' '<<this->local_or_world_positions[idx].z<<'\n';
    }

 /* if(this->left == false) {
    return;
  }
  int iw_c, ih_c;
  float distant = 1e9f;
  bool if_selected = false;
  // std::cout<<"test\n";
  for(int iw = 0; iw < this->mass_dim.x; ++iw)
    for(int ih = 0; ih < this->mass_dim.y; ++ih)
    {
      size_t idx = this->Get1DIndex(iw, ih);
      Vec3 veca = this->local_or_world_positions[idx] - object->transform->position;
      Vec3 vecb = this->mouse - object->transform->position;
      float dis = glm::length(glm::cross(veca, vecb)) / glm::length(vecb);
      if((iw == 0 && ih == 29) || (iw == 39 && ih == 29))
      {
        // std::cout<<dis<<'\n';
        continue;

      }
      if(dis < distant && dis < 0.5f)
      {
        distant = dis;
        iw_c = iw;
        ih_c = ih;
        if_selected = true;
        puts("test");
      }
      
    }


  if (if_selected) {
    SetMassFixedOrNot(iw_c, ih_c, true);
  }*/
  // if(distant < 1.0f)
  // {
  //   if(this->left == 1)
  //   {
  //     if(this->chose == 0)
  //     {
  //       // this->is_fixed_masses[idx] = 1;
  //       SetMassFixedOrNot(iw_c, ih_c, true);
  //       this->chose = 1;
  //     }
  //     // else
  //       // this->local_or_world_positions[this->Get1DIndex(iw_c, ih_c)] = this->mouse;
  //   }
   /* else if(this->middle == 0)
    {
      SetMassFixedOrNot(iw_c, ih_c, false);
      // this->is_fixed_masses[idx] = 0;
    }*/
  //this->UpdateMeshVertices();
}



bool RectCloth::DragUpdate(Vec3 mouse, Vec3 CameraPos)
{
  this->LocalToWorldPositions();

  float distant = 0.1f;
  int iw_c = -1, ih_c = -1;

  for(int iw = 0; iw < this->mass_dim.x; ++iw)
    for(int ih = 0; ih < this->mass_dim.y; ++ih)
    {
      if(iw == 0 && ih == this->mass_dim.y - 1)
        continue;
      if(iw == this->mass_dim.x - 1 && ih == this->mass_dim.y - 1)
        continue;
      size_t idx = this->Get1DIndex(iw, ih);
      Vec3 veca = this->local_or_world_positions[idx] - CameraPos;
      Vec3 vecb = mouse - CameraPos;
      float dis = glm::length(glm::cross(veca, vecb)) / glm::length(vecb);
      if(dis < distant)
      {
        distant = dis;
        iw_c = iw;
        ih_c = ih;
        this->factor = glm::length(this->local_or_world_positions[idx] - CameraPos) / glm::length(mouse - CameraPos);
      }
    }

  for(int iw = 0; iw < this->mass_dim.x; ++iw)
    for(int ih = 0; ih < this->mass_dim.y; ++ih)
    {
      size_t idx = this->Get1DIndex(iw, ih);
      this->world_velocities[idx] = Vec3(0.0f, 0.0f, 0.0f);
      this->world_accelerations[idx] = Vec3(0.0f, 0.0f, 0.0f);
    }

  this->WorldToLocalPositions();

  if(iw_c == -1)
    return false;
  
  this->iw_fix = iw_c;
  this->ih_fix = ih_c;
  // std::cout<<iw_fix<<' '<<iw_fix<<' '<<distant<<'\n';
  // std::cout<<object->transform->position.x<<' '<<object->transform->position.y<<' '<<object->transform->position.z<<'\n';
  this->SetMassFixedOrNot(iw_c, ih_c, true);
  this->UpdateMeshVertices();
  return true;
}

void RectCloth::move_fix(Vec3 mouse, bool mid, Vec3 CameraPos)
{
    
  if(this->iw_fix == -1)
    return;
  
  this->LocalToWorldPositions();
  size_t idx = this->Get1DIndex(this->iw_fix, this->ih_fix);
  this->local_or_world_positions[idx] = this->factor * (mouse - CameraPos) + CameraPos;
  this->WorldToLocalPositions();
  // std::cout<<mid<<'\n';
  if(mid)
  {
    // puts("dealing with un-fixed");

    this->SetMassFixedOrNot(this->iw_fix, this->ih_fix, false);
    for(int iw = 0; iw < this->mass_dim.x; ++iw)
      for(int ih = 0; ih < this->mass_dim.y; ++ih)
      {
        size_t idx = this->Get1DIndex(iw, ih);
        this->world_velocities[idx] = Vec3(0.0f, 0.0f, 0.0f);
        this->world_accelerations[idx] = Vec3(0.0f, 0.0f, 0.0f);
      }
    this->iw_fix = -1;
    this->ih_fix = -1;
  }
}


void RectCloth::ComputeAccelerations() {

  /*! TODO: implement this: compute accelerations for each mass
   *
   *        note: you may find ComputeSpringForce() helpful
   *              you may store the results into `world_accelerations`
   */
  int axis = this->wind_time / 100;
  if (axis >= 40)
    axis = 80 - axis;
  // std::cout<<axis<<'\n';

  for(size_t iw = 0; iw < this->mass_dim.x ; ++iw)
    for(size_t ih = 0; ih < this->mass_dim.y ; ++ih)
    {
      size_t idx = this->Get1DIndex(iw, ih);
      Vec3 F(0.0f, 0.0f, 0.0f);
      if (this->is_fixed_masses[idx] == false)
      {
        Vec3 F_spring = this->ComputeSpringForce(iw, ih);
        Vec3 F_G = - this->mass_weight * gravity;
        Vec3 F_dampling = - this->damping_ratio * this->world_velocities[idx];

        Vec3 wind(0, 0, 1.0f);
        float strength = 0.0f;
        if(bonus == 1)
        {
          strength = 0.05f * exp(-(double)(axis - iw) * (double)(axis - iw) / (double)400 );
          // if(iw == 0 && axis == 40) std::cout<<strength<<'\n';
        }
        
        F = F_spring + F_G + F_dampling + wind * strength;
      }
      this->world_accelerations[idx] = F / this->mass_weight;
    }

}

Vec3 RectCloth::interaction(size_t idx)
{
  
  if(glm::length(this->local_or_world_positions[idx] - this->center) <= 1.0f + 1e-2f)
  {
    return glm::normalize(this->local_or_world_positions[idx] - this->center) * 1.0f;
  }
  return Vec3(0,0,0);
}

void RectCloth::ComputeVelocities() {

  /*! TODO: implement this: update velocities for each mass
   *
   *        note: you may store the results into `world_velocities`
   *              you may use `this->fixed_delta_time` instead of `Time::fixed_delta_time`, why?
   */
  // std::cout<<center.x<<' '<<center.y<<' '<<center.z<<'\n';
  for(size_t iw = 0; iw < this->mass_dim.x; ++iw)
    for(size_t ih = 0; ih < this->mass_dim.y; ++ih)
    {
      size_t idx = this->Get1DIndex(iw, ih);
      
      Vec3 normal = this->interaction(idx);
      normal = -normal;
      if(glm::length(normal) > 0.1f)
      {
        float cosine = glm::dot(this->world_velocities[idx], normal) / glm::length(this->world_velocities[idx]) / glm::length(normal);
        Vec3 v = glm::length(this->world_velocities[idx]) * cosine * normal;
         this->world_velocities[idx] -= v;
        // std::cout<<glm::length(normal)<<'\n';
         
        // this->world_velocities[idx] = Vec3(0,0,0);
      }
      else
       this->world_velocities[idx] += this->fixed_delta_time * this->world_accelerations[idx];

    }
}

void RectCloth::ComputePositions() {

  /*! TODO: implement this: update positions for each mass
   *
   *        note: you may store the results into `local_or_world_positions`
   *              you may use `this->fixed_delta_time` instead of `Time::fixed_delta_time`, why?
   */
  for(size_t iw = 0; iw < this->mass_dim.x; ++iw)
    for(size_t ih = 0; ih < this->mass_dim.y; ++ih)
    {
      size_t idx = this->Get1DIndex(iw, ih);
      
      // std::cout<<"----------\n"<<this->local_or_world_positions[idx].x<<' '<<this->local_or_world_positions[idx].y<<' '<<this->local_or_world_positions[idx].z<<'\n';
      // std::cout<<this->local_or_world_positions[idx].x<<' '<<this->local_or_world_positions[idx].y<<' '<<this->local_or_world_positions[idx].z<<"----------\n";

      Vec3 normal = this->interaction(idx);
      if(glm::length(normal) > 0.1f)
      {
        // this->local_or_world_positions[idx] = this->center + 1.00001f * normal;// * 1.0001f;
        this->local_or_world_positions[idx] += 0.01f * normal;// * 1.0001f;

        // Vec3 delta = this->local_or_world_positions[idx] - this->center - normal;
        // std::cout<<delta.x<<' '<<delta.y<<" "<<delta.z<<'\n';
      }
      else
        this->local_or_world_positions[idx] += this->fixed_delta_time * this->world_velocities[idx];

    }
    
  // puts("working in position changing");
}

void RectCloth::WorldToLocalPositions() {

  const Mat4 model_matrix = object->transform->ModelMat();

  /*! TODO: implement this: transform mass positions from world coordinate to local coordinate
   *
   *        note: you may find `class Transform` in `transform.h` helpful
   */

  // this->center = Vec3(glm::inverse(model_matrix) * Vec4(this->center, 1.0f));

  for(int iw = 0; iw < this->mass_dim.x; ++iw)
    for(int ih = 0; ih < this->mass_dim.y; ++ih)
    {
      size_t idx = this->Get1DIndex(iw, ih);
      this->local_or_world_positions[idx] = Vec3(glm::inverse(model_matrix) * Vec4(this->local_or_world_positions[idx], 1.0f));
      
    }

  
}

void RectCloth::Simulate(unsigned num_steps) {
  for (unsigned i = 0; i < num_steps; ++i) {
    // size_t idx1 = this->Get1DIndex(0, 0), idx2 = this->Get1DIndex(39, 0), idx3 = this->Get1DIndex(0, 29), idx4 = this->Get1DIndex(39, 29);
    // std::cout<<this->local_or_world_positions[idx1].x<<' '<<this->local_or_world_positions[idx1].y<<' '<<this->local_or_world_positions[idx1].z<<'\n'; 
    // std::cout<<this->local_or_world_positions[idx2].x<<' '<<this->local_or_world_positions[idx2].y<<' '<<this->local_or_world_positions[idx2].z<<'\n'; 
    // std::cout<<this->local_or_world_positions[idx3].x<<' '<<this->local_or_world_positions[idx3].y<<' '<<this->local_or_world_positions[idx3].z<<'\n'; 
    // std::cout<<this->local_or_world_positions[idx4].x<<' '<<this->local_or_world_positions[idx4].y<<' '<<this->local_or_world_positions[idx3].z<<'\n'; 
    this->wind_time++;
    this->wind_time %= 8000;

    // std::cout<<this->mouse.x<<" "<<mouse.y<<'\n';
    // std::cout<<this->left<<' '<<this->middle<<'\n';
    if(this->bonus == 3)
    {
      if(this->middle == 1)
      {
        this->chose = 0;
      }
    }

    LocalToWorldPositions();
    ComputeAccelerations();
    ComputeVelocities();
    ComputePositions();

    // std::cout<<this->local_or_world_positions[idx1].x<<' '<<this->local_or_world_positions[idx1].y<<' '<<this->local_or_world_positions[idx1].z<<'\n'; 
    // std::cout<<this->local_or_world_positions[idx2].x<<' '<<this->local_or_world_positions[idx2].y<<' '<<this->local_or_world_positions[idx2].z<<'\n'; 
    // std::cout<<this->local_or_world_positions[idx3].x<<' '<<this->local_or_world_positions[idx3].y<<' '<<this->local_or_world_positions[idx3].z<<'\n'; 
    // std::cout<<this->local_or_world_positions[idx4].x<<' '<<this->local_or_world_positions[idx4].y<<' '<<this->local_or_world_positions[idx3].z<<'\n'; 
    
    WorldToLocalPositions();
  }
}



/////////////////
/// rendering ///
/////////////////

void RectCloth::UpdateMeshVertices() {

  // set vertex positions
  for (size_t i = 0; i < local_or_world_positions.size(); ++i)
    vertices[i].position = local_or_world_positions[i];

  // reset vertex normals
  auto compute_normal = [&](auto v1, auto v2, auto v3) {
    return glm::normalize(glm::cross(vertices[v2].position - vertices[v1].position, vertices[v3].position - vertices[v1].position));
  };

#pragma omp parallel for collapse(2)
  for (int ih = 0; ih < mass_dim.y; ++ih)
    for (int iw = 0; iw < mass_dim.x; ++iw) {
      constexpr Float w_small = Float(0.125);
      constexpr Float w_large = Float(0.25);

      auto i  = Get1DIndex(iw, ih);
      auto l  = Get1DIndex(iw - 1, ih);
      auto r  = Get1DIndex(iw + 1, ih);
      auto u  = Get1DIndex(iw, ih + 1);
      auto d  = Get1DIndex(iw, ih - 1);
      auto lu = Get1DIndex(iw - 1, ih + 1);
      auto rd = Get1DIndex(iw + 1, ih - 1);
      auto& normal = vertices[i].normal;

      normal = { 0, 0, 0 };

      if (iw > 0 && ih < mass_dim.y - 1) {
        normal += compute_normal(l, i, lu) * w_small;
        normal += compute_normal(i, u, lu) * w_small;
      }
      if (iw < mass_dim.x - 1 && ih < mass_dim.y - 1) {
        normal += compute_normal(i, r, u) * w_large;
      }
      if (iw > 0 && ih > 0) {
        normal += compute_normal(l, d, i) * w_large;
      }
      if (iw < mass_dim.x - 1 && ih > 0) {
        normal += compute_normal(d, rd, i) * w_small;
        normal += compute_normal(rd, r, i) * w_small;
      }

      normal = glm::normalize(normal);
    }

  // vbo
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(MeshVertex) * vertices.size(), vertices.data(), buffer_data_usage_vbo);
  glBindVertexArray(0);
}



//////////////////////////
/// supporting methods ///
//////////////////////////

size_t RectCloth::Get1DIndex(int iw, int ih) const {
  return size_t(ih) * size_t(mass_dim.x) + size_t(iw);
}

bool RectCloth::Get1DIndex(int iw, int ih, size_t &idx) const {
  if (iw < 0 || ih < 0 || iw >= mass_dim.x || ih >= mass_dim.y)
    return false;
  idx = size_t(ih) * size_t(mass_dim.x) + size_t(iw);
  return true;
}
