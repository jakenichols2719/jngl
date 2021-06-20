#include "jngl.h"

Debug JNGL::debug;
int JNGL::WIN_W = 600;
int JNGL::WIN_H = 600;
char* JNGL::WIN_NAME = (char*)"JNGL";

float JNGL::LIGHT_CLOSE = 1.0;
float JNGL::LIGHT_FAR   = 128.0;
int JNGL::SHADOW_RESOLUTION = 512;

// Get active shader name
GLint activeShader() {
  GLint name;
  glGetIntegerv(GL_CURRENT_PROGRAM,&name);
  if(name==0) {
    JNGL::debug << "Scene is using null shader";
    JNGL::debug.lib_flush();
  }
  return name;
}

// Get name for uniform location in active shader
GLint uniform(std::string name) {
  return glGetUniformLocation(activeShader(), name.c_str());
}