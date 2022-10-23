#version 450

layout(location = 0) in vec4 fragColor;

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

float quadraticBezier (float x, vec2 a){
  // adapted from BEZMATH.PS (1993)
  // by Don Lancaster, SYNERGETICS Inc. 
  // http://www.tinaja.com/text/bezmath.html

  float epsilon = 0.00001;
  a.x = clamp(a.x,0.0,1.0);
  a.y = clamp(a.y,0.0,1.0);
  if (a.x == 0.5){
    a += epsilon;
  }
  
  // solve t from x (an inverse operation)
  float om2a = 1.0 - 2.0 * a.x;
  float t = (sqrt(a.x*a.x + om2a*x) - a.x)/om2a;
  float y = (1.0-2.0*a.y)*(t*t) + (2.0*a.y)*t;
  return y;
}
float plot(vec2 st, float pct){
  return  smoothstep( pct-0.02, pct, st.y) 
  //- smoothstep( pct, pct+0.4, st.y)
  ;
}
void main() {
    vec2 st = gl_FragCoord.xy / vec2(800, 600);
    float px = 1.0 / 600;
    // float y = st.x + pc.offset.x;
    vec2 cp = vec2(pc.offset.x,pc.offset.y) * 0.55 + 0.5; //  
    
    float pct = quadraticBezier(st.x, cp);
    vec3 color = vec3(smoothstep(pct, pct+px, st.y));;

    // color = lerp(color, vec3(0.0,1.0,0.0), pct);

    outColor = vec4(color, fragColor.w);
}
