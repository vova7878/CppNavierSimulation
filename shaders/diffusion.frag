#version 330 core

out vec2 outData;

uniform ivec2 renderSize;
uniform sampler2D inData1;
uniform sampler2D inData2;
uniform float diffValue;
uniform bvec4 invertEdges;

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
	bvec4 tmp = notEqual(xy, map_xy).xyxy && invertEdges;
	bvec2 invert = bvec2(any(tmp.xy), any(tmp.zw));
    vec2 data = (value(inData1, xy) + diffValue * (value(inData2, xy.x + 1, xy.y) + value(inData2, xy.x - 1, xy.y) + value(inData2, xy.x, xy.y + 1) + value(inData2, xy.x, xy.y - 1))) / (1.0 + 4.0 * diffValue);
	outData = mix(data, -data, invert);
}