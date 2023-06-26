#ifndef _BEZIER_H_
#define _BEZIER_H_
#include "defines.h"
#include <object.h>

#include <vector>

class BezierCurve {
 public:
  std::vector<vec3> control_points_;
  std::vector<float> knot;

  BezierCurve(int m);
  BezierCurve(std::vector<vec3>& control_points);

  void setControlPoint(int i, vec3 point);
  Vertex evaluate(std::vector<vec3>& control_points, float t);
  Vertex evaluate(float t);
  vec3 Bsplineevaluate(std::vector<vec3>& control_points, float t);
  vec3 Bsplineevaluate(float t);
  Object generateObject();
};

class BezierSurface {
 public:
  std::vector<std::vector<vec3>> control_points_m_;
  std::vector<std::vector<vec3>> control_points_n_;
  std::vector<std::vector<int>> point_index;

  BezierSurface(int m, int n);
  void setControlPoint(int i, int j, vec3 point);
  Vertex evaluate(std::vector<std::vector<vec3>>& control_points, float u, float v);
  Object generateObject();
  Vertex Bsplineevaluate(std::vector<std::vector<vec3>>& control_points, float u, float v);
  Object BsplinegenerateObject();
};

std::vector<BezierSurface> read(const std::string &path);
#endif
