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
    vec3 normal = normalize(inNormal);
    vec3 lightDir = normalize(gubo.DlightDir);
    vec3 viewDir = normalize(gubo.eyePos - gl_FragCoord.xyz);

    // Lambertian diffuse factor
    float diffuseFactor = clamp(dot(normal, lightDir), 0.0f, 1.0f);

    // Blinn-Phong specular reflection
    vec3 halfwayDir = normalize(lightDir + viewDir);
    int shininess = 100;
    float specularFactor = clamp(pow(max(0.0, dot(normal, halfwayDir)), shininess), 0.0f, 1.0f);

    // Compute direct diffuse and specular lighting
    vec3 directDiffuse = gubo.DlightColor * diffuseFactor;
    vec3 directSpecular = gubo.DlightColor * specularFactor;

    // Calculate ambient lighting
    vec3 ambient = gubo.AmbLightColor;

    // Combine ambient, diffuse, and specular lighting
    vec3 finalColor = clamp(inColour * (ambient + directDiffuse) + directSpecular, 0.0f, 1.0f);

    outColor = vec4(finalColor, 1.0f);    // Final color with lighting
}