#include "jngl.h"

static std::vector<std::string> splitstr(std::string s, char d) {
  std::vector<std::string> tokens;
  unsigned int pos = 0;
  while(true) {
    int i = s.find(d, pos);
    if(i == -1) {
      tokens.push_back(s.substr(pos));
      break;
    } else {
      tokens.push_back(s.substr(pos,i-pos));
    }
    pos = i+1;
  }
  return tokens;
}

static std::vector<std::string> splitstr(std::string s, std::string d) {
  std::vector<std::string> tokens;
  unsigned int pos = 0;
  while(true) {
    int i = s.find(d, pos);
    if(i == -1) {
      tokens.push_back(s.substr(pos));
      break;
    } else {
      tokens.push_back(s.substr(pos,i-pos));
    }
    pos = i+d.size();
  }
  return tokens;
}

/*
 * Reads an mtl file into a vector of materials
 * Continuously loads data into the last material present in the vector
*/
std::vector<MTL> readMTLFile(std::string filename) {
  std::vector<MTL> ret; // Return vector
  int idx = -1; // Current index for data
  std::ifstream file (filename);
  char line[100];
  while(!file.eof()) {
    // Get lines
    file.getline(line,100,'\n');
    std::string strline = std::string(line);
    if(strline.size()==0) continue; // skip empty lines
    // Line up to first space is identifier
    std::string id = strline.substr(0,strline.find(' '));
    // Content of line is remaining after first space
    std::string content = strline.substr(strline.find(' ')+1);
    // Act based on id of line
    if(id == "newmtl") { // Create a new material and set its name
      MTL m;
      m.name = content;
      ret.push_back(m);
      idx++;
    }
    if(id == "Ns") { // Set shininess
      float shine = std::stof(content);
      ret[idx].Ns = shine;
    }
    if(id == "Ka") { // Set ambient color
      std::vector<std::string> values = splitstr(content,' ');
      glm::vec4 col_ambient;
      col_ambient.r = std::stof(values[0]);
      col_ambient.g = std::stof(values[1]);
      col_ambient.b = std::stof(values[2]);
      col_ambient.a = 1.0;
      ret[idx].Ka = col_ambient;
    }
    if(id == "Kd") { // Set diffuse color
      std::vector<std::string> values = splitstr(content,' ');
      glm::vec4 col_diffuse;
      col_diffuse.r = std::stof(values[0]);
      col_diffuse.g = std::stof(values[1]);
      col_diffuse.b = std::stof(values[2]);
      col_diffuse.a = 1.0;
      ret[idx].Kc = col_diffuse;
      ret[idx].Kd = col_diffuse;
    }
    if(id == "Ks") { // Set specular color
      std::vector<std::string> values = splitstr(content,' ');
      glm::vec4 col_specular;
      col_specular.r = std::stof(values[0]);
      col_specular.g = std::stof(values[1]);
      col_specular.b = std::stof(values[2]);
      col_specular.a = 1.0;
      ret[idx].Ks = col_specular;
    }
  }
  return ret;
}

// Populate a buffer with values from a source array
static void populateBuffer(std::vector<float>* buf, std::vector<float> src,
                           int idx, int step) {
  int i = (idx-1)*step;
  for(int n=0; n<step; n++) {
    buf->push_back(src[i+n]);
  }
}
// Fill with a static value
static void populateBuffer(std::vector<float>* buf,
                           float val, int step) {
  for(int n=0; n<step; n++) {
    buf->push_back(val);
  }
}

static GLuint createGLBuffer(std::vector<float>* vbuf, int vertex_count) {
  GLuint name = 0;
  // Generate buffer
  glGenBuffers(1, &name);
  // Bind buffer and send data
  glBindBuffer(GL_ARRAY_BUFFER, name);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertex_count*9, vbuf->data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  return name;
}

