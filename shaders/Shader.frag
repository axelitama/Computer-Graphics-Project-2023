#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl: enable

layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec2 inUV;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform sampler2D tex;

layout(set = 1, binding = 0) uniform GlobalUniformBlock {
    vec3 DlightDir;
    vec3 DlightColor;
    vec3 AmbLightColor;
    vec3 eyePos;
} gubo;

float OrenNayarDiffuse(vec3 V, vec3 N, vec3 L, float sigma)
{
    float theta_i = acos(dot(L, N));
	float theta_r = acos(dot(V, N));

	float alpha = max(theta_i, theta_r);
	float beta = min(theta_i, theta_r);

    float A = 1.0 - (0.5 * sigma * sigma / (sigma * sigma + 0.33));
    float B = 0.45 * sigma * sigma / (sigma * sigma + 0.09);

    vec3 V_i = normalize(L - dot(L, N) * N);
	vec3 V_r = normalize(V - dot(V, N) * N);

	float G = max(0, dot(V_i, V_r));

	float _L = clamp(dot(L, N), 0, 1);

	return _L * (A + B * G * sin(alpha) * tan(beta));
}

void main() {
    vec3 normal = normalize(inNormal);
    vec3 lightDir = normalize(-gubo.DlightDir);
    vec3 viewDir = normalize(gubo.eyePos - gl_FragCoord.xyz);

    // Oren-Nayar diffuse factor
    float roughness = 0.2;
    float diffuseFactor = OrenNayarDiffuse(viewDir, normal, lightDir, roughness);

    // Blinn-Phong specular reflection
    vec3 halfwayDir = normalize(lightDir + viewDir);
    int shininess = 50;
    float specularFactor = pow(max(0.0, dot(normal, halfwayDir)), shininess);

    // Compute diffuse and specular lighting
    vec3 directDiffuse = gubo.DlightColor * diffuseFactor;
    vec3 directSpecular = gubo.DlightColor * specularFactor;

    // Calculate ambient lighting
    vec3 ambient = gubo.AmbLightColor;

    // Combine ambient, diffuse, and specular lighting
    vec3 finalColor = texture(tex, inUV).rgb * (ambient + directDiffuse) + directSpecular;

    outColor = vec4(finalColor, 1.0f);    // Final color with lighting
}
