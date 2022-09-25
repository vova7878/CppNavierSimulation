#version 430 core

uniform ivec2 renderSize;
uniform ivec2 dataSize;

restrict buffer ssbo1 {
    float data[];
};

out vec3 outData;

void main(){
    ivec2 xy = ivec2(gl_FragCoord.xy);
	ivec2 rxy = xy - (renderSize - dataSize) / 2;
	if(any(greaterThan(rxy, dataSize - 1)) || any(lessThan(rxy, ivec2(0)))){
	    discard;
	}
    outData = vec3(data[rxy.y * dataSize.x + rxy.x], 0.0, 0.0);
	//data[rxy.y * dataSize.x + rxy.x] = 1.0 - data[rxy.y * dataSize.x + rxy.x];
}
