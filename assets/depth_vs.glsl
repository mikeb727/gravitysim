// simple Phong shader
#version 330 core
layout (location = 0) in vec3 bufpos;

uniform mat4 modelMat = mat4(1.0f); // initialize this! otherwise no vertices will be drawn
uniform mat4 viewMat = mat4(1.0f);
uniform mat4 projMat = mat4(1.0f);
uniform mat4 normalMat = mat4(1.0f);

out vec3 fragPos;

void main(){
    gl_Position = projMat * viewMat * modelMat * vec4(bufpos, 1.0);
}