#version 330 core

out float outData;

uniform ivec2 renderSize;
uniform sampler2D inData;

int width = renderSize.x, height = renderSize.y;

vec2 value(sampler2D array, ivec2 xy){
    return texelFetch(array, xy, 0).rg;
}

vec2 value(sampler2D array, int x, int y){
    ivec2 xy = ivec2(x, y);
    return value(array, xy);
}

void main(){
    ivec2 map_xy = ivec2(gl_FragCoord.xy);
	ivec2 xy = map_xy;
	xy = max(xy, ivec2(1));
	xy = min(xy, ivec2(width, height) - 2);
    outData = -0.5 * (value(inData, xy.x + 1, xy.y).x - value(inData, xy.x - 1, xy.y).x + value(inData, xy.x, xy.y + 1).y - value(inData, xy.x, xy.y - 1).y);
}