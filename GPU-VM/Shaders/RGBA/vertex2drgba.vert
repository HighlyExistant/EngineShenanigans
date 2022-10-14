#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 rgba;

layout(location = 0) out vec4 fragColor;

layout(push_constant) uniform Push {
    mat2 modelvec; // 16
    vec2 offset; // 8
} push; // Current size: (24)

layout(binding = 0) uniform UniformBuffer {
    vec2 offset;
} ubo;

void main() {
    vec2 actual_pos = vec2(push.modelvec * position);
    gl_Position = vec4(actual_pos, 0.0, 1.0);
    fragColor = rgba;
}