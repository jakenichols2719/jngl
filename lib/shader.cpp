#include "jngl.h"

static char* readFile(const char* filename) {
  using namespace std; // fstream
  // Open file
  fstream file (filename);
  if(!file.is_open()) {
    cout << "Couldn't open file " << filename << endl;
  }
  // Get file size
  file.seekg(0,ios::end);
  int size = file.tellg();
  file.seekg(0,ios::beg);

  char* buffer = new char[size];
  file.get(buffer,size,'\0');

  file.close();

  return buffer;
}

static void printLog(int prog) {
  GLint len = 0;
  glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
  if(len > 1) {
    char* log = new char[len];
    glGetProgramInfoLog(prog, len, nullptr, log);
    std::cout << log << std::endl;
  }
  GLint status = 0;
  glGetProgramiv(prog, GL_LINK_STATUS, &status);
  if(!status) exit(0);
}

static void attachShader(GLint P, GLenum type, const char* fn) {
  GLint shader = glCreateShader(type);
  // Compile source into shader
  char* src  = readFile(fn);
  glShaderSource(shader, 1, (const char**)&src, nullptr);
  glCompileShader(shader);
  // Attach shader to program
  glAttachShader(P, shader);
  // free memory
  delete[] src;
}

void compileShader(Shader* S) {
  JNGL::debug << "Compiling shader"; JNGL::debug.lib_flush();
  if(S->vs_file=="" || S->fs_file=="") {
    JNGL::debug << "Shader must have a vertex and fragment shader attached";
    JNGL::debug.lib_flush();
    return;
  }
  S->name = glCreateProgram();
  // Attach VS/FS
  attachShader(S->name, GL_VERTEX_SHADER, S->vs_file.c_str());
  attachShader(S->name, GL_FRAGMENT_SHADER, S->fs_file.c_str());
  if(S->gs_file != "") attachShader(S->name, GL_GEOMETRY_SHADER, S->gs_file.c_str());
  glLinkProgram(S->name);
  printLog(S->name);
}

Shader createShader(std::string files[5]) {
  JNGL::debug << "Creating shader with vertex file " << files[0]; JNGL::debug.lib_flush();
  Shader S;
  S.vs_file = files[0];
  S.tcs_file = files[1];
  S.tes_file = files[2];
  S.gs_file = files[3];
  S.fs_file = files[4];
  compileShader(&S);
  return S;
}