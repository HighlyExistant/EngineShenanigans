#version 450


// layout(location = 0) in vec2 position;
// layout(location = 1) in vec3 color;
// layout(push_constant) uniform Push {
//     vec2 modelvec;
// } push; // Current size: (8)
// layout(binding = 0) uniform UniformBuffer {
//     vec2 offset;
// } ubo;
// layout(location = 0) out vec3 fragColor;
// void main() {
//     vec2 actual_pos = position + push.modelvec + ubo.offset;
//     gl_Position = vec4(actual_pos, 0.0, 1.0);
//     fragColor = color;
// }

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texture;

// layout(location = 0) out vec3 fragColor;
layout(location = 0) out vec2 fragTexCoord;

layout(push_constant) uniform Push {
    
    mat2 modelvec;
    vec2 offset;
    int index;
} push; // Current size: (28)

layout(binding = 0) uniform UniformBuffer {
    vec2 offset;
} ubo;

void main() {
    vec2 actual_pos = vec2(push.modelvec * position + push.offset);
    gl_Position = vec4(actual_pos, 0.0, 1.0);
    fragTexCoord = texture + push.offset;
}