#ifndef TERRAIN_RENDERER_H
#define TERRAIN_RENDERER_H

#define _USE_MATH_DEFINES

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../src/shaders/TerrainShader.h"
#include "../objects/Light.h"
#include "../objects/Camera.h"
#include "../utils/Model.h"

#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

class TerrainRenderer {
private:
    TerrainShader shader;
public:
    TerrainRenderer();

    void render(Terrain* terrain, std::vector<Light*> lights, glm::mat4 view, glm::mat4 proj, bool use_fog);
};

#endif //TERRAIN_RENDERER_H