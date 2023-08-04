#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform UniformBufferObject {
	mat4 mvpMat;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 outColor;

void main() {
	// float x = inPosition.x;
	// float y = inPosition.y;
	// float z = inPosition.z;

	// Create a translation matrix
	//vec3 vpos = vec3(x, y+ubo.height, z);

	gl_Position = ubo.mvpMat * vec4(inPosition, 1.0);//* vec4(vpos, 1.0);
    outColor = inColor;
}