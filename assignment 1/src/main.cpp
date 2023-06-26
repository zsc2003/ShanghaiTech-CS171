#include <utils.h>
#include <mesh.h>
#include <shader.h>
#include <camera.h>

const int WIDTH = 1920;
const int HEIGHT = 1080;

GLFWwindow* window;

const char* vertex_shader_source_code = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 10.0);\n"
"   ourColor = aColor;\n"
"}\0";

const char* fragment_shader_source_code = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor, 1.0f);\n"
"}\n\0";

float vertices[] = {

            0.3f,    0.37, 0.0f, 0.583f,  0.771f,  0.014f,
            -0.024f, 0.5f, 0.0f, 0.609f,  0.115f,  0.436f,
            -0.3f,   0.32f, 0.0f, 0.327f,  0.483f,  0.844f,
            0.3f,    0.37f, 0.0f, 0.822f,  0.569f,  0.201f,
            -0.3f,   0.32f, 0.0f, 0.435f,  0.602f,  0.223f,
            0.06f,   0.1f, 0.0f, 0.310f,  0.747f,  0.185f,
            0.3f,    0.37f, 0.0f, 0.597f,  0.770f,  0.761f,
            0.06f,   0.1f, 0.0f, 0.559f,  0.436f,  0.730f,
            0.252f,  -0.18f, 0.0f, 0.359f,  0.583f,  0.152f,
            0.06f,   0.1f, 0.0f, 0.483f,  0.596f,  0.789f,
            0.252f,  -0.18f, 0.0f, 0.559f,  0.861f,  0.639f,
            0.048f,  -0.54f, 0.0f, 0.195f,  0.548f,  0.859f,
            -0.3f,   0.32f, 0.0f, 0.014f,  0.184f,  0.576f,
            0.06f,   0.1f, 0.0f, 0.771f,  0.328f,  0.970f,
            -0.24f,  -0.26f, 0.0f, 0.406f,  0.615f,  0.116f,
            0.048f,  -0.54f, 0.0f, 0.676f,  0.977f,  0.133f,
            0.06f,   0.1f, 0.0f, 0.971f,  0.572f,  0.833f,
            -0.24f,  -0.26f, 0.0f, 0.140f,  0.616f,  0.489f
};

float new_vertices[] = {

            0.3f,    0.37, 0.0f, 0.583f,  0.771f,  0.014f,
            -0.024f, 0.5f, 0.0f, 0.609f,  0.115f,  0.436f,
            -0.3f,   0.32f, 0.0f, 0.327f,  0.483f,  0.844f,
            0.3f,    0.37f, 0.0f, 0.822f,  0.569f,  0.201f,
            -0.3f,   0.32f, 0.0f, 0.435f,  0.602f,  0.223f,
            0.06f,   0.1f, 0.0f, 0.310f,  0.747f,  0.185f,
            0.3f,    0.37f, 0.0f, 0.597f,  0.770f,  0.761f,
            0.06f,   0.1f, 0.0f, 0.559f,  0.436f,  0.730f,
            0.252f,  -0.18f, 0.0f, 0.359f,  0.583f,  0.152f,
            0.06f,   0.1f, 0.0f, 0.483f,  0.596f,  0.789f,
            0.252f,  -0.18f, 0.0f, 0.559f,  0.861f,  0.639f,
            0.048f,  -0.54f, 0.0f, 0.195f,  0.548f,  0.859f,
            -0.3f,   0.32f, 0.0f, 0.014f,  0.184f,  0.576f,
            0.06f,   0.1f, 0.0f, 0.771f,  0.328f,  0.970f,
            -0.24f,  -0.26f, 0.0f, 0.406f,  0.615f,  0.116f,
            0.048f,  -0.54f, 0.0f, 0.676f,  0.977f,  0.133f,
            0.06f,   0.1f, 0.0f, 0.971f,  0.572f,  0.833f,
            -0.24f,  -0.26f, 0.0f, 0.140f,  0.616f,  0.489f
};

void rendering_immediate()
{
    const int indices[] = { 0, 1, 2,
                            1, 2, 3 };
    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


    //立即渲染模式:
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.5f, -0.5f);
    glVertex2f(0.0f, 0.5f);
    glEnd();
}


void warmup()
{
    double  timeValue = glfwGetTime();
    double change = sin(timeValue) / 2.0 + 0.5;
    // 颜色属性
    for (int num = 0;num <= 18;++num)
    {
        new_vertices[6 * num + 3] = vertices[6 * num + 3] * change;
        new_vertices[6 * num + 4] = vertices[6 * num + 4] * change;
        new_vertices[6 * num + 5] = vertices[6 * num + 5] * change;
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(new_vertices), new_vertices, GL_STATIC_DRAW);
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // glUseProgram(shaderProgram);
    glDrawArrays(GL_TRIANGLES, 0, 18);

}
float vertex[] = {
         0.5f,  0.5f, 0.0f,1,0,0,  // top right
         0.5f, -0.5f, 0.0f,1,0,0,  // bottom right
        -0.5f, -0.5f, 0.0f,1,0,0,  // bottom left
        -0.5f,  0.5f, 0.0f,1,0,0  // top left 
};
unsigned int indices[] = {  // note that we start from 0!
    0, 1, 3,  // first Triangle
    1, 2, 3   // second Triangle
};
void try_indices()
{
    //puts("running in try_indices()");

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), &vertex[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    /*glUseProgram(shaderProgram);
    glBindVertexArray(0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    */
    
}

void furry(Camera camera, Shader shader, Shader normalShader, Mesh object)
{
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);// identity matrix
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    shader.setMat4("model", model);
    object.draw();

    normalShader.use();
    normalShader.setVec3("objectColor", vec3(1.0f, 1.0f, 1.0f));
    normalShader.setVec3("lightColor", vec3(1.0f, 1.0f, 1.0f));
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    normalShader.setVec3("lightPos", lightPos);
    normalShader.setMat4("model", model);
    normalShader.setMat4("view", view);
    normalShader.setMat4("projection", projection);
    object.draw();
}

int main() {


    WindowGuard windowGuard(window, WIDTH, HEIGHT, "CS171 hw1");

    Shader Phong_Lighting_shader("src/phone_lightning_vertex_shader.txt", "src/phong_lightning_fragment_shader.txt", "");
    Shader spot_light_shader("src/spot_lights_vertex_shader.txt", "src/spot_lights_fragment_shader.txt", "");
    Shader furry_shader("src/furry_vertex_shader.txt", "src/furry_fragment_shader.txt", "src/geometry_shader.txt");
   // Shader normalShader("src/vertex_shader.txt", "src/fragment_shader.txt", "src/geometry_shader.txt");
    
    Mesh bunny("assets/bunny.obj");
    Mesh sphere("assets/sphere.obj");
    Mesh plane("assets/plane.obj");

    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

    glEnable(GL_DEPTH_TEST);

    float deltaTime = 0.0f;	// time between current frame and last frame
    float lastFrame = 0.0f;
    camera.set(window);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        camera.processInput(window, deltaTime);
       
        // single white light
       // Phong_Lighting_shader.Phong_Lighting(camera);

        //combined spotlight with camera, dot light
        spot_light_shader.combine(camera);
        
        bunny.draw();
        //sphere.draw();
        //plane.draw();

        //furry on the bunny
        furry(camera, furry_shader, spot_light_shader, bunny);


        //warmup();
        // try_indices();
        // rendering_immediate();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    return 0;
}