#include "jngl.h"

JNGLContext context;
Debug debug;
std::string def_depth[5] = {"shaders/def_depth.vert","","","shaders/def_depth.geom","shaders/def_depth.frag"};
std::string def_render[5] = {"shaders/def_render.vert","","","","shaders/def_render.frag"};

float light_positions[8][3] = {
  {100,5,-200},
  {100,5,-50},
  {100,-100,-200},
  {100,-100,-100},
  {1000,1000,1000},
  {1000,1000,1000},
  {1000,1000,1000},
  {1000,1000,1000},
};

// Camera preset position/rotations
float cam_positions[10][6] = {
  {-100,50,100, -10,-45,0},
  {-25,5,50, 0,-10,0},
  {130,5,-120, 0,45,0},
  {0,0,0, 0,0,0},
  {0,0,0, 0,0,0},
  {40,-160,-290, 8,-140,-6},
  {0,0,0, 0,0,0},
  {0,0,0, 0,0,0},
  {0,0,0, 0,0,0},
  {0,0,0, 0,0,0}
};
int current_cam_position = 0;

float dsin(float theta) { return sin(((float)theta/180)*3.14);};
float dcos(float theta) { return cos(((float)theta/180)*3.14);};

void ukey(GLFWwindow* win, int key, int scancode, int action, int mods) {
  if(action==GLFW_RELEASE) return;
  else if(key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(win, 1);
  else if(key == GLFW_KEY_1) current_cam_position = 0;
  else if(key == GLFW_KEY_2) current_cam_position = 1;
  else if(key == GLFW_KEY_3) current_cam_position = 2;
  else if(key == GLFW_KEY_4) current_cam_position = 3;
  else if(key == GLFW_KEY_5) current_cam_position = 4;
  else if(key == GLFW_KEY_6) current_cam_position = 5;
  else if(key == GLFW_KEY_7) current_cam_position = 6;
  else if(key == GLFW_KEY_8) current_cam_position = 7;
  else if(key == GLFW_KEY_9) current_cam_position = 8;
  else if(key == GLFW_KEY_0) current_cam_position = 9;
}

int main(int argc, char* argv[]) {
  // Enable debug output to std::cout
  Debug::DEBUG_COUT = true;
  // Open a context
  context.open();
  context.setKeyCallback(ukey);
  // Load shaders
  context.shader_dataPass = createShader(def_depth);
  context.shader_renderPass = createShader(def_render);
  // Create a scene
  Scene* scn = context.newScene();
  // Load bases
  ObjectBase DesertBase = loadObject(".", "desert");
  ObjectBase CubeBase = loadObject(".","cube");

  // Add objects using those bases
  Object* env = scn->addObject(DesertBase);
  Object* light_reps[8];
  for(int n=0; n<8; n++) {
    light_reps[n] = scn->addObject(CubeBase);
    light_reps[n]->setPos(light_positions[n][0], light_positions[n][1], light_positions[n][2]);
    light_reps[n]->useLighting = false;
  }
  //env->setScl(0.01,0.01,0.01);
  // Add a camera to the scene
  Camera* cam = scn->getCamera();
  cam->setRot(-10,0,0);
  for(int n=0; n<8; n++) {
    scn->getPointLight(n)->setPos(light_positions[n][0], light_positions[n][1], light_positions[n][2]);
  }
  while(true) {
    // Does all context passes and renders scene
    context.process();
    //cam->addRot(0,1.0/6.0,0);
    //cam->setPos(dsin(a)*l+100, -100, dcos(a)*(-l*2)-200);
    int n = current_cam_position;
    cam->setPos(cam_positions[n][0], cam_positions[n][1], cam_positions[n][2]);
    cam->setRot(cam_positions[n][3], cam_positions[n][4], cam_positions[n][5]);
    glfwPollEvents();
    //exit(0);
  }
  return 0;
}