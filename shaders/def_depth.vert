#version 450 core

layout(location=0) in vec4 Vertex;

uniform mat4 Model;

void main() {
  gl_Position = Model * Vertex;
}
