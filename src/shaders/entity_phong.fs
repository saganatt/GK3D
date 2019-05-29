// Calculates Phong colour at each fragment.
// Ambient, diffuse and specular terms.
// Uses interpolated position and normal values passed from vertex shader.

#version 450 core
layout(location = 0) out vec4 fragColor;

in vec4 pos;
in vec3 normal;
in vec2 texCoords;

uniform sampler2D texMap;

uniform mat4 model;
uniform mat4 view;
uniform mat4 inv_view;
uniform mat4 projection;

// Light parameters
#define MAX_LIGHTS 10
uniform int num_lights;
uniform struct Light {
    vec4 position;
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    float radius;
    float coneAngle;
    vec3 coneDirection;
} lights[MAX_LIGHTS];

uniform vec3 mtl_ambient;   // Ambient surface colour
uniform vec3 mtl_diffuse;   // Diffuse surface colour
uniform vec3 mtl_specular;  // Specular surface colour
uniform vec3 emission;      // Specular surface colour
uniform float shininess;

uniform float fog_density = 0.02;
uniform bool use_fog;

// Multiple lights code from
// http://www.tomdalling.com/blog/modern-opengl/08-even-more-lighting-directional-lights-spotlights-multiple-lights
vec3 ApplyLight(Light light, vec3 surfaceColor, vec3 normal, vec4 vertex_world, vec4 vertex_view) {
    vec3 light_surface_dir;
    float attenuation = 1.0;

    if(light.position.w == 0.0) {
        //directional light
        light_surface_dir = normalize(light.position.xyz);
        attenuation = 1.0; //no attenuation for directional lights
    } else {
        //point light
        light_surface_dir = normalize(vec3(light.position - vertex_world));
        float distanceToLight = length(vec3(light.position - vertex_world));
        attenuation = clamp(1.0 - distanceToLight*distanceToLight/(light.radius*light.radius), 0.0, 1.0);

        //cone restrictions (affects attenuation)
        float lightToSurfaceAngle = acos(dot(-light_surface_dir, normalize(light.coneDirection)));
        if(lightToSurfaceAngle > light.coneAngle){
            attenuation = 0.0;
        }
    }

    //ambient
    vec3 ambient = surfaceColor.rgb * light.ambient;

    //diffuse
    float sDotN = max(0.0, dot(normal, light_surface_dir));
    vec3 diffuse = sDotN * surfaceColor.rgb * light.diffuse;

    // Convert world space variables to view space
    vec3 normal_view = normalize(mat3(view) *normal);
    vec3 view_dir = normalize(-vertex_view.xyz);
    vec3 light_surface_view_dir = normalize(vec3(view * light.position - vertex_view));

    // Specular
    vec3 halfDir = normalize(light_surface_view_dir + view_dir);
    float specAngle = max(dot(halfDir, normal_view), 0.0);

    vec3 reflection = reflect(view_dir, normal_view);
    vec3 r_world = vec3(inv_view * vec4(reflection, 0.0));
    vec3 specular = mtl_diffuse * light.specular * pow(specAngle, shininess);

    return emission + ambient + attenuation*(diffuse + specular);
}

vec3 applyFog( in vec3  rgb,       // original color of the pixel
               in float distance ) // camera to point distance
{
    float fogAmount = 1.0 - exp( -distance*fog_density);
    vec3  fogColor  = vec3(0.5,0.6,0.7);
    return mix(rgb, fogColor, fogAmount);
}

void main(void) {
    vec4 vertex_view = view * pos;

    vec3 lit_colour = vec3(0);
    float texture_alpha = texture(texMap, texCoords).a;
    // Do not render or blend clear pixels
    if(texture_alpha < 0.1){
        discard;
    }

    for(int i = 0; i < num_lights; ++i){
        lit_colour += ApplyLight(lights[i], vec3(texture(texMap, texCoords)), normal, pos, vertex_view);
    }

    if(use_fog) lit_colour = applyFog(lit_colour,-vertex_view.z);
    fragColor = vec4(lit_colour, texture_alpha);
}