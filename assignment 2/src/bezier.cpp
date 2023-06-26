#include <bezier.h>
#include <utils.h>
#include <vector>
#include <fstream>

const int degree = 2;
BezierCurve::BezierCurve(int m)
{ 
    control_points_.resize(m);
    int knot_num = control_points_.size() + degree + 1;
    for (int i = 1;i <= degree;++i)
        knot.push_back(0.0f);

    int num = knot_num - 2 * degree;
    float delta = 1.0f / (float)(num);


    for (int i = 0;i <= num;++i)
        knot.push_back(delta * i);

    for (int i = 1;i <= degree;++i)
        knot.push_back(1.0f);
    /*for (int i = 1;i <= 3;++i)knot.push_back(0.0f);for (int i = 1;i <= 3;++i)knot.push_back(1.0f);*/
}

BezierCurve::BezierCurve(std::vector<vec3>& control_points) {
  control_points_ = control_points;
  int knot_num = control_points_.size() + degree + 1;
  for (int i = 1;i <= degree;++i)
      knot.push_back(0.0f);

  int num = knot_num - 2 * degree;
  float delta = 1.0f / (float)(num-1);
  for (int i = 0;i < num;++i)
      knot.push_back(delta * i);

  for (int i = 1;i <= degree;++i)
      knot.push_back(1.0f);
  //for (int i = 1;i <= 3;++i)knot.push_back(0.0f);for (int i = 1;i <= 3;++i)knot.push_back(1.0f);

}

void BezierCurve::setControlPoint(int i, vec3 point) {
  control_points_[i] = point;
}

/**
 * TODO: evaluate the point at t with the given control_points
 */
Vertex BezierCurve::evaluate(std::vector<vec3>& control_points, float t) {
    int len = control_points.size();
    if (len == 2)
    {
        Vertex vertex;
        vertex.position = (1.0f - t) * control_points[0] + t * control_points[1];
        vertex.normal = glm::normalize(control_points[1] - control_points[0]);
        return vertex;
    }
    std::vector<vec3> new_control_points;
    for (auto i = 0;i < len - 1;++i)
    {
        vec3 new_point = (1.0f - t) * control_points[i] + t * control_points[i + 1];
        new_control_points.push_back(new_point);
    }
    return evaluate(new_control_points, t);
}

Vertex BezierCurve::evaluate(float t) {
  return evaluate(control_points_, t);
}

vec3 BezierCurve::Bsplineevaluate(std::vector<vec3>& control_points, float t)
{
    int n = control_points.size();
    int k = degree;
    if (t < knot[degree] + 1e-3)
        k = degree;
    else if (t + 1e-3 > knot[n])
        k = n - 1;
    else
    {
        for (;k <= n;++k)
            if (knot[k] <= t + 1e-3 && t < knot[k + 1] + 1e-3)
                break;
    }
    std::vector<vec3> tmp;
    tmp.resize(degree + 1);
    for (int i = 0;i <= degree;++i)
        tmp[i] = control_points[i + k - degree];

    for (int r = 1;r <= degree;++r)
        for (int i = k;i >= k - degree + r;--i)
        {
            int j = i - k + degree;
            //std::cout << "j="<<j << '\n';
            float a = (t - knot[i]) / (knot[i + degree - r + 1] - knot[i]);
            tmp[j] = (1.0f - a) * tmp[j - 1] + a * tmp[j];
        }
    return tmp[degree];
}

vec3 BezierCurve::Bsplineevaluate(float t) {
    return Bsplineevaluate(control_points_, t);
}

/**
 * TODO: generate an Object of the current Bezier curve
 */
Object BezierCurve::generateObject() {
  // !DELETE THIS WHEN YOU FINISH THIS FUNCTION
    Object new_object;
    return new_object;
}

BezierSurface::BezierSurface(int m, int n) {
  control_points_m_.resize(m);
  for (auto& sub_vec : control_points_m_) {
    sub_vec.resize(n);
  }
  control_points_n_.resize(n);
  for (auto& sub_vec : control_points_n_) {
    sub_vec.resize(m);
  }
  point_index.resize(m);
  for (auto& sub_vec : point_index) {
      sub_vec.resize(n);
  }
}

/**
 * @param[in] i: index (i < m)
 * @param[in] j: index (j < n)
 * @param[in] point: the control point with index i, j
 */
void BezierSurface::setControlPoint(int i, int j, vec3 point) {
  control_points_m_[i][j] = point;
  control_points_n_[j][i] = point;
}

/**
 * TODO: evaluate the point at (u, v) with the given control points
 */
Vertex BezierSurface::evaluate(std::vector<std::vector<vec3>>& control_points,
                               float u, float v) {
    int m = control_points.size();
    int n = control_points[0].size();
    
    std::vector<vec3> new_control_points;
    new_control_points.resize(m);
    for (int i = 0;i < m;++i)
    {
        BezierCurve curve(control_points[i]);
        new_control_points[i] = curve.evaluate(u).position;
    }

    BezierCurve curve(new_control_points);
    return curve.evaluate(v);
}

/**
 * TODO: generate an Object of the current Bezier surface
 */
