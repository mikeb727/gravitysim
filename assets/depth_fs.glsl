#version 330 core

out vec4 fragColor;

float nearZ = 0.1;
float farZ = 100.0;

float linearDepth(float d){
  float depthNdc = (d * 2.0) - 1.0;
  return (2.0 * nearZ * farZ) / (farZ + nearZ - depthNdc * (farZ - nearZ));
}

void main(){
  float depth = gl_FragCoord.z;
  fragColor = vec4(vec3(depth), 1.0);
};