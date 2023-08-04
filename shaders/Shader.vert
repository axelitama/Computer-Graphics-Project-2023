#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform UniformBufferObject {
	mat4 mvpMat;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec3 outNormal;
layout(location = 1) out vec2 outUV;

void main() {
	// float x = inPosition.x;
	// float y = inPosition.y;
	// float z = inPosition.z;

	// mat4 scaleMat = mat4(1);
	// //scaleMat[1][1] = ubo.height;

	// // Create a translation matrix
	// vec3 vpos = vec3(x, y, z);
	// gl_Position =  scaleMat  * ubo.mvpMat * vec4(vpos, 1);
	gl_Position = ubo.mvpMat * vec4(inPosition, 1.0);

	outNormal = inNormal;
	outUV = inUV;
}