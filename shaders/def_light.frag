#version 450 core

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform mat3 Normal;
uniform vec4 Color;

uniform vec3 light_positions[8];
uniform float light_range;

in VS_TO_FS {
  vec4 vPos;
  vec2 vTex;
  vec3 vNrm;
  vec4 vCol;
} fs_in;

layout (location=0) out vec4 frag_color;

// Calculate intensity of ambient, diffuse, and specular light
vec3 phong_intensities(int light_idx) {
  // Coordinate of fragment
  vec3 frag_pos = vec3(View * Model * fs_in.vPos);
  // Transformed normal vector
  vec3 frag_nrm = normalize(Normal * fs_in.vNrm);
  // Light vector (start with just light 0)
  vec3 vec_light = normalize(vec3(View * vec4(light_positions[light_idx],1))-frag_pos);

  vec3 intensity;
  // Ambient color
  vec4 color = fs_in.vCol*.3;
  // Check distance
  if(length(vec3(View * vec4(light_positions[light_idx],1))-frag_pos) > light_range) return color;

  // Diffuse/Specular
  float I_diffuse = dot(vec_light, frag_nrm);
  if(I_diffuse>0.0) {
    color += I_diffuse*fs_in.vCol;
  }
  return intensity;
}