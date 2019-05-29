#ifndef ENTITYSHADER_H
#define ENTITYSHADER_H

#define _USE_MATH_DEFINES

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../objects/Entity.h"
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

const static std::string ENTITY_PHONG_VERTEX_SHADER = "../src/shaders/entity_phong.vs";
const static std::string ENTITY_PHONG_FRAGMENT_SHADER = "../src/shaders/entity_phong.fs";
const static std::string ENTITY_GOURAUD_VERTEX_SHADER = "../src/shaders/entity_gouraud.vs";
const static std::string ENTITY_GOURAUD_FRAGMENT_SHADER = "../src/shaders/entity_gouraud.fs";

class EntityShader : public ShaderProgram {
private:
    GLuint location_texMap;
    GLuint location_cubeMap;

    GLuint location_projection;
    GLuint location_model;
    GLuint location_view;
    GLuint location_inv_view;

    GLuint location_num_lights;
    GLuint location_shininess;
    GLuint location_emission;

    GLuint location_mtl_ambient;
    GLuint location_mtl_diffuse;
    GLuint location_mtl_specular;

    GLuint location_use_fog;
public:
    EntityShader(std::string vertexShader, std::string fragmentShader);

    virtual void bindUniformLocations();

    void loadLights(std::vector<Light*> lights);
    void loadLight(Light* light, int i);
    void loadView(glm::mat4 view);
    void loadEntity(Entity* entity);
    void loadModelComponent(const ModelComponent& component);
    void loadProjection(glm::mat4 proj);
    void loadUseFog(bool use_fog);
};

#endif //ENTITYSHADER_H