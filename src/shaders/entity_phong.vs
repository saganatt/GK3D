// Per-fragment Phong lighting.
// The vertex shader converts vertex position and normal in eye space.
// Passes these to the fragment shader for per-fragment Phong calculation.

#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec4 pos; // vertex position in world space
out vec3 normal; // the world space normal
out vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    pos = model * vec4(aPos, 1.0);
    normal = normalize(mat3(model) * aNormal);     // not using inverse-transpose but still seems to work
    texCoords = vec2(aTexCoords.x, 1.0 - aTexCoords.y);
    gl_Position = projection * view * pos;
}