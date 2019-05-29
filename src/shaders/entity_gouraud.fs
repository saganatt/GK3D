// Goraud shader
// Applies pre-calculated light colour to an object and adds some fog

#version 450 core
layout(location = 0) out vec4 fragColor;

uniform sampler2D texMap;

in vec3 GoraudColor;
in vec2 texCoords;
in vec4 vertex_view;

uniform float fog_density = 0.02;
uniform bool use_fog;

vec3 applyFog( in vec3  rgb,       // original color of the pixel
               in float distance ) // camera to point distance
{
    float fogAmount = 1.0 - exp( -distance*fog_density);
    vec3  fogColor  = vec3(0.5,0.6,0.7);
    return mix(rgb, fogColor, fogAmount);
}

void main(void) {
    float texture_alpha = texture(texMap, texCoords).a;
    // Do not render or blend clear pixels
    if(texture_alpha < 0.1){
        discard;
    }

    vec3 lit_colour = GoraudColor * vec3(texture(texMap, texCoords));

    if(use_fog) lit_colour = applyFog(lit_colour, -vertex_view.z);

    fragColor = vec4(lit_colour, texture_alpha);
}

