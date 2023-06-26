#include "cloth.h"
#include "scene.h"


int main() {

  constexpr int bonus = 0;

  /// settings

  // window
  constexpr int window_width = 1920;
  constexpr int window_height = 1080;

  // cloth
  constexpr Float cloth_weight = Float(2);
  constexpr UVec2 mass_dim = { 40, 30 };
  constexpr Float dx_local = Float(0.1);
  constexpr Float stiffness = Float(15);
  constexpr Float damping_ratio = Float(0.0015);
  std::vector<IVec2> fixed_masses { { 0, -1 }, { -1, -1 } };
                                    //{0,-1}->{0,29},{-1,-1}->{39,29}


  /// setup window
  GLFWwindow* window;
  {
    if (!glfwInit()) // initialize glfw library
      return -1;

    // setting glfw window hints and global configurations
    {
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // use core mode
      // glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // use debug context
    #ifdef __APPLE__
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
    #endif
    }

    // create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(window_width, window_height, "CS171 HW5: Cloth Simulation", NULL, NULL);
    if (!window) {
      glfwTerminate();
      return -1;
    }

    // make the window's context current
    glfwMakeContextCurrent(window);

    // load Opengl
    if (!gladLoadGL()) {
      glfwTerminate();
      return -1;
    }

    // setup call back functions
    glfwSetFramebufferSizeCallback(window, Input::CallBackResizeFlareBuffer);
  }

  /// main Loop
  {
    // shader
    Shader::Initialize();

    // scene
    Scene scene(45);
    scene.camera.transform.position = { 0, -1.5, -6 };
    scene.camera.transform.rotation = { 0, 0, 1, 0 };
    scene.light_position = { 0, 3, -10 };
    scene.light_color = Vec3(1, 1, 1) * Float(1.125);


    Vec3 center(3.5, -1.8, 0.3);
    if(bonus == 2)
    {
      // center = Vec3(0.5, -1.3, 0.3);
      center = Vec3(0.25, -1.3, 0.3);

    }
    
    // clothes
    auto cloth = std::make_shared<RectCloth>(cloth_weight,
                                             mass_dim,
                                             dx_local,
                                             stiffness, damping_ratio, bonus, center);
    for (const auto& fixed_mass : fixed_masses) {
      if (!cloth->SetMassFixedOrNot(fixed_mass.x, fixed_mass.y, true))
        abort();
    }

    // mesh primitives
    auto mesh_cube = std::make_shared<Mesh>(MeshPrimitiveType::cube);
    auto mesh_sphere = std::make_shared<Mesh>(MeshPrimitiveType::sphere);

    // objects
    auto object_cloth = scene.AddObject(cloth,
                                        Shader::shader_phong,
                                        Transform(Vec3(0, 0, 0),
                                                  glm::quat_cast(glm::rotate(Mat4(Float(1.0)), glm::radians(Float(60)), Vec3(1, 0, 0))),
                                                  Vec3(1, 1, 1)));
    
    auto object_cube = scene.AddObject(mesh_cube,
                                       Shader::shader_phong,
                                       Transform(Vec3(-3.5, -1.8, 0.3),
                                                 Quat(1, 0, 0, 0),
                                                 Vec3(1, 1, 1)));
    

    auto object_sphere = scene.AddObject(mesh_sphere,
                                     Shader::shader_phong,
                                     Transform(center,
                                               Quat(1, 0, 0, 0),
                                               Vec3(1, 1, 1)));
    
     
    object_cloth->color = { zero, Float(0.75), one };
    object_cube->color = { Float(0.75), one, zero };
    object_sphere->color = { one, Float(0.75), zero };

    // loop until the user closes the window
    Input::Start(window);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);

    bool selected = 0;
    int mouse_x, mouse_y;
    while (!glfwWindowShouldClose(window)) {
      Input::Update(mouse_x, mouse_y);
      Time::Update();
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      /// terminate
      if (Input::GetKey(KeyCode::Escape))
        glfwSetWindowShouldClose(window, true);

      // mouse_position.y = -mouse_position.y;
      // std::cout<<mouse_position.x<<' '<<mouse_position.y<<' '<<mouse_position.z<<'\n';
      // mouse_position.x = 1920 - mouse_position.x;
      bool left = 0;
      bool middle = 0;
      if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        left = 1;
      if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
        middle = 1;

      Mat4 projection = scene.camera.PerspectiveMat();
      Mat4 view = scene.camera.LookAtMat();
      Vec3 ndc_space = Vec3((2.0f * mouse_x) / window_width - 1.0f, 1.0f - (2.0f * mouse_y) / window_height, -0.1f);
      Vec4 clip_space = Vec4(ndc_space, 1.0);
      Vec4 eye_coord = glm::inverse(projection) * clip_space;
      Vec4 world_coord = glm::inverse(view) * eye_coord;
      world_coord /= world_coord.w;
      


    /*Vec3 ndcCoords = Vec3((2 * mouse_position.x - window_width) / window_width, (2 * mouse_position.y - window_height) / window_height, -0.1f);
    float clipW = projection[2][3] / (ndcCoords.z - (projection[2][2] / projection[3][2]));
    Vec4 clipCoords = Vec4(ndcCoords * clipW, clipW);
    Vec4 eyeCoords = glm::inverse(projection) * clipCoords;
    Vec4 worldCoords = glm::inverse(view) * eyeCoords;
    worldCoords /= worldCoords.w;
    Vec3 mouse_world = Vec3(worldCoords);
    mouse_world.y = -mouse_world.y;
    Vec3 mouse = Vec3(mouse_world.x,mouse_world.y,-0.1f);*/


      Vec3 mouse = Vec3(world_coord.x,world_coord.y,world_coord.z);
      // std::cout<<world_coord.x<<' '<<world_coord.y<<' '<<world_coord.z<<'\n';
      
      if(!selected)
      {
        if(left)
        {
          selected = scene.DragUpdate(mouse);
        }
      }
      else
      {
        if(middle)
          selected = 0;
        scene.move_fix(mouse, middle);
      }

      /// fixed update
      for (unsigned i = 0; i < Time::fixed_update_times_this_frame; ++i) {
        if(Input::GetKey(KeyCode::Space)) { //! only when space is pressed
          scene.FixedUpdate(mouse, left, middle);
        }
      }

      /// update
      {
        scene.Update(mouse, left, middle);
//        printf("Pos = (%f, %f, %f)\n", scene.camera.transform.position.x, scene.camera.transform.position.y, scene.camera.transform.position.z);
//        printf("Rot = (%f, %f, %f, %f)\n", scene.camera.transform.rotation.w, scene.camera.transform.rotation.x, scene.camera.transform.rotation.y, scene.camera.transform.rotation.z);
//        printf("\n");
      }

      /// render
      {
        scene.RenderUpdate(mouse, left, middle);
      }

      // swap front and back buffers
      glfwSwapBuffers(window);

      // poll for and process events
      glfwPollEvents();
    }
  }

  glfwTerminate();
  return 0;
}

