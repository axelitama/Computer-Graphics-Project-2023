#version 450

layout(location = 0) in vec3 inPosition; // Vertex position
layout(location = 1) in vec3 inColor;    // Vertex color

layout(location = 0) out vec3 fragColor; // Output color for fragment shader

layout(set = 0, binding = 0) uniform UniformBufferObject {
	mat4 mvpMat;
} ubo;

void main() {
    gl_Position = ubo.mvpMat * vec4(inPosition, 1.0);
    fragColor = inColor;
}
