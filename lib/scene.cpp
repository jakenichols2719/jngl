#include "jngl.h"

/*
 * Adds an object to the scene
 * Returns a pointer to the object; if modified in main, modified in scene
*/
Object* Scene::addObject(ObjectBase B) {
  Object O(B);
  objects.push_back(O);
  JNGL::debug << "Added new object " << &objects[objects.size()-1]
              << " to scene " << this;
  JNGL::debug.lib_flush();
  return &objects[objects.size()-1];
}

/*
 * Removes an object from the scene
 * Currently leaves the pointer to the user, but it doesn't do anything
*/
void Scene::removeObject(Object* O) {
  for(unsigned int n=0; n<objects.size(); n++) {
    if(&objects[n] == O) {
      objects.erase(objects.begin()+n,objects.begin()+n+1);
      JNGL::debug << " Removed object " << O
                  << " from scene " << this;
      JNGL::debug.lib_flush();
      break;
    }
  }
}

/*
 * Returns reference to scene camera
*/
Camera* Scene::getCamera() {
  return &camera;
}

/*
 * Adds a new point light to the scene
*/
PointLight* Scene::getPointLight(int idx) {
   if(idx >= JNGL::MAX_POINT_LIGHTS) {
     JNGL::debug << "Attempted to get point light > max"; JNGL::debug.lib_flush();
     return nullptr;
   }
   return &lights[idx];
}

/*
 * Light depth pass
 * Calculates depth of fragments from each light
 * Stores result in the active framebuffer's depth attachment
*/
void Scene::lightDepthPass(GLFWwindow* win) {
  // "Shadow matrices" for cube projection of depth
  glm::mat4 sP = glm::perspective(90.0f,1.0f,JNGL::LIGHT_CLOSE,JNGL::LIGHT_FAR);
  glm::mat4 sM [6];
  sM[0] = sP*glm::lookAt(glm::vec3(0),glm::vec3(+1,0,0), glm::vec3(0,-1,0));
  sM[1] = sP*glm::lookAt(glm::vec3(0),glm::vec3(-1,0,0), glm::vec3(0,-1,0));
  sM[2] = sP*glm::lookAt(glm::vec3(0),glm::vec3(0,+1,0), glm::vec3(0,0,1));
  sM[3] = sP*glm::lookAt(glm::vec3(0),glm::vec3(0,-1,0), glm::vec3(0,0,-1));
  sM[4] = sP*glm::lookAt(glm::vec3(0),glm::vec3(0,0,+1), glm::vec3(0,-1,0));
  sM[5] = sP*glm::lookAt(glm::vec3(0),glm::vec3(0,0,-1), glm::vec3(0,-1,0));
  //std::cout << glm::to_string(sM[0]) << std::endl;
  glm::vec3 light_positions[JNGL::MAX_POINT_LIGHTS];
  for(int n=0; n<JNGL::MAX_POINT_LIGHTS; n++) {
    light_positions[n] = lights[n].position;
  }
  // Send uniforms to active shader (should be depth calculation shader)
  glUniformMatrix4fv(uniform("shadow_matrices"),6,false,&sM[0][0][0]);
  glUniform3fv(uniform("light_positions"),JNGL::MAX_POINT_LIGHTS,&light_positions[0][0]);
  glUniform1f(uniform("light_far"),JNGL::LIGHT_FAR);
  // Render all objects with useLighting
  for(unsigned int n=0; n<objects.size(); n++) {
    if(!objects[n].useLighting) continue;
    Object O = objects[n];
    glm::mat4 Model = O.asModelMatrix();
    glUniformMatrix4fv(uniform("Model"),1,false,&Model[0][0]);
    // Draw
    O.draw();
  }
}

/*
 * Render pass
 * Puts together data to push visuals to the screen
*/
void Scene::renderPass(GLFWwindow* win) {
  // View matrix (Need camera functionality in scene)
  glm::mat4 View = camera.asViewMatrix();//camera.getTransform()->View();
  glUniformMatrix4fv(uniform("View"),1,false,&View[0][0]);
  // Projection matrix (Also camera)
  float fovy = 57.0; float asp = (float)JNGL::WIN_W/(float)JNGL::WIN_H; float dim = 64;
  float near = dim/16; float far = dim*16;
  glm::mat4 Perspective = glm::perspective(fovy, asp, near, far);
  glUniformMatrix4fv(uniform("Projection"),1,false,&Perspective[0][0]);
  // Light positions
  glm::vec3 light_positions[JNGL::MAX_POINT_LIGHTS];
  for(int n=0; n<JNGL::MAX_POINT_LIGHTS; n++) {
    light_positions[n] = lights[n].position;
  }
  glUniform3fv(uniform("light_positions"),JNGL::MAX_POINT_LIGHTS, &light_positions[0][0]);
  glUniform1f(uniform("light_far"),JNGL::LIGHT_FAR);
  // Iterate through objects and display
  for(unsigned int n=0; n<objects.size(); n++) {
    Object O = objects[n];
    // Set model matrix
    glm::mat4 Model = O.asModelMatrix();
    glUniformMatrix4fv(uniform("Model"),1,false,&Model[0][0]);
    // Set normal matrix
    glm::mat3 Normal = glm::inverseTranspose(glm::mat3(View * Model));
    glUniformMatrix3fv(uniform("Normal"),1,false,&Normal[0][0]);
    // Draw object
    O.draw();
  }
}