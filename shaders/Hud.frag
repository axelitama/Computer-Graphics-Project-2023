#version 450

layout(set = 0, binding = 0) uniform sampler2D texSampler;
layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;
layout(location = 1) in vec2 inUV;

void main() {
    outColor = vec4(texture(texSampler, inUV).rgb, 1.0);
}