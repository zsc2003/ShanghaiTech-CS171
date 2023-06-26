#include <utils.h>
#include <camera.h>
#include "enum.h"
#include <object.h>
#include <shader.h>
#include <bezier.h>
#include <cmath>

void processInput(GLFWwindow *window);

void mouse_callback(GLFWwindow *window, double x, double y);

const int WIDTH = 800;
const int HEIGHT = 600;

bool firstMouse = true;
float lastX = WIDTH / 2.0;
float lastY = HEIGHT / 2.0;

int control_mouse;

GLFWwindow *window;
Camera mycamera;

BETTER_ENUM(RenderCase, int,
            uniform_multi_bezier_surface,
            adaptive_multi_bezier_surface,
            multi_Bspline_surface,
            Interactive )

struct inter{
    int status;
    vec3 vertex;
    float ratio;
};

/**
 * BETTER_ENUM is from https://aantron.github.io/better-enums/
 * You can add any render case as you like,
 * this is to allow you to demonstrate multi task in single executable file.
 * */


RenderCase choice = RenderCase::_values()[0];

std::vector<BezierSurface> v;
glm::vec3 lightPos;
std::vector<vec3> ball;
void drawmesh(Shader shader, Camera mycamera, std::vector<Object>& objects);

void Bspline(Shader shader, Camera mycamera, std::vector<Object>& objects);

void interactive(Shader shader, Camera mycamera, std::vector<Object>& objects, std::vector<inter>& center);
void adaptive(Shader shader,Camera mycamera,std::vector<Object>& objects);

