#version 450 core

uniform float light_far;

in GS_TO_FS {
  vec3 light_pos;
  vec4 frag_pos;
} fs_in;

void main() {
  vec3 relative_pos = fs_in.frag_pos.xyz-fs_in.light_pos;
  float dist = length(fs_in.frag_pos.xyz-fs_in.light_pos);
  dist = dist/light_far;
  gl_FragDepth = relative_pos/light_far;
}
