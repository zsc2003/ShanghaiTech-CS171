#include <shader.h>
#include <utils.h>
#include <fstream>
#include <sstream>
#include <camera.h>

Shader::Shader(const std::string &vsPath, const std::string &fsPath, const std::string& gsPath) {
  init(vsPath, fsPath, gsPath);
}

void Shader::init(const std::string &vsPath, const std::string &fsPath, const std::string& gsPath) {
  initWithCode(getCodeFromFile(vsPath), getCodeFromFile(fsPath), getCodeFromFile(gsPath));
}

void Shader::initWithCode(const std::string &vs, const std::string &fs, const std::string& gs) {
// TODO: filling this function to set shaders to OpenGL
    const char* vertex_shader_source_code = vs.c_str();
    const char* fragment_shader_source_code = fs.c_str();

    bool flag = true;
    if (gs == "")
        flag = false;
    
    int success;
    char infoLog[512];

   //  std::cout << vertex_shader_source_code;
     
   // vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);//创建着色器
    glShaderSource(vertexShader, 1, &vertex_shader_source_code, NULL);//绑定着色器
    glCompileShader(vertexShader);//编译
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment_shader_source_code, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    //geometry shader
    GLuint geometryShader;
    if (flag == true)
    {
        const char* geometry_shader_source_code = gs.c_str();
        geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryShader, 1, &geometry_shader_source_code, NULL);
        glCompileShader(geometryShader);
        glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::GEOMEYRY::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
    }
    
    this->id = glCreateProgram();
    glAttachShader(this->id, vertexShader);
    glAttachShader(this->id, fragmentShader);
    if(flag==true)
        glAttachShader(this->id, geometryShader);
    
    glLinkProgram(this->id);
    glGetProgramiv(this->id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(this->id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);//释放内存 
    glDeleteShader(fragmentShader);
    if (flag == true)
        glDeleteShader(geometryShader);
}

void Shader::Phong_Lighting(Camera camera)// Phong Lighting
{
    use();
    setVec3("objectColor", vec3(1.0f, 1.0f, 1.0f));
    setVec3("lightColor", vec3(1.0f, 1.0f, 1.0f));
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    setVec3("lightPos", lightPos);
    
    // define a identify model matrix for Stanford bunny.
    glm::mat4 model = glm::mat4(1.0f);// identity matrix
    setMat4("model", model);

    glm::mat4 view;
    view = camera.GetViewMatrix();
    setMat4("view", view);

    // std::cout << camera.Zoom << '\n';
    float aspect_ratio = (float)WIDTH / (float)HEIGHT , near = 0.1f, far = 100.0f;
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), aspect_ratio, near, far);
    setMat4("projection", projection);
}

void Shader::combine(Camera camera)
{
    use();
    setVec3("viewPos", camera.Position);
    setFloat("material.shininess", 32.0f);

    setInt("IfColor[0]", camera.key_R);
    setInt("IfColor[1]", camera.key_G);
    setInt("IfColor[2]", camera.key_B);

    // point light 1
    glm::vec3 pointLightPositions[] = {
        glm::vec3(4.2f,  10.0f,  10.0f),
        glm::vec3(-2.3f, -3.3f, 4.0f),
        glm::vec3(-4.0f,  2.0f, 10.0f),
    };

    glm::vec3 pointLightColor[] = {
        glm::vec3(1.0f,  0.0f,  0.0f),
        glm::vec3(0.0f,  1.0f,  0.0f),
        glm::vec3(0.0f,  0.0f,  1.0f)
    };
    //pointLight 0 (R)
    setVec3("pointLights[0].position", pointLightPositions[0]);
    setVec3("pointLights[0].color", pointLightColor[0]);
    setVec3("pointLights[0].ambient", vec3(0.05f, 0.05f, 0.05f));
    setVec3("pointLights[0].diffuse", vec3(0.8f, 0.8f, 0.8f));
    setVec3("pointLights[0].specular", vec3(1.0f, 1.0f, 1.0f));
    //pointLight 1 (G)
    setVec3("pointLights[1].position", pointLightPositions[1]);
    setVec3("pointLights[1].color", pointLightColor[1]);
    setVec3("pointLights[1].ambient", vec3(0.05f, 0.05f, 0.05f));
    setVec3("pointLights[1].diffuse", vec3(0.8f, 0.8f, 0.8f));
    setVec3("pointLights[1].specular", vec3(1.0f, 1.0f, 1.0f));
    //pointLight 2 (B)
    setVec3("pointLights[2].position", pointLightPositions[2]);
    setVec3("pointLights[2].color", pointLightColor[2]);
    setVec3("pointLights[2].ambient", vec3(0.05f, 0.05f, 0.05f));
    setVec3("pointLights[2].diffuse", vec3(0.8f, 0.8f, 0.8f));
    setVec3("pointLights[2].specular", vec3(1.0f, 1.0f, 1.0f));

    // spotLight
    setVec3("spotLight.position", camera.Position);
    setVec3("spotLight.direction", camera.Front);
    setVec3("spotLight.ambient", vec3(0.0f, 0.0f, 0.0f));
    setVec3("spotLight.diffuse", vec3(1.0f, 1.0f, 1.0f));
    setVec3("spotLight.specular", vec3(1.0f, 1.0f, 1.0f));
    setFloat("spotLight.constant", 1.0f);
    setFloat("spotLight.linear", 0.09f);
    setFloat("spotLight.quadratic", 0.032f);
    setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
    
    // define a identify model matrix for Stanford bunny.
    glm::mat4 model = glm::mat4(1.0f);// identity matrix
    setMat4("model", model);

    glm::mat4 view;
    view = camera.GetViewMatrix();
    setMat4("view", view);

    // std::cout << camera.Zoom << '\n';
    float aspect_ratio = (float)WIDTH / (float)HEIGHT, near = 0.1f, far = 100.0f;
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), aspect_ratio, near, far);
    setMat4("projection", projection);
}

std::string Shader::getCodeFromFile(const std::string &path) {
    if (path == "")
    {
        std::string empty_string = "";
        return empty_string;
    }
  std::string code;
  std::ifstream file;
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    file.open(getPath(path));
    std::stringstream stream;
    stream << file.rdbuf();
    file.close();
    code = stream.str();
  } catch (std::ifstream::failure &e) {
    LOG_ERR("File Error: " + std::string(e.what()));
  }
  return code;
}

void Shader::use() const { glUseProgram(id); }

GLint Shader::getUniformLocation(const std::string &name) const {
  return glGetUniformLocation(id, name.c_str());
}

void Shader::setInt(const std::string &name, GLint value) const {
  glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(const std::string &name, GLfloat value) const {
  glUniform1f(getUniformLocation(name), value);
}

void Shader::setMat3(const std::string &name, const mat3 &value) const {
  glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE,
                     glm::value_ptr(value));
}

void Shader::setMat4(const std::string &name, const mat4 &value) const {
  glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE,
                     glm::value_ptr(value));
}

void Shader::setVec3(const std::string &name, const vec3 &value) const {
  glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string &name, const vec4 &value) const {
  glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
}