int main() {
    for (size_t index = 0; index < RenderCase::_size(); ++index) {
        RenderCase render_case = RenderCase::_values()[index];
        std::cout << index << ". " << +render_case << "\n";
    }
    while (true) {
        std::cout << "choose a rendering case from [0:" << RenderCase::_size() - 1 << "]" << "\n";
        std::string input;
        std::cin >> input;
        if (isNumber(input) &&
            std::stoi(input) >= 0 &&
            std::stoi(input) < RenderCase::_size()) {
            choice = RenderCase::_values()[std::stoi(input)];
            break;
        } else {
            std::cout << "Wrong input.\n";
        }
    }
    /**
     * Choose a rendering case in the terminal.
     * */
    WindowGuard windowGuard(window, WIDTH, HEIGHT, "CS171 hw2");
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glEnable(GL_DEPTH_TEST);

    Shader shader("../src/vertex_shader.txt", "../src/fragment_shader.txt");
    std::vector<Object> objects;
    int choose = 0;
    std::vector<inter> center;
    lightPos = vec3(1.2f, 3.0f, 2.0f);
    switch (choice) {
        case RenderCase::uniform_multi_bezier_surface: {
            std::cout << +RenderCase::uniform_multi_bezier_surface << "do something\n";
            //uniform_single_bezier_surface();
            choose = 0;
            v = read("../assets/tea.bzs");
            objects.resize(v.size());

            for (int i = 0;i < v.size();++i)
            {
                //std::cout << i << '\n';
;               objects[i] = v[i].generateObject();
                objects[i].drawArrays();
            }
            puts("first part success!!");
            break;
        }
        case RenderCase::adaptive_multi_bezier_surface: {
            std::cout << +RenderCase::adaptive_multi_bezier_surface << "do something\n";
            choose = 1;
            v = read("../assets/tea.bzs");
            objects.resize(v.size());

            for (int i = 0;i < v.size();++i)
            {
                objects[i] = v[i].generateObject();
                objects[i].drawArrays();
            }
            puts("first part success!!");
            break;
        }
        case RenderCase::multi_Bspline_surface: {
            std::cout << +RenderCase::multi_Bspline_surface << "do something\n";
             // v = read("../assets/tea.bzs");
             v = read("../assets/bspline.bzs");
              //v = read("../assets/test.bzs");

            objects.resize(v.size());

            for (int i = 0;i < v.size();++i)
            {
                objects[i] = v[i].BsplinegenerateObject();
                objects[i].drawArrays();
                for (int x = 0;x < v[i].control_points_m_.size();++x)
                    for (int y = 0;y < v[i].control_points_m_[0].size();++y)
                        ball.push_back(v[i].control_points_m_[x][y]);
            }

            choose = 2;
            break;
        }
        case RenderCase::Interactive: {
            std::cout << +RenderCase::Interactive << "do something\n";
            //lightPos = vec3(-4.0f, -4.0f, 0.0f);
            lightPos = vec3(1.2f, 3.0f, 2.0f);

            choose = 3;
            v = read("../assets/interactive.bzs");
            // v = read("../assets/test3.bzs");
            // v = read("../assets/bspline.bzs");

            objects.resize(v.size());

            for (int i = 0;i < v.size();++i)
            {
                // std::cout << i << '\n';
                objects[i] = v[i].generateObject();
                objects[i].drawArrays();
                objects[i].control_points = v[i].control_points_m_;
            }
            
            for (int i = 0;i < objects.size();++i)
                for (int j = 0;j < objects[i].control_points.size();++j)
                    for (int k = 0;k < objects[i].control_points[0].size();++k)
                    {
                        inter to_push;
                        to_push.status = 0;
                        to_push.vertex = objects[i].control_points[j][k];
                        center.push_back(to_push);
                    }
            break;
        }
    }

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glfwSetCursorPosCallback(window, mouse_callback);
        control_mouse = 1;
        if (choose == 0)
        {
            drawmesh(shader, mycamera, objects);
        }
        
        if (choose == 1)
        {
            drawmesh(shader, mycamera, objects);
            adaptive(shader, mycamera, objects);
        }

        if (choose == 2)
        {
            drawmesh(shader, mycamera, objects);
            Bspline(shader, mycamera, objects);
        }

        if (choose == 3)
        {
            control_mouse = 0;
            interactive(shader, mycamera, objects, center);

        }
            
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        mycamera.processWalkAround(Forward);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        mycamera.processWalkAround(Backward);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        mycamera.processWalkAround(Leftward);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        mycamera.processWalkAround(Leftward);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        mycamera.processWalkAround(Rightward);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        mycamera.processWalkAround(Rightward);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        mycamera.processWalkAround(Upward);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        mycamera.processWalkAround(Downward);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        std::cout << "Camera position: (" << mycamera.Position.x << ", "
                  << mycamera.Position.y << ", " << mycamera.Position.z << ")" << std::endl;
    }
}

void mouse_callback(GLFWwindow *window, double x, double y) {
    x = (float) x;
    y = (float) y;
    if (firstMouse) {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }
    if(control_mouse)
        mycamera.processLookAround(x - lastX, y - lastY);

    // update record
    lastX = x;
    lastY = y;
}

void drawmesh(Shader shader, Camera mycamera, std::vector<Object>& objects)
{
    shader.use();
    shader.setVec3("objectColor", vec3(1.0f, 1.0f, 1.0f));
    shader.setVec3("lightColor", vec3(1.0f, 1.0f, 1.0f));
    shader.setVec3("viewPos", mycamera.Position);
    shader.setVec3("lightPos", lightPos);

    // define a identify model matrix for Stanford bunny.
    glm::mat4 model = glm::mat4(1.0f);// identity matrix
    shader.setMat4("model", model);

    glm::mat4 view;
    view = mycamera.getViewMatrix();
    shader.setMat4("view", view);

    float aspect_ratio = (float)WIDTH / (float)HEIGHT, near = 0.1f, far = 100.0f;
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect_ratio, near, far);
    shader.setMat4("projection", projection);
    
    for (int i = 0;i < objects.size();++i)
        objects[i].drawArrays();        
}

