#version 330 core

out vec3 outData;

uniform ivec2 renderSize;
uniform sampler2D inData1;
uniform sampler2D inData2;

int width = renderSize.x, height = renderSize.y;

vec2 value(sampler2D array, ivec2 xy){
    return texelFetch(array, xy, 0).rg;
}

void main(){
    ivec2 xy = ivec2(gl_FragCoord.xy);
	xy.y = height - xy.y - 1;
	
	// скорость
	/*float md = length(value(inData2, xy));
	float col = md / 50.0 + 0.5;
	outData = vec3(col, col, col);*/
	
	// цвет
	/*vec2 col = value(inData1, xy) * 50.0 / 255.0;
	float md = length(value(inData2, xy)) / 50.0;
	outData = vec3(col.y * 0.7 + col.x * 0.2 + 0.1 * md, col.y * 0.95 + 0.05 * md, col.x * 1.05 - md * 0.05);*/
	
	//ещё один вариант цвета
	vec2 c1 = value(inData1, xy) * 50.0 / 255.0;
	//vec2 c2 = ((value(inData2, xy) - 1.0) * 2.0 + 1.0) * 50.0 / 255.0;
	vec2 c2 = value(inData2, xy) * 50.0 / 255.0;
	outData = vec3(c1.x * 0.75 + c1.y * 0.25, c1.y * 0.75 + c2.x * 0.25, c2.x * 0.75 + c1.x * 0.25);
}