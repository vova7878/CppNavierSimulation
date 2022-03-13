#version 330 core

out float outData;

uniform ivec2 renderSize;
uniform sampler2D inData1;
uniform sampler2D inData2;

int width = renderSize.x, height = renderSize.y;

float value(sampler2D array, ivec2 xy){
    return texelFetch(array, xy, 0).r;
}

float value(sampler2D array, int x, int y){
    ivec2 xy = ivec2(x, y);
    return value(array, xy);
}

void main(){
    ivec2 map_xy = ivec2(gl_FragCoord.xy);
	ivec2 xy = map_xy;
	xy = max(xy, ivec2(1));
	xy = min(xy, ivec2(width, height) - 2);
    outData = (value(inData1, xy) + value(inData2, xy.x + 1, xy.y) + value(inData2, xy.x - 1, xy.y) + value(inData2, xy.x, xy.y + 1) + value(inData2, xy.x, xy.y - 1)) / 4.0;
}