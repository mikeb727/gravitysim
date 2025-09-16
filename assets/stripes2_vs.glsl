#version 330 core
layout (location = 0) in vec4 bufpos;

uniform mat4 transform;
uniform float drawDepth;

out VS_OUT {
    vec2 texCoords;
    float bw;
} vs_out;

void main(){
    gl_Position = transform * vec4(bufpos.xy, drawDepth, 1.0);
    vs_out.texCoords = bufpos.zw;
    vs_out.bw = 0.2 + (floor((gl_VertexID % 12) / 6.0) * 0.5);
}