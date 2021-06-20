#include "jngl.h"

Object::Object(ObjectBase B) {
  object_name = B.object_name;
  object_data = B.object_data;
}

// Draw object
// Assumes shaders and matrices are taken care of
void Object::draw() {
  // Iterate through object parts
  std::vector<ObjectPart> parts = object_data;
  for(unsigned int n=0; n<parts.size(); n++) {
    MTL M = parts[n].material;
    VBO O = parts[n].mesh;
    // Setup material
    glUniform4fv(uniform("Ambient"),1,&M.Ka[0]);
    glUniform4fv(uniform("Diffuse"),1,&M.Kd[0]);
    glUniform4fv(uniform("Specular"),1,&M.Ks[0]);
    glUniform1f(uniform("Shine"),M.Ns);
    // Color to diffuse
    glUniform4fv(uniform("Color"),1,&M.Kc[0]);
    // Setup VBO
    glBindBuffer(GL_ARRAY_BUFFER, O.name);
    // Set attributes
    glVertexAttribPointer(0,4,GL_FLOAT,0,36,(void*) 0); // Vertex
    glVertexAttribPointer(1,3,GL_FLOAT,0,36,(void*)16); // Texture
    glVertexAttribPointer(2,4,GL_FLOAT,0,36,(void*)24); // Normal
    glEnableVertexAttribArray(0); // Vertex
    glEnableVertexAttribArray(1); // Texture
    glEnableVertexAttribArray(2); // Normal

    // Draw
    glDrawArrays(O.draw_type, 0, O.vertex_count);

    // Disable attributes
    glDisableVertexAttribArray(0); // Vertex
    glDisableVertexAttribArray(1); // Texture
    glDisableVertexAttribArray(2); // Normal

    // Unset VBO
    glBindBuffer(GL_ARRAY_BUFFER,0);
  }
}