Object BezierSurface::generateObject() {
    Vertex points[16][16];
    float delta = 0.1;
    float u, v;
    for (int i = 0;i <= 10;i++)
    {
        for (int j = 0;j <= 10;j++)
        {
            u = (float)i * 0.1f;
            v = (float)j * 0.1f;
            Vertex vertex;
            Vertex vertex_u = evaluate(control_points_m_, u, v);
            Vertex vertex_v = evaluate(control_points_n_, v, u);
            vertex.position = (vertex_u.position + vertex_v.position) / 2.0f;

            vertex.normal = glm::normalize(glm::cross(vertex_u.normal, vertex_v.normal));
            points[i][j] = vertex;
            // std::cout << vertex.position.x << ' ' << vertex.position.y << ' ' << vertex.position.z << ' ' << vertex.normal.x << ' ' << vertex.normal.y << ' ' << vertex.normal.z << '\n';
            // std::cout << i << ' ' << j << '\n';
        }
    }
    
    Object object;
    
    for (int i = 0;i <= 10;++i)
        for (int j = 0;j <= 10;++j)
        {
            if ((i != 10) && (j != 10))
            {
                object.vertices.push_back(points[i][j]);
                object.vertices.push_back(points[i][j + 1]);
                object.vertices.push_back(points[i + 1][j]);
            }
            
            if ((i != 0) && (j != 0))
            {
                object.vertices.push_back(points[i][j]);
                object.vertices.push_back(points[i][j - 1]);
                object.vertices.push_back(points[i - 1][j]);
            }
        }
    object.init();
    return object;
}

/**
 * TODO: read in bzs file to generate a vector of Bezier surface
 * for the first line we have b p m n
 * b means b bezier surfaces, p means p control points.
 *
 */
std::vector<BezierSurface> read(const std::string &path) {
    std::ifstream infile(path);
    std::vector<BezierSurface> v;

    if (infile.fail())
    {
        puts("fail to open .bzs document");
        return v;
    }
    puts("success opened .bzs document");
    int b, p, m, n, idx;
    infile >> b >> p >> m >> n;
    
    for (int num = 0;num < b;++num)
    {
        BezierSurface beziersurface(m, n);
        for (int i = 0;i < m;++i)
            for (int j = 0;j < n;++j)
            {
                infile >> idx;
                beziersurface.point_index[i][j] = idx;
            }
        v.push_back(beziersurface);
    }
    float x, y, z;
    std::vector<vec3> point;
    point.resize(p);
    for (int i = 0;i < p;++i)
    {
        infile >> x >> y >> z;
        point[i] = vec3(x, y, z);
    }
    
    for (int num = 0;num < b;++num)
        for(int i=0;i<m;++i)
            for (int j = 0;j < n;++j)
                v[num].setControlPoint(i, j, point[v[num].point_index[i][j]]);

    return v;
}

Vertex BezierSurface::Bsplineevaluate(std::vector<std::vector<vec3>>& control_points, float u, float v)
{
    int m = control_points.size();
    int n = control_points[0].size();

    std::vector<vec3> new_control_points;
    new_control_points.resize(m);
    for (int i = 0;i < m;++i)
    {
        BezierCurve curve(control_points[i]);
        new_control_points[i] = curve.Bsplineevaluate(u);
    }

    BezierCurve curve(new_control_points);
    Vertex vertex;
    vertex.position=curve.Bsplineevaluate(v);
    vertex.normal = glm::normalize(curve.Bsplineevaluate(v + 1e-3) - vertex.position);

    return vertex;
}

Object BezierSurface::BsplinegenerateObject()
{
    Vertex points[16][16];
    float delta = 0.1;
    float u, v;
    for (int i = 0;i <= 10;i++)
    {
        for (int j = 0;j <= 10;j++)
        {
            u = (float)i * 0.1f;
            v = (float)j * 0.1f;
            Vertex vertex;
            Vertex vertex_u = Bsplineevaluate(control_points_m_, u, v);
            Vertex vertex_v = Bsplineevaluate(control_points_n_, v, u);
            vertex.position = (vertex_u.position + vertex_v.position) / 2.0f;

            vertex.normal = glm::normalize(glm::cross(vertex_u.normal, vertex_v.normal));
            points[i][j] = vertex;
            //std::cout << vertex.position.x << ' ' << vertex.position.y << ' ' << vertex.position.z << ' ' << vertex.normal.x << ' ' << vertex.normal.y << ' ' << vertex.normal.z << '\n';
            // std::cout << i << ' ' << j << '\n';
        }
    }

    Object object;

    for (int i = 0;i <= 10;++i)
        for (int j = 0;j <= 10;++j)
        {
            if ((i != 10) && (j != 10))
            {
                object.vertices.push_back(points[i][j]);
                object.vertices.push_back(points[i][j + 1]);
                object.vertices.push_back(points[i + 1][j]);
            }

            if ((i != 0) && (j != 0))
            {
                object.vertices.push_back(points[i][j]);
                object.vertices.push_back(points[i][j - 1]);
                object.vertices.push_back(points[i - 1][j]);
            }
        }
    object.init();
    return object;
}