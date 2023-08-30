#version 450

layout(location = 0) in vec3 fragColor; // Input color from vertex shader
layout(location = 0) out vec4 outColor; // Output color of the fragment shader

void main() {
    outColor = vec4(fragColor, 1.0); // Pass the vertex color as the fragment color
}
