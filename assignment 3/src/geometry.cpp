#include "geometry.h"

#include <utility>
#include <iostream>

Triangle::Triangle(Vec3f v0, Vec3f v1, Vec3f v2)
    : v0(std::move(v0)), v1(std::move(v1)), v2(std::move(v2))
{
  normal = (v1 - v0).cross(v2 - v1).normalized();
}

bool Triangle::intersect(Ray &ray, Interaction &interaction) const
{
  // TODO: Your code here.
  float t, b1, b2;

  // td + (v0-v1)b1 + (v0-v2)b2 = v0 - o
  //  e1 = v1 - v0
  //  e2 = v2 - v0
  //  s = o - v0
  //  s1 = cross(d,e2)
  //  s2 = cross(s,e1)
  Vec3f o = ray.origin;
  Vec3f d = ray.direction;
  Vec3f e1 = this->v1 - this->v0;
  Vec3f e2 = this->v2 - this->v0;
  Vec3f s = o - this->v0;
  Vec3f s1 = d.cross(e2);
  Vec3f s2 = s.cross(e1);
  t = s2.dot(e2) / s1.dot(e1);
  b1 = s1.dot(s) / s1.dot(e1);
  b2 = s2.dot(d) / s1.dot(e1);

  if (d.norm() * t > interaction.dist)
    return false;
  if (t < ray.t_min || t > ray.t_max)
    return false;
  if (b1 < 0.0f - 1e-3f || b1 > 1.0f + 1e-3f)
    return false;
  if (b2 < 0.0f - 1e-3f || b2 > 1.0f + 1e-3f)
    return false;
  if ((1.0f - b1 - b2) < 0.0f - 1e-3f || (1.0f - b1 - b2) > 1.0f + 1e-3f)
    return false;

  interaction.type = Interaction::Type::GEOMETRY;
  interaction.pos = o + d * t;
  interaction.normal = this->normal;
  interaction.dist = d.norm() * t;
  if(this->material != nullptr)
    interaction.model = this->material->evaluate(interaction);
  interaction.uv[0] = b1;
  interaction.uv[1] = b2;
  return true;
}

Rectangle::Rectangle(Vec3f position, Vec2f dimension, Vec3f normal, Vec3f tangent)
    : Geometry(),
      position(std::move(position)),
      size(std::move(dimension)),
      normal(std::move(normal)),
      tangent(std::move(tangent)) {}

bool Rectangle::intersect(Ray &ray, Interaction &interaction) const
{
  // TODO: Your code here
  // (o + td - position) dot normal = 0
  // t = dot((position-o),normal)/dot(d,normal)
  Vec3f d = ray.direction;
  Vec3f o = ray.origin;
  if (fabs(d.dot(this->normal)) < 1e-7f)
    return false;
  float t = (this->position - o).dot(this->normal) / d.dot(this->normal);
  if (t < ray.t_min || t > ray.t_max)
    return false;
  if (d.norm() * t > interaction.dist)
    return false;
  Vec3f pos = ray(t);
  Vec3f tangent_y = this->normal.cross(this->tangent).normalized();
  /*if (position.x() - ((size.x() / 2.0f) * this->tangent).x() - 1e-3f > pos.x())
    return false;
  if (position.x() + ((size.x() / 2.0f) * this->tangent).x() + 1e-3f < pos.x())
    return false;
  if (position.y() - ((size.y() / 2.0f) * tangent_y).y() - 1e-3f > pos.y())
    return false;
  if (position.y() + ((size.y() / 2.0f) * tangent_y).y() + 1e-3f < pos.y())
    return false;*/
  Vec3f vec = pos - this->position;
  // puts("true");
  if(fabs(vec.dot(this->tangent)) > (size.x()/2.0f) + 1e-3)
    return false;
  if(fabs(vec.dot(tangent_y)) > (size.y()/2.0f) + 1e-3)
    return false;
  interaction.type = Interaction::Type::GEOMETRY;
  interaction.pos = o + d * t;
  interaction.normal = this->normal;
  interaction.dist = d.norm() * t;
  if(this->material != nullptr)
    interaction.model = this->material->evaluate(interaction);
  interaction.uv[0] = vec.dot(this->tangent) / size.x() + 0.5f;
  if(interaction.uv[0] < 0.0f)
    interaction.uv[0] = 0.0f;
  interaction.uv[1] = vec.dot(tangent_y) / size.y() + 0.5f;
  if(interaction.uv[1] < 0.0f)
    interaction.uv[1] = 0.0f;
  return true;
}
Vec2f Rectangle::getSize() const
{
  return size;
}
Vec3f Rectangle::getNormal() const
{
  return normal;
}
Vec3f Rectangle::getTangent() const
{
  return tangent;
}

Ellipsoid::Ellipsoid(const Vec3f &p, const Vec3f &a, const Vec3f &b, const Vec3f &c)
    : p(p), a(a), b(b), c(c) {}

