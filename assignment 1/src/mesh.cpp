#include <mesh.h>
#include <utils.h>
#include <fstream>
#include <vector>

Mesh::Mesh(const std::string &path) 
{
    loadDataFromFile(getPath(path));
    glGenVertexArrays(1, &this->VAO);// generate vertex array
    glGenBuffers(1, &this->VBO);// 顶点缓冲对象 Vertex Buffer Objects
    glGenBuffers(1, &this->EBO);

    glBindVertexArray(this->VAO); //bind to the state machine
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO); //bind to the state machine (above vao)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // 顶点位置
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // 顶点法线
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // 顶点纹理坐标
   // glEnableVertexAttribArray(2);
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

}

/**
 * TODO: implement load mesh data from file here
 * you need to open file with [path]
 *
 * File format
 *
 *
 * For each line starting with 'v' contains 3 floats, which
 * represents the position of a vertex
 *
 * For each line starting with 'n' contains 3 floats, which
 * represents the normal of a vertex
 *
 * For each line starting with 'f' contains 6 integers
 * [v0, n0, v1, n1, v2, n2], which represents the triangle face
 * v0, n0 means the vertex index and normal index of the first vertex
 * v1, n1 means the vertex index and normal index of the second vertex
 * v2, n2 means the vertex index and normal index of the third vertex
 */
void Mesh::loadDataFromFile(const std::string &path) {
    /**
     * path stands for the where the object is storaged
     * */
  // !DELETE THIS WHEN YOU FINISH THIS FUNCTION

    std::ifstream infile(path);
    std::string s;
    GLfloat x, y, z;
    std::vector<glm::vec3> vertex;
    std::vector<glm::vec3> normals;

    while (infile >> s)
    {
        // std::cout << c << '\n';
        if (s == "v")//position of a vertex
        {
            infile >> x >> y >> z;
            vertex.push_back({ x,y,z });
        }
        if (s == "n")//normal of a vertex
        {
            infile >> x >> y >> z;
            normals.push_back({ x,y,z });
        }
        if (s == "f")//[v0, n0, v1, n1, v2, n2], which represents the triangle face
        {
            GLuint v0, v1, v2, n0, n1, n2;
            infile >> v0 >> n0 >> v1 >> n1 >> v2 >> n2;
            //v0, n0 means the vertex indexand normal index of the first vertex
            //v1, n1 means the vertex indexand normal index of the second vertex
            //v2, n2 means the vertex indexand normal index of the third vertex
            indices.push_back(v0);
            indices.push_back(n0);
            indices.push_back(v1);
            indices.push_back(n1);
            indices.push_back(v2);
            indices.push_back(n2);
        }
    }
    for (std::size_t i{}; i < indices.size();i += 2)
    {
        this->vertices.push_back({ vertex[indices[i]],normals[indices[i + 1]] });
    }
}

/**
 * TODO: implement your draw code here
 */
void Mesh::draw() const {
   // puts("working in mesh.draw()");
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}
