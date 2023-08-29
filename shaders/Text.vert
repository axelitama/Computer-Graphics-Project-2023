#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inPosition;
layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) in vec2 inUV;

layout(set = 0, binding = 1) uniform UniformBufferObject {
	mat4 mvpMat;
} ubo;

void main() {
    vec2 position = inPosition;
    gl_Position = ubo.mvpMat * vec4(position.x, position.y, 0.0, 1.0);
    vec2 uv = inUV;
    fragTexCoord = uv;
}
