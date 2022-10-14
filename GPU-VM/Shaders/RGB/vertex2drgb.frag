#version 450

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PER_OBJECT {
    mat2 modelvec;
    vec2 offset;
}pc;

vec3 lerp(vec3 a, vec3 b, float t) {
    return (1.0-t) * a + t * b;
}
vec2 lerp(vec2 a, vec2 b, float t) {
    return (1.0-t) * a + t * b;
}
float lerp(float a, float b, float t) {
    return (1.0-t) * a + t * b;
}


void main() {
    vec2 st = gl_FragCoord.xy;
    
    vec3 color = vec3(0.0);
    vec3 color1 = vec3(0.0);
    
    // Plot a line
    st.x = tan(st.x - st.y) + 15 + pc.offset.x * 3 ;
    
    float pct = smoothstep(10.0, 0.0, abs(st.x));
    
    color = lerp(color,vec3(fragColor), pct );
    
    st.x = tan(st.x) + 4 + (pc.offset.x - 45) * 6;
    
    color1 = lerp(color,vec3(1.0, 1.0, 0.0), pct );

    outColor = vec4((color +color1), 1.0);
}
