#version 450 core

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec4 Color;

layout(location = 0) in vec4 Vertex;
layout(location = 1) in vec2 Texture;
layout(location = 2) in vec3 Normal;

out VS_TO_FS {
  out vec4 vPos;
  out vec2 vTex;
  out vec3 vNrm;
  out vec4 vCol;
} vs_out;

void main() {
  mat4 MVP = Projection * View * Model;
  gl_Position = MVP * Vertex;
  vs_out.vPos = Vertex;
  vs_out.vTex = Texture;
  vs_out.vNrm = Normal;
  vs_out.vCol = Color;
}
