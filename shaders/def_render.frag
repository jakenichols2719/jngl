#version 450 core

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform mat3 Normal;

uniform vec3 light_positions[8];
uniform float light_far;

// Material properties
uniform vec4 Ambient;
uniform vec4 Diffuse;
uniform vec4 Specular;
uniform vec4 Color;
uniform float Shine;

// Textures
uniform samplerCubeArray ldepth_cubemap;

in VS_TO_FS {
  vec4 vPos;
  vec2 vTex;
  vec3 vNrm;
  vec4 vCol;
} fs_in;

layout (location=0) out vec4 frag_color;


float shadow(vec3 frag_pos, int light) {
  vec3 frag_to_light = frag_pos-light_positions[light];
  float closest_depth = texture(ldepth_cubemap, vec4(frag_to_light,light)).r * light_far;
  float current_depth = length(frag_to_light);
  float bias = 0.08;
  float s = current_depth-bias < closest_depth ? 1.0 : 0.0;
  //return current_depth/light_far;
  return texture(ldepth_cubemap, vec4(frag_to_light,light)).r;
}

vec3 max_elements(vec3 a, vec3 b) {
  vec3 c = vec3(0,0,0);
  c.x = max(a.x,b.x);
  c.y = max(a.y,b.y);
  c.z = max(a.z,b.z);
  return c;
}

vec3 calcLightIntensity(int light_idx) {
  // Coordinate of fragment
  vec3 frag_pos = vec3(View * Model * fs_in.vPos);
  // Transformed normal vector
  vec3 frag_nrm = normalize(Normal * fs_in.vNrm);
  // Light vector (start with just light 0)
  vec3 vec_light = normalize(vec3(View * vec4(light_positions[light_idx],1))-frag_pos);

  // Create intensity vector, first element with ambient
  vec3 I = vec3(.5,0,0);
  // Check distance
  if(length(vec3(View * vec4(light_positions[light_idx],1))-frag_pos) > light_far) {
    return I;
  };
  // Diffuse
  I.y = dot(vec_light, frag_nrm);
  // Specular
  if(I.y > 0.0) {
    // Reflection of light across normal
    vec3 R = reflect(-vec_light, frag_nrm);
    // Vector from fragment to eye
    vec3 V = normalize(-frag_pos);
    // Specular is dot of reflection and view
    float spec_base = dot(R,V);
    I.z = pow(spec_base, Shine); // Shininess
  }
  return I;
}

void main() {
  // Get max (base) light intensity at this fragment
  vec3 max_I = vec3(0,0,0);
  for(int n=0; n<1; n++) {
    vec3 temp = calcLightIntensity(n);
    max_I = max_elements(max_I, temp);
  }
  // Apply to colors
  float shadowMult = shadow(vec3(fs_in.vPos),0);
  vec4 A = Ambient.r * Color * max_I.x;
  vec4 D = Diffuse * max_I.y * shadowMult;
  vec4 S = Specular * max_I.z * shadowMult;
  //frag_color = vec4(vec3(A+D+S)*0.5 + shadowMult*0.5 ,1);
  frag_color = vec4(vec3(shadowMult),1);
  //frag_color = vec4(rgb, 1);
  //frag_color = vec4(vec3(A+D+S),1);
}
