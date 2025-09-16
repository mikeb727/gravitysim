#version 330 core

uniform bool useTex;
uniform sampler2D tex;
uniform vec4 color;

out vec4 fragColor;

in VS_OUT {
    vec2 texCoords;
    float bw;
} fs_in;

void main(){
    fragColor = color * fs_in.bw * vec4(1.0, 1.0, 1.0, useTex ? texture(tex, fs_in.texCoords).r : 1.0);
}