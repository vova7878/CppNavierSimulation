#version 330 core

out vec2 outData;

uniform ivec2 renderSize;
uniform sampler2D inData1;
uniform sampler2D inData2;
uniform float dt;
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
	vec2 last_xy = vec2(xy) - dt * value(inData2, xy);
	last_xy = max(last_xy, vec2(0));
	last_xy = min(last_xy, vec2(width, height) - 1.0);
	ivec2 int_xy = ivec2(last_xy);
	vec2 point = last_xy - vec2(int_xy);
	vec2 data = mix(
				          mix(value(inData1, int_xy), value(inData1, int_xy.x + 1, int_xy.y), point.x),
				          mix(value(inData1, int_xy.x, int_xy.y + 1), value(inData1, int_xy.x + 1, int_xy.y + 1), point.x),
					      point.y);
	outData = mix(data, -data, invert);
}