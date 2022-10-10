#version 450
// layout (location = 0) in vec3 fragColor;
// layout (location = 0) out vec4 outColor;

// void main() {
//     outColor = vec4(fragColor, 1.0);
// }
layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler[3];

layout(push_constant) uniform PER_OBJECT
{
    mat2 modelvec;
    vec2 offset;
    int index;
}pc;
void main() {
    
    vec4 text = texture(texSampler[pc.index], fragTexCoord);
    
    // float hairval = float(0.333333 <= text.x && 0.180392 <= text.y && 0.0431373 <= text.z);
    // 0.333333, 0.180392, 0.0431373
    outColor = text - vec4(0.0,0.0,0.0,0.5);
}