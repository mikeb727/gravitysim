// Phong shading with per-triangle color alternation.
// Creates striped effect (assuming mesh is a sphere
// with 16 lat/lon segments) useful for visualizing
// rotation.

#version 330 core

layout (location = 0) in vec3 bufpos;
layout (location = 1) in vec3 bufnorm;
layout (location = 2) in vec2 buftexcoord;

uniform mat4 modelMat = mat4(1.0f); // initialize this! otherwise no vertices will be drawn
uniform mat4 viewMat = mat4(1.0f);
uniform mat4 projMat = mat4(1.0f);
uniform mat4 normalMat = mat4(1.0f);
uniform mat4 lightMat = mat4(1.0f);

out VS_OUT {
    vec3 fragPos;
    vec4 fragPos_l; // light space transformation
    vec3 normalVec;
    vec2 texCoords;
    flat int bw; // whether vertex is black or color (for checkerboard pattern)
} vs_out;

void main(){
    vs_out.fragPos = vec3(modelMat * vec4(bufpos, 1.0));
    gl_Position = projMat * viewMat * vec4(vs_out.fragPos, 1.0);
    vs_out.fragPos_l = lightMat * vec4(vs_out.fragPos, 1.0);
    vs_out.normalVec = vec3(normalMat * vec4(bufnorm, 0.0));
    vs_out.texCoords = buftexcoord;
    // assign each vertex a "black" or "white" value
    // to be multiplied with Phong diffuse color
    // vs_out.bw = int(floor(((gl_VertexID % 12) / 6) + floor(gl_VertexID / 96))) % 2;
    vs_out.bw = int(floor((gl_VertexID % 48) / 24));
}
