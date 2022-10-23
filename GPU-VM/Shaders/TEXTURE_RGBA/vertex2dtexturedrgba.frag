#version 450

layout(location = 0) in vec2 fragTexCoord;

layout(location = 1) in vec4 fragColor;

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
    
    outColor = text + fragColor;
}