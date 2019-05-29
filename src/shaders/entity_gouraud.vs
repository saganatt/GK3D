// Goraud shading
// The vertex shader calculates all parameters.
// It passes to fragment shader calculated color.

#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 inv_view;
uniform mat4 projection;

out vec3 GoraudColor; // resulting color from lighting calculations
out vec2 texCoords;
out vec4 vertex_view;

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

// Modified multiple lights code from
// http://www.tomdalling.com/blog/modern-opengl/08-even-more-lighting-directional-lights-spotlights-multiple-lights
vec3 ApplyLight(Light light, vec3 normal, vec4 vertex_world, vec4 vertex_view) {
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
    vec3 ambient = light.ambient;

    //diffuse
    float sDotN = max(0.0, dot(normal, light_surface_dir));
    vec3 diffuse = sDotN * light.diffuse;

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

void main(void) {
    vec4 pos = model * vec4(aPos, 1.0);
    vec3 normal = normalize(mat3(model) * aNormal);     // not using inverse-transpose but still seems to work
    texCoords = vec2(aTexCoords.x, 1.0 - aTexCoords.y);
    gl_Position = projection * view * pos;

    vertex_view = view * pos;

    GoraudColor = vec3(0);

    for(int i = 0; i < num_lights; ++i){
        GoraudColor += ApplyLight(lights[i], normal, pos, vertex_view);
    }
}