void Bspline(Shader shader, Camera mycamera, std::vector<Object>& objects)
{
    drawmesh(shader, mycamera, objects);
    double r = 0.25f;
    int len = ball.size();
    // std::cout<<len<<'\n';
    for (int num = 0;num < len;++num)
    {
        shader.setVec3("objectColor", vec3(0.0f, 0.0f, 1.0f));
  
        std::vector<Vertex> vertex;
        Vertex points[16][16];
        for (int i = 0; i <= 10; ++i)
        {
            for (int j = 0; j <= 10; ++j)
            {
                float xSegment = (float)i / (float)10.0f;
                float ySegment = (float)j / (float)10.0f;
                float xPos = r * std::cos(xSegment * 2.0f * 3.14f) * std::sin(ySegment * 3.14f);
                float yPos = r * std::cos(ySegment * 3.14f);
                float zPos = r * std::sin(xSegment * 2.0f * 3.14f) * std::sin(ySegment * 3.14f);

                vec3 pos = vec3(xPos, yPos, zPos);
                points[i][j] = { ball[num] + pos,pos };
            }
        }


        for (int i = 0;i <= 10;i++)
        {
            for (int j = 0;j <= 10;j++)
            {
                if ((i != 10) && (j != 10))
                {
                    vertex.push_back(points[i][j]);
                    vertex.push_back(points[i][j + 1]);
                    vertex.push_back(points[i + 1][j]);
                }

                if ((i != 0) && (j != 0))
                {
                    vertex.push_back(points[i][j]);
                    vertex.push_back(points[i][j - 1]);
                    vertex.push_back(points[i - 1][j]);
                }
            }
        }

        GLuint VAO, VBO;

        glGenVertexArrays(1, &VAO);// generate vertex array
        glGenBuffers(1, &VBO);// 顶点缓冲对象 Vertex Buffer Objects

        glBindVertexArray(VAO); //bind to the state machine
        glBindBuffer(GL_ARRAY_BUFFER, VBO); //bind to the state machine (above vao)

        glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(Vertex), &vertex[0], GL_STATIC_DRAW);

        // 顶点位置
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // 顶点法线
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertex.size());
    }
}

