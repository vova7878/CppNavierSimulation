#version 330 core

out vec2 outData;

uniform ivec2 renderSize;
uniform vec2 add1;
uniform vec2 add2;
uniform vec2 scale;
uniform sampler2D inData;

int width = renderSize.x, height = renderSize.y;

vec2 value(sampler2D array, ivec2 xy){
    return texelFetch(array, xy, 0).rg;
}

void main(){
    ivec2 xy = ivec2(gl_FragCoord.xy);
	outData = add2 + (value(inData, xy) + add1) * scale;
}