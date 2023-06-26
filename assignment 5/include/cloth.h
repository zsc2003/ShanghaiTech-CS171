#pragma once

#include "mesh.h"
#include "time_system.h"

class RectCloth : public Mesh {
 public:

  /// constructor

  RectCloth(Float cloth_weight,
            const UVec2& mass_dim,
            Float dx_local,
            Float stiffness, Float damping_ratio, int m_bonus, Vec3 m_center);

  RectCloth(const RectCloth&) = default;
  RectCloth(RectCloth&&) = default;
  RectCloth& operator=(const RectCloth&) = default;
  RectCloth& operator=(RectCloth&&) = default;
  virtual ~RectCloth() override = default;

  Vec3 interaction(size_t idx);
  Vec3 center;

  /// interfaces

  bool SetMassFixedOrNot(int iw, int ih, bool fixed_or_not);

  Vec3 mouse;
  bool left{0}, middle{0};
  bool chose{0};
  int iw_fix, ih_fix;
  float factor;

  virtual void FixedUpdate(Vec3 mouse, bool left, bool middle) override;
  virtual bool DragUpdate(Vec3 mouse, Vec3 CameraPos) override;
  virtual void move_fix(Vec3 mouse, bool mid, Vec3 CameraPos) override;


 private:
  static constexpr unsigned simulation_steps_per_fixed_update_time = 20;
  static constexpr Float fixed_delta_time = Time::fixed_delta_time / Float(simulation_steps_per_fixed_update_time);

  UVec2 mass_dim;
  Float mass_weight;

  Float dx_local;

  Float stiffness;
  Float damping_ratio;

  std::vector<bool> is_fixed_masses;
  std::vector<Vec3> local_or_world_positions;
  std::vector<Vec3> world_velocities;
  std::vector<Vec3> world_accelerations;

  int bonus;
  int wind_time;

  /// force computation

  [[nodiscard]] Vec3 ComputeHookeForce(int iw_this, int ih_this,
                                       int iw_that, int ih_that,
                                       Float dx_world) const;

  [[nodiscard]] Vec3 ComputeSpringForce(int iw, int ih) const;



  /// simulation pipeline

  void LocalToWorldPositions();

  void ComputeAccelerations();

  void ComputeVelocities();

  void ComputePositions();

  void WorldToLocalPositions();

  void Simulate(unsigned num_steps);



  /// rendering

  void UpdateMeshVertices();



  /// supporting methods

  [[nodiscard]] size_t Get1DIndex(int iw, int ih) const;
  bool Get1DIndex(int iw, int ih, size_t& idx) const;
};
