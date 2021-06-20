#include "jngl.h"

void Transformable::setPos(float x, float y, float z) {
  position = glm::vec3(x,y,z);
}
void Transformable::setRot(float p, float y, float r) {
  rotation = glm::vec3(p,y,r)/180.0f * 3.1415f;
}
void Transformable::setScl(float x, float y, float z) {
  scale = glm::vec3(x,y,z);
}

void Transformable::addPos(float x, float y, float z) {
  position += glm::vec3(x,y,z);
}
void Transformable::addRot(float p, float y, float r) {
  rotation += glm::vec3(p,y,r)/180.0f * 3.1415f;
}
void Transformable::addScl(float x, float y, float z) {
  scale += glm::vec3(x,y,z);
}

glm::mat4 Transformable::asModelMatrix() {
  // Translation
  glm::mat4 T = glm::translate(glm::mat4(1.0),position);
  //std::cout << glm::to_string(T) << std::endl;
  // Euler angle rotation
  glm::mat4 Ry = glm::rotate(glm::mat4(1.0),rotation.y,glm::vec3(0,1,0));
  glm::mat4 Rx = glm::rotate(glm::mat4(1.0),rotation.x,glm::vec3(1,0,0));
  glm::mat4 Rz = glm::rotate(glm::mat4(1.0),rotation.z,glm::vec3(0,0,1));
  glm::mat4 R = Rz*Rx*Ry;
  //std::cout << glm::to_string(R) << std::endl;
  // Scale
  glm::mat4 S = glm::scale(glm::mat4(1.0), scale);
  //std::cout << glm::to_string(S) << std::endl;
  // Return matrix product
  return T*R*S;
}

glm::mat4 Transformable::asViewMatrix() {
  // Translation (negative of position)
  glm::mat4 T = glm::translate(glm::mat4(1.0),-position);
  //std::cout << glm::to_string(T) << std::endl;
  // Euler angle rotation
  glm::mat4 Ry = glm::rotate(glm::mat4(1.0),rotation.y,-glm::vec3(0,1,0));
  glm::mat4 Rx = glm::rotate(glm::mat4(1.0),rotation.x,-glm::vec3(1,0,0));
  glm::mat4 Rz = glm::rotate(glm::mat4(1.0),rotation.z,-glm::vec3(0,0,1));
  glm::mat4 R = Rz*Rx*Ry;
  return R*T;
}