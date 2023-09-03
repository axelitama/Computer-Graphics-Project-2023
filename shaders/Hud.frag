#version 450

layout(set = 0, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {    
    vec4 Tx;
    Tx = texture(texSampler, fragTexCoord);
    
    float value0 = Tx[0];
    //Text Color
    vec4 result = vec4(1.0, 1.0, 1.0, 1.0) * value0;
    
    float value1 = Tx[1];
    //Outilne Color
    vec4 factor1 = vec4(0.0, 0.0, 0.0, 1.0) * value1;
    result = result + factor1;
    
    float value2 = Tx[2];
    //Shadow Color
    vec4 factor2 = vec4(0.0, 0.0, 0.0, 1.0) * value2;
    result = result + factor2;
    
    outColor = result;
}