#include "jngl.h"

void APIENTRY debug(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
  // Some debug messages are just annoying informational messages
  switch (id)
  {
  case 131185: // glBufferData
      return;
  }

  printf("Message: %s\n", message);
  printf("Source: ");

  switch (source)
  {
  case GL_DEBUG_SOURCE_API:
      printf("API");
      break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      printf("Window System");
      break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
      printf("Shader Compiler");
      break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
      printf("Third Party");
      break;
  case GL_DEBUG_SOURCE_APPLICATION:
      printf("Application");
      break;
  case GL_DEBUG_SOURCE_OTHER:
      printf("Other");
      break;
  }

  printf("\n");
  printf("Type: ");

  switch (type)
  {
  case GL_DEBUG_TYPE_ERROR:
      printf("Error");
      break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      printf("Deprecated Behavior");
      break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      printf("Undefined Behavior");
      break;
  case GL_DEBUG_TYPE_PORTABILITY:
      printf("Portability");
      break;
  case GL_DEBUG_TYPE_PERFORMANCE:
      printf("Performance");
      break;
  case GL_DEBUG_TYPE_MARKER:
      printf("Marker");
      break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
      printf("Push Group");
      break;
  case GL_DEBUG_TYPE_POP_GROUP:
      printf("Pop Group");
      break;
  case GL_DEBUG_TYPE_OTHER:
      printf("Other");
      break;
  }

  printf("\n");
  printf("ID: %d\n", id);
  printf("Severity: ");

  switch (severity)
  {
  case GL_DEBUG_SEVERITY_HIGH:
      printf("High");
      break;
  case GL_DEBUG_SEVERITY_MEDIUM:
      printf("Medium");
      break;
  case GL_DEBUG_SEVERITY_LOW:
      printf("Low");
      break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
      printf("Notification");
      break;
  }

  printf("\n\n");
}

void reshape(GLFWwindow* window,int width,int height) {
  //  Get framebuffer dimensions
  glfwGetFramebufferSize(window,&width,&height);
  // Set window size properties
  JNGL::WIN_W = width;
  JNGL::WIN_H = height;
}

void key(GLFWwindow* win, int key, int scancode, int action, int mods) {
  if(action==GLFW_RELEASE) return;
  else if(key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(win, 1);
}

void JNGLContext::createLDepthBuffer() {
  // Create buffer
  glGenFramebuffers(1,&ldepth_buffer);
  // Create texture
  glGenTextures(1,&ldepth_cubemap);
  glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, ldepth_cubemap);
  // Set texture parameters
  glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT,
               JNGL::SHADOW_RESOLUTION, JNGL::SHADOW_RESOLUTION,
               JNGL::MAX_POINT_LIGHTS*6, 0,
               GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY,GL_TEXTURE_BASE_LEVEL, 0);
  // Bind texture to framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, ldepth_buffer);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, ldepth_cubemap,0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);
}

void JNGLContext::open() {
  JNGL::debug << "Initializing GLFW"; JNGL::debug.lib_flush();
  if(!glfwInit()) {
    JNGL::debug << "Failed to initialize GLFW. Terminating";
    JNGL::debug.lib_flush();
    exit(1);
  }
  // Window hints
  glfwWindowHint(GLFW_RESIZABLE,1);     //  Window can be resized
  glfwWindowHint(GLFW_DOUBLEBUFFER,1);  //  Window has double buffering
  glfwWindowHint(GLFW_DEPTH_BITS,24);   //  Prefer 24 depth bits
  glfwWindowHint(GLFW_ALPHA_BITS,8);    //  Prefer 8 alpha bits
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,GL_TRUE); // Debug output
#ifdef APPLE_GL4
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
#endif
  // Create window
  JNGL::debug << "Creating window"; JNGL::debug.lib_flush();
  win = glfwCreateWindow(JNGL::WIN_W, JNGL::WIN_H, JNGL::WIN_NAME, NULL, NULL);
  if(!win) {
    JNGL::debug << "Failed to create window. Terminating";
    JNGL::debug.lib_flush();
    exit(1);
  }
  glfwMakeContextCurrent(win);
  // Set callbacks
  glfwSetWindowSizeCallback(win, reshape);
  glfwSetKeyCallback(win, key);
  glEnable(GL_DEBUG_OUTPUT); glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(debug, NULL);

  // Enable VSync
  glfwSwapInterval(1);
  JNGL::debug << "Context created successfully"; JNGL::debug.lib_flush();

  // Create buffers
  createLDepthBuffer();
}

Scene* JNGLContext::newScene() {
  S = Scene();
  JNGL::debug << "New scene created: " << &S; JNGL::debug.lib_flush();
  return &S;
}

/*
 * Processes one frame of the content in the JNGL context.
*/
void JNGLContext::process() {

  // Set data pass shader to active
  glUseProgram(shader_dataPass.name);
  // Viewport/framebuffer(s) for data pass
  glViewport(0,0,JNGL::SHADOW_RESOLUTION,JNGL::SHADOW_RESOLUTION);
  glBindFramebuffer(GL_FRAMEBUFFER, ldepth_buffer);
  glClear(GL_DEPTH_BUFFER_BIT);
  S.lightDepthPass(win);

  // Set render pass shader to active
  glUseProgram(shader_renderPass.name);
  // Set the result of depth pass to texture 15 (last one that MUST exist)
  glActiveTexture(GL_TEXTURE15);
  glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, ldepth_cubemap);
  glActiveTexture(GL_TEXTURE0);
  glUniform1i(uniform("ldepth_cubemap"), 15);
  // Viewport/framebuffer settings for render pass
  glViewport(0,0,JNGL::WIN_W,JNGL::WIN_H);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  // Render scene
  S.renderPass(win);

  // Set no shader
  glUseProgram(0);
  // Flush and swap buffers
  glFlush();
  glfwSwapBuffers(win);
  // Check closing
  if(glfwWindowShouldClose(win)) exit(0);
}


// Callback stuff
void JNGLContext::setKeyCallback(GLFWkeyfun key) {
  glfwSetKeyCallback(win, key);
}