bool Ellipsoid::intersect(Ray &ray, Interaction &interaction) const
{
  // TODO: Your code here.
  Eigen::Matrix<float,4,4> M, T, R, S;
  Vec3f a_normal = a.normalized();
  Vec3f b_normal = b.normalized();
  Vec3f c_normal = c.normalized();

  T << 1, 0, 0, p.x(),
       0, 1, 0, p.y(),
       0, 0, 1, p.z(),
       0, 0, 0, 1;

  R << a_normal.x(), b_normal.x(), c_normal.x(), 0,
       a_normal.y(), b_normal.y(), c_normal.y(), 0,
       a_normal.z(), b_normal.z(), c_normal.z(), 0,
       0, 0, 0, 1;

  S << a.norm(), 0, 0, 0,
       0, b.norm(), 0, 0,
       0, 0, c.norm(), 0,
       0, 0, 0, 1;

  M = T * R * S;

  // puts("T:");std::cout<< T(0,0) <<' '<<T(0,1) <<' '<< T(0,2) <<' ' << T(0,3) <<'\n'<< T(1,0) <<' '<<T(1,1) <<' '<< T(1,2) <<' ' << T(1,3) <<'\n'<< T(2,0) <<' '<<T(2,1) <<' '<< T(2,2) <<' ' << T(2,3) <<'\n'<< T(3,0) <<' '<<T(3,1) <<' '<< T(3,2) <<' ' << T(3,3) <<'\n';
  // puts("R:");std::cout<< R(0,0) <<' '<<R(0,1) <<' '<< R(0,2) <<' ' << R(0,3) <<'\n'<< R(1,0) <<' '<<R(1,1) <<' '<< R(1,2) <<' ' << R(1,3) <<'\n'<< R(2,0) <<' '<<R(2,1) <<' '<< R(2,2) <<' ' << R(2,3) <<'\n'<< R(3,0) <<' '<<R(3,1) <<' '<< R(3,2) <<' ' << R(3,3) <<'\n';
  // puts("S:");std::cout<< S(0,0) <<' '<<S(0,1) <<' '<< S(0,2) <<' ' << S(0,3) <<'\n'<< S(1,0) <<' '<<S(1,1) <<' '<< S(1,2) <<' ' << S(1,3) <<'\n'<< S(2,0) <<' '<<S(2,1) <<' '<< S(2,2) <<' ' << S(2,3) <<'\n'<< S(3,0) <<' '<<S(3,1) <<' '<< S(3,2) <<' ' << S(3,3) <<'\n';
  // puts("M:");std::cout<< M(0,0) <<' '<<M(0,1) <<' '<< M(0,2) <<' ' << M(0,3) <<'\n'<< M(1,0) <<' '<<M(1,1) <<' '<< M(1,2) <<' ' << M(1,3) <<'\n'<< M(2,0) <<' '<<M(2,1) <<' '<< M(2,2) <<' ' << M(2,3) <<'\n'<< M(3,0) <<' '<<M(3,1) <<' '<< M(3,2) <<' ' << M(3,3) <<'\n';

  Eigen::Matrix<float,4,1> O_origin, D_origin, O, D;
  O_origin << ray.origin.x(), ray.origin.y(), ray.origin.z(), 1.0f;
  D_origin << ray.direction.x(), ray.direction.y(), ray.direction.z(), 0.0f;
  O = M.inverse() * O_origin;
  D = M.inverse() * D_origin;
  Vec3f o = Vec3f(O(0, 0), O(1, 0), O(2, 0)) / O(3,0);
  Vec3f d = Vec3f(D(0, 0), D(1, 0), D(2, 0));

  float dis = d.cross(-o).norm() / (d).norm();
  //std::cout<<o<<'\n';
  if (dis > 1.0f - 1e-3f)
    return false;
  float thc = sqrt(1.0f * 1.0f - dis * dis);
  float tca = sqrt((-o).norm() * (-o).norm() - dis * dis);
  float t = tca - thc;
  //std::cout<<"111:"<<t<<'\n';
  t /= d.norm();
// std::cout<<thc<<' '<<tca<<'\n';
  
  Vec4f affine = M.inverse() * Vec4f(this->p.x(),this->p.y(),this->p.z() , 1.0f);
 // std::cout<<affine.x()<<' '<<affine.y()<<' '<<affine.z()<<'\n';
  // if(4.0*o.dot(d)*o.dot(d)-4.0*d.norm()*d.norm()*(o.norm()*o.norm()-1.0)<1e-3)
  //   return false;
  // std::cout<<"t "<<t<<'\n';
  // std::cout<<(-2.0*o.dot(d)+sqrt(4.0*o.dot(d)*o.dot(d)-4.0*d.norm()*d.norm()*(o.norm()*o.norm()-1.0)))/(2.0*d.norm()*d.norm())<<'\n';
  // std::cout<<(-2.0*o.dot(d)-sqrt(4.0*o.dot(d)*o.dot(d)-4.0*d.norm()*d.norm()*(o.norm()*o.norm()-1.0)))/(2.0*d.norm()*d.norm())<<'\n';

  // std::cout<<"dis "<<dis<<'\n';
  // std::cout<<"tca "<<tca<<'\n';
  // std::cout<<"thc "<<thc<<'\n';
  // std::cout<<"d.norm() "<<d.norm()<<'\n';
  // std::cout<<"ray.direction.norm() "<< ray.direction.norm()<<'\n';

  // puts("1111");
  if (t < ray.t_min || t > ray.t_max)
    return false;
  // puts("2222");

  if (ray.direction.norm() * t > interaction.dist)
  {
    // std::cout<<d.norm() * t<<' '<<interaction.dist<<'\n';
    return false;
  }
  // puts("3333");

  // puts("intersect");
  interaction.type = Interaction::Type::GEOMETRY;    
  interaction.pos = ray(t);// without affine transformation
 // std::cout<<interaction.pos<<'\n';
  //std::cout<< interaction.pos;
  //std::cout<<t<<'\n';
  Eigen::Matrix<float, 4, 1> pos_origin, pos;
 // pos_origin << interaction.pos.x(), interaction.pos.y(),interaction.pos.z(),1.0f;
  //pos_origin << p.x(), p.y(),p.z(),1.0f;
  pos_origin << interaction.pos.x(), interaction.pos.y(),interaction.pos.z(),1.0f;

  
  pos = M.inverse() * pos_origin;
  //std::cout<<(Vec3f(pos(0,0),pos(1,0),pos(2,0))/pos(3,0)).norm()<<'\n';
 // std::cout<<pos(3,0)<<'\n';

  Eigen::Matrix<float, 4, 1> normal;
  normal << (o + t*d), 0.0f;
  //std::cout<<(o+t*d).norm()<<'\n';
  normal = M.inverse().transpose() * normal;
  interaction.normal = Vec3f(normal(0,0),normal(1,0), normal(2,0)).normalized();// / normal(3,0);
  interaction.dist = ray.direction.norm() * t;
  if(this->material != nullptr)
    interaction.model = this->material->evaluate(interaction);

  //pos is on the unit ball
  // pos.x = sin(phi)cos(theta)
  // pos.y = sin(phi)sin(theta)
  // pos.z = cos(phi)
  float phi = acos(pos.z());//[0,pi]
  float theta = asin(pos.y()/sin(phi));//[-pi/2,pi/2]
  interaction.uv[0] = phi / PI;
  interaction.uv[1] = theta / PI + 0.5;
  return true;
}


