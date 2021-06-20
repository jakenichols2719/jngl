#include "jngl.h"
#include <sstream> // needed for some formatting

bool Debug::DEBUG_ON = true;
bool Debug::DEBUG_LIB_ON = true;
bool Debug::DEBUG_COUT = false;
bool Debug::DEBUG_FOUT = true;
std::ofstream Debug::fout ("debug.txt");

Debug& operator << (Debug& D, std::string s) {
  D.line += s;
  return D;
}

Debug& operator << (Debug& D, int i) {
  D.line += std::to_string(i);
  return D;
}

Debug& operator << (Debug& D, float f) {
  D.line += std::to_string(f);
  return D;
}

Debug& operator << (Debug& D, void* p) {
  std::stringstream ss;
  ss << p;
  D.line += ss.str();
  return D;
}

void Debug::flush() {
  if(!Debug::DEBUG_ON) return;
  if(DEBUG_COUT) std::cout << line << std::endl;
  if(DEBUG_FOUT) Debug::fout << line << std::endl;
  line = "";
}

void Debug::lib_flush() {
  if(!Debug::DEBUG_LIB_ON) {
    line = "";
    return;
  }
  line = "JNGL: " + line;
  flush();
}