#version 450 core

// 1 invocation for each light
layout(triangles, invocations = 8) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 shadow_matrices[6];
uniform vec3 light_positions[8];

out GS_TO_FS {
  vec3 light_pos;
  vec4 frag_pos;
} gs_out;

void main() {
  // Invocation defines cubemap layer and current active light
  int layer = gl_InvocationID;
  vec3 pos = light_positions[layer]; // position of current light
  mat4 ViewTranslate = mat4(
    1,0,0,-pos.x,
    0,1,0,-pos.y,
    0,0,1,-pos.z,
    0,0,0,1
  );
  for(int face=0; face<6; face++) {
    // Formula for cubemap array face in gl_Layer docs
    gl_Layer = layer*6 + face;
    // Iterate through input
    for(int n=0; n<gl_in.length(); n++) {
      gs_out.light_pos = pos;
      gs_out.frag_pos = gl_in[n].gl_Position;
      mat4 SM = ViewTranslate * shadow_matrices[face];
      gl_Position = SM * gs_out.frag_pos;
      EmitVertex();
    }
    EndPrimitive();
  }
}
