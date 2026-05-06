#version 120

uniform vec2 resolution;
uniform float time;
uniform vec2 mouse;
uniform sampler2D occlusionMap;

#define blur 1.
#define DITHER
#define QUALITY 2
#define DECAY 0.974
#define EXPOSURE 0.4

#if (QUALITY==2)
 #define SAMPLES 64
 #define DENSITY 0.97
 #define WEIGHT 0.25
#elif (QUALITY==1)
 #define SAMPLES 32
 #define DENSITY 0.95
 #define WEIGHT 0.25
#else
 #define SAMPLES 16
 #define DENSITY 0.93
 #define WEIGHT 0.36
#endif

#define ViewZoom 3.
#define SS (blur/min(resolution.x, resolution.y))

vec2 fra(vec2 u) {
    return (u - 0.5 * resolution.xy) * ViewZoom / resolution.y;
}

float bayer2(vec2 a) {
    a = floor(a);
    return fract(dot(a, vec2(0.5, a.y * 0.75)));
}

float bayer4(vec2 a) { return bayer2(0.5 * a) * 0.25 + bayer2(a); }
float bayer8(vec2 a) { return bayer4(0.5 * a) * 0.25 + bayer2(a); }
float bayer16(vec2 a) { return bayer4(0.25 * a) * 0.0625 + bayer4(a); }
float bayer32(vec2 a) { return bayer8(0.25 * a) * 0.0625 + bayer4(a); }
float bayer64(vec2 a) { return bayer8(0.125 * a) * 0.015625 + bayer8(a); }
float bayer128(vec2 a) { return bayer16(0.125 * a) * 0.015625 + bayer8(a); }

#define dither128(p) (bayer128(p)-0.499969482)

float iib(vec2 u) {
    return dither128(u);
}

vec3 sun(vec2 uv) {
    // Automatic slow movement like the original Shadertoy but 3x slower
    float slowTime = time * 0.3;
    vec2 p = vec2(sin(slowTime), sin(slowTime * 0.5) * 0.5);
    
    vec3 res;
    float di = distance(uv, p);
    res.x = di <= 0.3333 ? sqrt(1.0 - di * 3.0) : 0.0;
    res.yz = p;
    res.y /= (resolution.x / resolution.y);
    res.yz = (res.yz + 1.0) * 0.5;
    return res;
}

float circle(vec2 p, float r) {
    return smoothstep(SS, -SS, length(p) - r);
}

vec4 BA(vec2 uv_norm) {
    // Sample the occlusion map with a small Y-offset correction
    float occluders = texture2D(occlusionMap, uv_norm - vec2(0.0, -0.03)).r;
    
    vec2 st = uv_norm * 2.0 - 1.0;
    float aspect = resolution.x / resolution.y;
    st.x *= aspect;
    
    vec3 light = min(sun(st), 1.0);
    float col = max(light.x - occluders, 0.0);
    
    return vec4(col, occluders, light.yz);
}

void main() {
    vec2 uv = gl_FragCoord.xy / resolution.xy;
    vec2 coord = uv;
    vec4 ic = BA(uv);
    vec2 lightpos = ic.zw;
    float occ = ic.x;
    float obj = ic.y;
    float dither = iib(gl_FragCoord.xy);
    vec2 dtc = (coord - lightpos) * (1.0 / float(SAMPLES) * DENSITY);
    float illumdecay = 1.0;
    
    for(int i = 0; i < SAMPLES; i++) {
        coord -= dtc;
        #ifdef DITHER
            float s = BA(coord + (dtc * dither)).x;
        #else
            float s = BA(coord).x;
        #endif
        s *= illumdecay * WEIGHT;
        occ += s;
        illumdecay *= DECAY;
    }
    
    // Aesthetic colors for Persona 5: Deep Red and Black
    vec3 rayColor = vec3(0.9, 0.0, 0.0); // Red rays
    vec3 bgColor = vec3(0.1, 0.05, 0.05); // Very dark reddish background
    
    vec3 finalColor = bgColor * obj + occ * EXPOSURE * rayColor;
    
    gl_FragColor = vec4(finalColor, 1.0);
}
