#include <iostream>
#include <chrono>

#include "integrator.h"
#include "config_io.h"
#include "config.h"

#include <fstream>
// #include <windows.h>
int main(int argc, char *argv[]) {
  /// load config from json file
  Config config;
  std::ifstream fin;
  if (argc == 1) {
    std::cout << "No json specified, use default path." << std::endl;
    /*char  buf[1000];
	  GetCurrentDirectory(1000, buf);
    std::cout<<buf<<'\n';*/
    
    // fin.open("../../configs/simple.json");
    // fin.open("../../configs/large_mesh.json");
    // fin.open("../../configs/bonus1.json");
    // fin.open("../../configs/bonus1_box.json");
    fin.open("../../configs/bonus2.json");
    // fin.open("../../configs/bonus3.json");
    // fin.open("../../configs/bonus4.json");


    /*Eigen::Matrix4f persp,view;
    persp<<sqrt(3),0,0,0,0,sqrt(3),0,0,0,0,-2,-3*sqrt(3),0,0,-1,0;
    view<<0.5,0,-sqrt(3)/2,0,0,1,0,0,sqrt(3)/2,0,0.5,-2,0,0,0,1;
    std::cout<<persp*view;*/


  } else {
    fin.open(argv[1]);
  }
  if (!fin.is_open()) {
    std::cerr << "Can not open json file. Exit." << std::endl;
    exit(0);
  } else {
    if(argc == 1)
      puts("Json file loaded from simple.json");
    else
      std::cout << "Json file loaded from " << argv[1] << std::endl;
  }
  // parse json object to Config
  puts("load in finished.");

  try {
    nlohmann::json j;
    fin >> j;
    nlohmann::from_json(j, config);
    fin.close();
  } catch (nlohmann::json::exception &ex) {
    fin.close();
    std::cerr << "Error:" << ex.what() << std::endl;
    exit(-1);
  }
  std::cout << "Parsed json to config. Start building scene..." << std::endl;
  // initialize all settings from config
  // set image resolution.
  std::shared_ptr<ImageRGB> rendered_img
      = std::make_shared<ImageRGB>(config.image_resolution[0], config.image_resolution[1]);
  std::cout << "Image resolution: "
            << config.image_resolution[0] << " x " << config.image_resolution[1] << std::endl;
  // set camera
  std::shared_ptr<Camera> camera = std::make_shared<Camera>(config.cam_config, rendered_img);
  // construct scene.
  auto scene = std::make_shared<Scene>();
  initSceneFromConfig(config, scene);
  // init integrator
  std::unique_ptr<Integrator> integrator
      = std::make_unique<Integrator>(camera, scene, config.spp, config.max_depth);
  std::cout << "Start Rendering..." << std::endl;
  auto start = std::chrono::steady_clock::now();
  // render scene
  integrator->render();
  auto end = std::chrono::steady_clock::now();
  auto time = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
  std::cout << "\nRender Finished in " << time << "s." << std::endl;
  rendered_img->writeImgToFile("../../result.png");
  std::cout << "Image saved to disk." << std::endl;
  return 0;
}