/*
void processmouse(std::vector<std::vector<vec3>> controlpoint, glm::mat4 proj, glm::mat4 view, vec3 camerapos) {
    float x = (2.0f * mousex) / WIDTH - 1.0f;
    float y = 1.0f - (2.0f * mousey) / HEIGHT;
    float z = -0.1f;
    vec3 ray_nds = vec3(x, y, z);
    vec4 ray_clip = vec4(ray_nds, 1.0);
    vec4 ray_view = inverse(proj) * ray_clip;
    vec4 ray_world = inverse(view) * ray_view;
    if (ray_world.w != 0.0)
    {
        ray_world.x /= ray_world.w;
        ray_world.y /= ray_world.w;
        ray_world.z /= ray_world.w;			
    }
    vec3 ray_dir = normalize(vec3(ray_world.x, ray_world.y, ray_world.z) - camerapos);
    vec3 vectorcp = controlpoint[0][0] - camerapos;
    double mindis = glm::length(glm::cross(vectorcp, ray_dir))/glm::length(vectorcp);
    for (int i = 0; i<controlpoint.size(); i++) {
        for (int j = 0; j<controlpoint[0].size(); j++) {
            vec3 vectorcp = controlpoint[i][j] - camerapos;
            double distance = glm::length(glm::cross(vectorcp, ray_dir))/glm::length(vectorcp);
            if (distance<0.1&&mindis>=distance) {
                iindex = i; jindex = j;
                mindis = distance;
                if_selected = true;
            }
        }
    }
}*/