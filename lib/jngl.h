#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

// OpenGL inclusions
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#ifdef __APPLE__
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

// Include GLM as the mathematics library
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtc/matrix_inverse.hpp"

/*
 * Debug
 * Can be enabled/disabled by changing vars
*/
class Debug {
private:
  std::string line = "";
public:
  static bool DEBUG_ON;
  static bool DEBUG_LIB_ON;
  static bool DEBUG_COUT;
  static bool DEBUG_FOUT;
  static std::ofstream fout;
  // Operators to insert info
  friend Debug& operator << (Debug& D, std::string s);
  friend Debug& operator << (Debug& D, int i);
  friend Debug& operator << (Debug& D, float f);
  friend Debug& operator << (Debug& D, void* p);
  // Flush to print info
  void flush();
  void lib_flush();
};

// Program constants/variables
struct JNGL {
  static Debug debug;
  // Window
  static int WIN_W;
  static int WIN_H;
  static char* WIN_NAME;
  // Lighting/Shadows
  const static int MAX_POINT_LIGHTS = 8;
  static float LIGHT_CLOSE;
  static float LIGHT_FAR;
  static int SHADOW_RESOLUTION;
};

// Shaders
struct Shader {
  GLint name = 0;
  std::string vs_file = "";
  std::string tcs_file = "";
  std::string tes_file = "";
  std::string gs_file = "";
  std::string fs_file = "";
};
void compileShader(Shader S);
// Create a shader with vs, tcs, tes, gs, and fs files in that order
Shader createShader(std::string files[5]);


// Loader structs
// OBJ File -> VBO
struct VBO {
  GLuint name = 0;
  GLint  vertex_count = 0;
  GLenum draw_type = GL_TRIANGLES;
  std::string mesh_name = "";
  std::string mtl_name = "";
};
std::vector<VBO> readOBJFile(std::string filename);

// BMP File -> TEX
struct TEX {
  GLuint name = 0;
  GLint  width = 0;
  GLint  height = 0;
};
//Texture loadBMPFile(std::string filename);

// MTL File -> MTL
struct MTL {
  std::string name = "";
  // Raw properties
  glm::vec4 Kc; // Color
  glm::vec4 Ka; // Ambient
  glm::vec4 Kd; // Diffuse
  glm::vec4 Ks; // Specular
  float     Ns; // Shininess
  // Maps
  TEX map_Kc; // Color
  TEX map_Ka; // Ambient
  TEX map_Kd; // Diffuse
  TEX map_Ks; // Specular
};
std::vector<MTL> readMTLFile(std::string filename);

struct ObjectPart {
  VBO mesh;
  MTL material;
};

/*
 * Transformable
 * Holds transformation data for all renderable objects
 * Should allow for parent-child relationships later
*/
struct Transformable {
  glm::vec3 position = glm::vec3(0,0,0);
  glm::vec3 rotation = glm::vec3(0,0,0);
  glm::vec3 scale = glm::vec3(1,1,1);
  void setPos(float x, float y, float z);
  void setRot(float pitch, float yaw, float roll);
  void setScl(float x, float y, float z);
  void addPos(float x, float y, float z);
  void addRot(float pitch, float yaw, float roll);
  void addScl(float x, float y, float z);
  glm::mat4 asModelMatrix();
  glm::mat4 asViewMatrix();
};

/*
 * Object Base
 * Contains rendering data for an object
 * Object Bases CANNOT be rendered--must be added to scene
*/
struct ObjectBase {
  // Vertex data
  std::string object_name;
  std::vector<ObjectPart> object_data;
};
ObjectBase loadObject(std::string folder, std::string object_name);

class Object : protected ObjectBase, public Transformable {
public:
  bool useLighting = true;
  Object(ObjectBase B);
  void draw();
};

class Camera : public Transformable {
private:
  //Transform transform;
public:
  Camera(){};
};

class PointLight : public Transformable {
private:
  bool enabled = false;
public:
  PointLight(){};
  void setEnabled(bool b) { enabled = b; };
};

/*
 * Scene
 * Can clone ObjectBases into the scene, returns Object pointer that can
 * be modified
*/
class Scene {
private:
  std::vector<Object> objects;
  PointLight lights [JNGL::MAX_POINT_LIGHTS]; // LIGHTS ALWAYS EXIST
  // Camera functionality; encapsulate later
  Camera camera;
public:
  Scene() {};
  Object* addObject(ObjectBase base);
  void removeObject(Object* ptr);
  PointLight* getPointLight(int idx);
  Camera* getCamera();
  void lightDepthPass(GLFWwindow* win);
  void renderPass(GLFWwindow* win);
  void clear();
};

// Context management
class JNGLContext {
private:
  // Window variables
  GLFWwindow* win;
  Scene S;
  // ldepth pass
  GLuint ldepth_buffer = 0;
  GLuint ldepth_cubemap = 0;
  void createLDepthBuffer();
  // render pass
public:
  // Variables
  Shader shader_dataPass;
  Shader shader_lightPass;
  Shader shader_renderPass;
  // Shader shader_postProcess;
  // Functions
  JNGLContext() {};
  void open();
  void process();
  void close();
  Scene* newScene();
  // Callback setting
  void setKeyCallback(GLFWkeyfun key);
};


// Generally helpful things

// Get active shader name
GLint activeShader();
// Get name for uniform location in active shader
GLint uniform(std::string name);