void interactive(Shader shader, Camera mycamera, std::vector<Object>& objects, std::vector<inter>& center)
{
    drawmesh(shader, mycamera, objects);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
        control_mouse ^= 1;

    glm::mat4 view;
    view = mycamera.getViewMatrix();
    float aspect_ratio = (float)WIDTH / (float)HEIGHT, near = 0.1f, far = 100.0f;
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect_ratio, near, far);

    
    //vec3 look = mycamera.Position + mycamera.Front*0.1f;
    //vec4 lookto = projection * view * vec4(look, 1.0f);
    //std::cout << lookto.x << ' ' << lookto.y << ' ' << lookto.z << '\n';
    //get normalized device from normalized screen at focus point depth ([0,1])
    vec3 ndcCoords = vec3((2 * lastX - WIDTH) / WIDTH, (2 * lastY - HEIGHT) / HEIGHT, -0.1f);
    //std::cout << lookto.z / lookto.w << '\n';
    //get clip [ps from normalized device coords
    float clipW = projection[2][3] / (ndcCoords.z - (projection[2][2] / projection[3][2]));
    vec4 clipCoords = vec4(ndcCoords * clipW, clipW);
    //std::cout << clipW << '\n';

    //get world pos from normalized device coords
    vec4 eyeCoords = glm::inverse(projection) * clipCoords;
    vec4 worldCoords = glm::inverse(view) * eyeCoords;
    worldCoords /= worldCoords.w;
    
    vec3 mouse_world = vec3(worldCoords);
    mouse_world.y = -mouse_world.y;
    //std::cout << mouse_world[0] << ' ' << mouse_world[1] << ' ' << mouse_world[2] << '\n';

    //std::cout << look[0] << ' ' << look[1] << ' ' << look[2] << '\n';
    //std::cout << lookto[0] << ' ' << lookto[1] << ' ' << lookto[2] << ' ' << lookto[3] << '\n';
    //std::cout << lookto.z / lookto.w << '\n';

    // vec4 mouse = glm::inverse(view) * glm::inverse(projection) * vec4(, 1.0f, 1.0f);
    //vec4 mouse = vec4(lastX, lastY, 0.0f, 1.0f);
    //vec4 test = vec4(center[1].vertex, 1.0f);
    //test = projection * view * test;
    //std::cout << mouse[0] << ' ' << mouse[1] << ' ' << mouse[2] << ' ' << mouse[3] << '\n';
    //std::cout << test[0] << ' ' << test[1] << ' ' << test[2] << ' ' << test[3] << '\n';

    //center.push_back(vec3());
    //projection = view;
    //std::cout << projection[0][0]<<' '<<projection[0][1]<<' ' << projection[0][2] << ' ' << projection[0][3] << '\n' << projection[1][0] << ' ' << projection[1][1] << ' ' << projection[1][2] << ' ' << projection[1][3] << '\n' << projection[2][0] << ' ' << projection[2][1] << ' ' << projection[2][2] << ' ' << projection[2][3] << '\n' << projection[3][0] << ' ' << projection[3][1] << ' ' << projection[3][2] << ' ' << projection[3][3] << '\n';
    int len = center.size();
    /*std::cout << "size=" << center.size() << '\n';
    for (int i = 0;i < len;++i)
       std::cout << center[i].x << ' ' << center[i].y << ' ' << center[i].z << '\n';
    */
    double r = 0.25f;
    // 生成球的顶点
    //std::cout << "-----------------------------------------\n";
    for (int num = 0;num < len;++num)
    {
        int color = 0;
        vec3 veca = center[num].vertex - mycamera.Position;
        vec3 vecb = mouse_world - mycamera.Position;
        float dis = glm::length(glm::cross(veca, vecb)) / glm::length(vecb);
        //std::cout << veca.x << ' ' << veca.y << ' ' << veca.z << '\n';
        //std::cout << glm::cross(glm::normalize(veca), glm::normalize(vecb)).length() << ' ' << vecb.length() << '\n';
        //std::cout << dis << '\n';
        if (dis < 0.5f)
        {
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
            {
                center[num].status = 2;
                center[num].ratio = (glm::length(mouse_world - mycamera.Position) / glm::length(center[num].vertex - mycamera.Position));
            }
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
                center[num].status = 0;
            if(center[num].status != 2)
                center[num].status = 1;
        }
        else if (center[num].status != 2)
            center[num].status = 0;
        shader.use();
        if (center[num].status == 0)
            shader.setVec3("objectColor", vec3(0.0f, 0.0f, 1.0f));
        else if(center[num].status == 1)
            shader.setVec3("objectColor", vec3(1.0f, 0.0f, 0.0f));
        else
            shader.setVec3("objectColor", vec3(0.0f, 1.0f, 0.0f));

        if (center[num].status == 2)
        {
            center[num].vertex = (mouse_world - mycamera.Position) / center[num].ratio + mycamera.Position;
            int m = v[0].control_points_m_[0].size();
            //std::cout << m << '\n';
            v[0].control_points_m_[num / m][num % m] = center[num].vertex;
            v[0].control_points_n_[num % m][num / m] = center[num].vertex;
            objects[0] = v[0].generateObject();
            
            glLineWidth(5.0f);

            glBegin(GL_LINES);
            vec3 dir = center[num].vertex + mycamera.Front;
            glVertex3f(center[num].vertex[0], center[num].vertex[1], center[num].vertex[2]);
            glVertex3f(dir[0], dir[1], dir[2]);

            dir = center[num].vertex + mycamera.Right;
            glVertex3f(center[num].vertex[0], center[num].vertex[1], center[num].vertex[2]);
            glVertex3f(dir[0], dir[1], dir[2]);

            dir = center[num].vertex + mycamera.Up;
            glVertex3f(center[num].vertex[0], center[num].vertex[1], center[num].vertex[2]);
            glVertex3f(dir[0], dir[1], dir[2]);
            // 绘制点结束
            glEnd();
            

        }

        std::vector<Vertex> vertex;
        Vertex points[16][16];
        for (int i = 0; i <= 10; ++i)
        {
            for (int j = 0; j <= 10; ++j)
            {
                float xSegment = (float)i / (float)10.0f;
                float ySegment = (float)j / (float)10.0f;
                float xPos = r * std::cos(xSegment * 2.0f * 3.14f) * std::sin(ySegment * 3.14f);
                float yPos = r * std::cos(ySegment * 3.14f);
                float zPos = r * std::sin(xSegment * 2.0f * 3.14f) * std::sin(ySegment * 3.14f);

                vec3 pos = vec3(xPos, yPos, zPos);
                points[i][j] = { center[num].vertex + pos,pos };
            }
        }

        
        for (int i = 0;i <= 10;i++)
        {
            for (int j = 0;j <= 10;j++)
            {
                if ((i != 10) && (j != 10))
                {
                    vertex.push_back(points[i][j]);
                    vertex.push_back(points[i][j + 1]);
                    vertex.push_back(points[i + 1][j]);
                }

                if ((i != 0) && (j != 0))
                {
                    vertex.push_back(points[i][j]);
                    vertex.push_back(points[i][j - 1]);
                    vertex.push_back(points[i - 1][j]);
                }
            }
        }

        GLuint VAO, VBO;

        glGenVertexArrays(1, &VAO);// generate vertex array
        glGenBuffers(1, &VBO);// 顶点缓冲对象 Vertex Buffer Objects

        glBindVertexArray(VAO); //bind to the state machine
        glBindBuffer(GL_ARRAY_BUFFER, VBO); //bind to the state machine (above vao)

        glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(Vertex), &vertex[0], GL_STATIC_DRAW);

        // 顶点位置
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // 顶点法线
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertex.size());
    }
}

