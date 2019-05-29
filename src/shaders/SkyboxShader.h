#ifndef SKYBOXSHADER_H
#define SKYBOXSHADER_H

#define _USE_MATH_DEFINES

#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

const std::string SKYBOX_VERTEX_SHADER = "../src/shaders/skybox.vs";
const std::string SKYBOX_FRAGMENT_SHADER = "../src/shaders/skybox.fs";

class SkyboxShader : public ShaderProgram {
private:
    GLuint location_projection;
    GLuint location_view;

public:
    SkyboxShader();

    virtual void bindUniformLocations();
    void loadMatrices(glm::mat4 camera, glm::mat4 projection);
};

#endif //SKYBOXSHADER_H