Ground::Ground(Vec3f position, Vec3f normal, Vec3f tangent)
    : Geometry(),
      position(std::move(position)),
      normal(std::move(normal)),
      tangent(std::move(tangent)) {}

bool Ground::intersect(Ray &ray, Interaction &interaction) const
{
  // TODO: Your code here
  // (o + td - position) dot normal = 0
  // t = dot((position-o),normal)/dot(d,normal)
  Vec3f d = ray.direction;
  Vec3f o = ray.origin;
  if (fabs(d.dot(this->normal)) < 1e-7f)
    return false;
  float t = (this->position - o).dot(this->normal) / d.dot(this->normal);
  if (t < ray.t_min || t > ray.t_max)
    return false;
  if (d.norm() * t > interaction.dist)
    return false;
  Vec3f pos = ray(t);
  Vec3f tangent_y = this->normal.cross(this->tangent).normalized();
  Vec3f vec = pos - this->position;
  interaction.type = Interaction::Type::GEOMETRY;
  interaction.pos = o + d * t;
  interaction.normal = this->normal;
  interaction.dist = d.norm() * t;
  interaction.ray_direction = ray.direction;

  if(this->material != nullptr)
    interaction.model = this->material->evaluate(interaction);
  //std::cout<<fabs(vec.dot(this->tangent))<<' '<<fabs(vec.dot(tangent_y))<<'\n';
  interaction.uv[0] = vec.dot(this->tangent) / 5.0f;
  while(interaction.uv[0] > 1.0f)
    interaction.uv[0] -= 1.0f;
  while(interaction.uv[0] < 0.0f)
    interaction.uv[0] += 1.0f;
  interaction.uv[1] = vec.dot(tangent_y) / 5.0f;
  while(interaction.uv[1] > 1.0f)
    interaction.uv[1] -= 1.0f;
  while(interaction.uv[1] < 0.0f)
    interaction.uv[1] += 1.0f;
 // std::cout<<interaction.uv[0]<<' '<<interaction.uv[1]<<'\n';
  return true;
}
Vec3f Ground::getNormal() const
{
  return normal;
}
Vec3f Ground::getTangent() const
{
  return tangent;
}