void adaptive(Shader shader, Camera mycamera, std::vector<Object>& objects)
{
    return;
    float x1, y1, x2, y2, x3, y3;
    unsigned int level;
    float x12 = (x1 + x2) / 2;
    float y12 = (y1 + y2) / 2;
    float x23 = (x2 + x3) / 2;
    float y23 = (y2 + y3) / 2;
    float x123 = (x12 + x23) / 2;
    float y123 = (y12 + y23) / 2;

    float dx = x3 - x1;
    float dy = y3 - y1;
    float d = fabs(((x2 - x3) * dy - (y2 - y3) * dx));

    float curve_collinearity_epsilon, m_distance_tolerance, m_angle_tolerance, curve_angle_tolerance_epsilon, pi;
    if (d > curve_collinearity_epsilon)
    {
        // Regular care
        //-----------------
        if (d * d <= m_distance_tolerance * (dx * dx + dy * dy))
        {
            // If the curvature doesn't exceed the distance_tolerance value
            // we tend to finish subdivisions.
            //----------------------
            if (m_angle_tolerance < curve_angle_tolerance_epsilon)
            {
                //.add(point_type(x123, y123));
                return;
            }

            // Angle & Cusp Condition
            //----------------------
            double da = fabs(atan2(y3 - y2, x3 - x2) - atan2(y2 - y1, x2 - x1));
            if (da >= pi) da = 2 * pi - da;

            if (da < m_angle_tolerance)
            {
                // Finally we can stop the recursion
                //----------------------
                //m_points.add(point_type(x123, y123));
                return;
            }
        }
    }
    else
    {
        // Collinear case
        //-----------------
        dx = x123 - (x1 + x3) / 2;
        dy = y123 - (y1 + y3) / 2;
        if (dx * dx + dy * dy <= m_distance_tolerance)
        {
            //m_points.add(point_type(x123, y123));
            return;
        }
    }
}