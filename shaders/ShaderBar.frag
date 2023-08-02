#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 colour;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform GlobalUniformBlock {
	vec3 DlightDir;
	vec3 DlightColor;
	vec3 AmbLightColor;
	vec3 eyePos;
} gubo;

void main() {
	outColor = vec4(colour, 1.0f);		// main color
}