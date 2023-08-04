#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec3 inColour;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform GlobalUniformBlock {
	vec3 DlightDir;
	vec3 DlightColor;
	vec3 AmbLightColor;
	vec3 eyePos;
} gubo;

void main() {
	outColor = vec4(inColour, 1.0f);		// main color
}