#ifndef TERRAINSHADER_H
#define TERRAINSHADER_H

#define _USE_MATH_DEFINES

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../objects/Terrain.h"
#include "../objects/Light.h"
#include "../objects/Camera.h"
#include "../utils/Model.h"
#include "../utils/ShaderProgram.h"

#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

const std::string TERRAIN_VERTEX_SHADER = "../src/shaders/terrain.vs";
const std::string TERRAIN_FRAGMENT_SHADER = "../src/shaders/terrain.fs";

class TerrainShader : public ShaderProgram {
private:
    GLuint location_blendMap;
    GLuint location_backMap;
    GLuint location_rMap;
    GLuint location_gMap;
    GLuint location_bMap;

    GLuint location_projection;
    GLuint location_model;
    GLuint location_view;

    GLuint location_num_lights;

    GLuint location_use_fog;
public:
    TerrainShader();

    virtual void bindUniformLocations();

    void loadTerrain(Terrain* terrain);

    void loadLights(std::vector<Light*> lights);
    void loadLight(Light* light, int i);
    void loadView(glm::mat4 view);
    void loadProjection(glm::mat4 proj);
    void loadUseFog(bool use_fog);
};

#endif //TERRAINSHADER_H