// Per-fragment Phong lighting.
// The vertex shader converts vertex position and normal in eye space.
// Passes these to the fragment shader for per-fragment Phong calculation.

#version 450
layout (location = 0) in vec3 a_vertex;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 vertex; // vertex position in world space
out vec3 normal; // the world space normal
out vec2 st;

void main(void) {
    vertex = model * vec4(a_vertex, 1.0);
    normal = normalize(mat3(model) * a_normal);     // not using inverse-transpose but still seems to work
    st = vec2(a_tex_coord.x, 1.0 - a_tex_coord.y);
    gl_Position = projection * view * vertex;
}