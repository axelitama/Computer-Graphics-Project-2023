#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl: enable

layout(location = 0) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform sampler2D tex;

layout(set = 1, binding = 0) uniform GlobalUniformBlock {
	vec3 DlightDir;
	vec3 DlightColor;
	vec3 AmbLightColor;
	vec3 eyePos;
} gubo;


void main() {
	outColor = vec4(texture(tex, fragUV).rgb, 1.0f);		// main color
}