#version 450

layout(location = 0) in vec2 position;  // x, y
layout(location = 1) in vec4 rgba;  // rgb + opacity

layout(location = 0) out vec4 fragColor;

layout(push_constant) uniform Push {
    
    mat2 modelvec;  // transform
    vec2 offset;
    int index;
} push; // Current size: (28)

layout(binding = 0) uniform UniformBuffer {
    vec2 offset;
} ubo;

void main() {
    vec2 actual_pos = vec2(push.modelvec * position + push.offset);
    gl_Position = vec4(actual_pos, 0.0, 1.0);
    fragColor = rgba;
}