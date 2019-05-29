#ifndef SKYBOX_RENDERER_H
#define SKYBOX_RENDERER_H

#define _USE_MATH_DEFINES

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../src/shaders/SkyboxShader.h"
#include "../objects/Light.h"
#include "../objects/Camera.h"
#include "../utils/Model.h"
#include "../utils/Loader.h"

#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

class SkyboxRenderer {
private:
    SkyboxShader shader;
    GLuint vao;
    GLuint texture;
    int indexCount;
    bool enabled;
public:
    /*
    Images should be in order:
        PosXFilename,
        NegXFilename,
        PosYFilename,
        NegYFilename,
        PosZFilename,
        NegZFilename
    */
    SkyboxRenderer(std::vector<std::string> images, const float SIZE);
    GLuint getSkyboxTexture();
    void render(glm::mat4 view, glm::mat4);
    void disable();
    void enable();
};

#endif //SKYBOX_RENDERER_H