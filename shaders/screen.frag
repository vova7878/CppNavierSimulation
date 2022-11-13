#version 430 core

layout(origin_upper_left) in vec4 gl_FragCoord;

uniform ivec2 renderSize;
uniform ivec2 dataSize;

restrict buffer ssbo1 {
    vec4 data[];
};

out vec3 outData;

void main(){
    ivec2 xy = ivec2(gl_FragCoord.xy);
	ivec2 rxy = xy - (renderSize - dataSize) / 2;
	if(any(greaterThan(rxy, dataSize - 1)) || any(lessThan(rxy, ivec2(0)))){
	    discard;
	}
    outData = data[rxy.y * dataSize.x + rxy.x].xyz;
}
