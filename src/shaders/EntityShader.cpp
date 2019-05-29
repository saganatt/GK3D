#include "EntityShader.h"

EntityShader::EntityShader(std::string vertexShader, std::string fragmentShader)
    : ShaderProgram(vertexShader, fragmentShader) {
    bindUniformLocations();
}

void EntityShader::bindUniformLocations(){
    // Setup named attributes
    glBindAttribLocation(shaderID, 0, "aPos");
    glBindAttribLocation(shaderID, 1, "aNormal");
    glBindAttribLocation(shaderID, 2, "aTexCoords");

    location_texMap = glGetUniformLocation(shaderID, "texMap");
    location_cubeMap = glGetUniformLocation(shaderID, "cubeMap");

    location_projection = glGetUniformLocation(shaderID, "projection");
    location_model = glGetUniformLocation(shaderID, "model");
    location_view = glGetUniformLocation(shaderID, "view");
    location_inv_view = glGetUniformLocation(shaderID, "inv_view");

    location_shininess = glGetUniformLocation(shaderID, "shininess");
    location_emission = glGetUniformLocation(shaderID, "emission");
    location_num_lights = glGetUniformLocation(shaderID, "num_lights");
    location_mtl_ambient = glGetUniformLocation(shaderID, "mtl_ambient");
    location_mtl_diffuse = glGetUniformLocation(shaderID, "mtl_diffuse");
    location_mtl_specular = glGetUniformLocation(shaderID, "mtl_specular");

    location_use_fog = glGetUniformLocation(shaderID, "use_fog");
}

void EntityShader::loadLights(std::vector<Light*> lights){
    loadUniformValue(location_num_lights, int(lights.size()));
    for(size_t i = 0; i < lights.size(); i++){
        loadLight(lights[i], i);
    }
}

void EntityShader::loadLight(Light* light, int i){
    loadLightUniform("position", i, light->position);
    loadLightUniform("specular", i, light->specular);
    loadLightUniform("diffuse", i, light->diffuse);
    loadLightUniform("ambient", i, light->ambient);
    loadLightUniform("radius", i, light->radius);
    loadLightUniform("coneAngle", i, light->coneAngle);
    loadLightUniform("coneDirection", i, light->coneDirection);
}

void EntityShader::loadView(glm::mat4 view){
    loadUniformValue(location_view, view);
    loadUniformValue(location_inv_view, glm::inverse(view));
}

void EntityShader::loadEntity(Entity* entity){
    loadUniformValue(location_texMap, 0);
    loadUniformValue(location_cubeMap, 1);
    glm::mat4 model = entity->getModelMatrix();

    loadUniformValue(location_model, model);
}

void EntityShader::loadModelComponent(const ModelComponent& component){
    loadUniformValue(location_mtl_ambient, component.getMaterial().ambient, 3);
    loadUniformValue(location_mtl_diffuse, component.getMaterial().diffuse, 3);
    loadUniformValue(location_mtl_specular, component.getMaterial().specular, 3);
    loadUniformValue(location_emission, component.getMaterial().emission, 3);
    loadUniformValue(location_shininess, component.getMaterial().shininess);
}

void EntityShader::loadProjection(glm::mat4 proj){
    loadUniformValue(location_projection, proj);
}

void EntityShader::loadUseFog(bool use_fog) {
    loadUniformValue(location_use_fog, use_fog);
}