/*
 * Read an object file
 * Similar approach to above
 * Finish object if line.id != f and line-1.id == f
*/
std::vector<VBO> readOBJFile(std::string filename) {
  std::vector<VBO> ret;
  int idx = -1;
  std::ifstream file (filename);
  char line[100];
  // Booleans to determine functionality
  bool has_tex = false;
  bool end = false;
  // Track active mesh name
  std::string active_MeshName = "NONE";
  // Vectors to gather data
  std::vector<float> vtx;
  std::vector<float> tex;
  std::vector<float> nrm;
  std::vector<float> vbuf;
  while(!file.eof()) {
    file.getline(line,100,'\n');
    std::string strline = std::string(line);
    // File ends with empty line, have to end last mesh creation here
    if(strline.size()==0 && end) {
      //std::cout << "Creating VBO " << active_MeshName << std::endl;
      ret[idx].vertex_count = vbuf.size()/9;
      ret[idx].name = createGLBuffer(&vbuf, vbuf.size()/9);
      vbuf.clear();
    }
    // Line up to first space is identifier
    std::string id = strline.substr(0,strline.find(' '));
    // Content of line is remaining after first space
    std::string content = strline.substr(strline.find(' ')+1);
    // Act based on id of line
    if(id == "usemtl" && end) { // End mesh creation
      // Reset booleans
      //has_tex = false;
      end = false;
      // Set vertex count
      ret[idx].vertex_count = vbuf.size()/9; // Each vertex takes 9 floats
      // Create actual OpenGL VBO
      //std::cout << "Creating VBO " << active_MeshName << std::endl;
      ret[idx].name = createGLBuffer(&vbuf, vbuf.size()/9);
      // Clear vertex buffer for next one
      vbuf.clear();
    }
    if(id == "o") { // Set active mesh name and reset texture tracking
      active_MeshName = content;
      has_tex = false;
    }
    if(id == "v") { // Append to vertex vector
      std::vector<std::string> values = splitstr(content,' ');
      for(int n=0; n<3; n++) {
        vtx.push_back(std::stof(values[n]));
      }
      vtx.push_back(1.0); // w value
    }
    if(id == "vt") { // Append to texture vector
      has_tex = true;
      std::vector<std::string> values = splitstr(content,' ');
      for(int n=0; n<2; n++) {
        tex.push_back(std::stof(values[n]));
      }
    }
    if(id == "vn") { // Append to normal vector
      std::vector<std::string> values = splitstr(content,' ');
      for(int n=0; n<3; n++) {
        nrm.push_back(std::stof(values[n]));
      }
    }
    if(id == "usemtl") { // Create new mesh with this material
      VBO m;
      m.mesh_name = active_MeshName;
      ret.push_back(m);
      idx++;
      ret[idx].mtl_name = content;
    }
    if(id == "f") { // Send the relevant data to vertex buffer
      //std::cout << "Face: " << content << std::endl;
      end = true; // will end after faces are complete
      // Get faces
      std::vector<std::string> dat = splitstr(content, ' ');
      // Iterate through data points and append to vbuf/
      for(unsigned int n=0; n<dat.size(); n++) {
        if(has_tex) {
          std::vector<std::string> indices = splitstr(dat[n],'/');
          //vbuf.push_back(vtx[std::stoi(indices[0])-1]);
          populateBuffer(&vbuf, vtx, std::stoi(indices[0]), 4);
          //vbuf.push_back(tex[std::stoi(indices[1])-1]);
          populateBuffer(&vbuf, tex, std::stoi(indices[1]), 2);
          //vbuf.push_back(nrm[std::stoi(indices[2])-1]);
          populateBuffer(&vbuf, nrm, std::stoi(indices[2]), 3);
        } else {
          std::vector<std::string> indices = splitstr(dat[n],"//");
          //vbuf.push_back(vtx[std::stoi(indices[0])-1]);
          populateBuffer(&vbuf, vtx, std::stoi(indices[0]), 4);
          //vbuf.push_back(-1.0); // Indicate lack of texture
          populateBuffer(&vbuf, -1.0, 2);
          //vbuf.push_back(nrm[std::stoi(indices[1])-1]);
          populateBuffer(&vbuf, nrm, std::stoi(indices[1]), 3);
        }
      }
    }
  }
  // Clear vectors
  vtx.clear();
  tex.clear();
  nrm.clear();
  return ret;
}

// Take path to folder and object name (ex. cube.obj, cube.mtl = cube)
// Output an object with meshes and materials
ObjectBase loadObject(std::string folder, std::string object_name) {
  JNGL::debug << "Loading object " << object_name
              << " from folder " << folder;
  JNGL::debug.lib_flush();
  // Create file paths
  std::string obj_path = folder+"/"+object_name+".obj";
  std::string mtl_path = folder+"/"+object_name+".mtl";
  // Read materials to be assigned by name
  std::vector<MTL> materials = readMTLFile(mtl_path);
  // Then read meshes from OBJ file
  std::vector<VBO> meshes = readOBJFile(obj_path);
  // Create an array to hold the object parts
  std::vector<ObjectPart> parts;
  // Loop through meshes, assign materials, and add to parts
  for(int n=0; n<(int)meshes.size(); n++) {
    ObjectPart part;
    std::string target = meshes[n].mtl_name;
    for(int m=0; m<(int)materials.size(); m++) {
      if(materials[m].name == target) {
        part.mesh = meshes[n];
        part.material = materials[m];
        break;
      }
    }
    parts.push_back(part);
  }
  // Return a new object with these parts
  ObjectBase O; O.object_data = parts; O.object_name = object_name;
  JNGL::debug << object_name << " loaded";
  JNGL::debug.lib_flush();
  return O;
}