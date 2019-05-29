#ifndef ENTITY_RENDERER_H
#define ENTITY_RENDERER_H

#define _USE_MATH_DEFINES

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../src/shaders/EntityShader.h"
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

class EntityRenderer {
private:
    EntityShader PhongShader;
    EntityShader GouraudShader;
public:
    EntityRenderer();

    void render(std::vector<Entity*> entities, std::vector<Light*> lights, glm::mat4 view, glm::mat4 proj,
            bool use_fog, bool use_phong);
    void renderModel(Model* model, bool use_phong);
};

#endif //ENTITY_RENDERER_H