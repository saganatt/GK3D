#include "TerrainShader.h"

TerrainShader::TerrainShader(): ShaderProgram(TERRAIN_VERTEX_SHADER, TERRAIN_FRAGMENT_SHADER) {
    bindUniformLocations();
}

void TerrainShader::bindUniformLocations(){
    location_blendMap = glGetUniformLocation(shaderID, "blendMap");
    location_backMap = glGetUniformLocation(shaderID, "backMap");
    location_rMap = glGetUniformLocation(shaderID, "rMap");
    location_gMap = glGetUniformLocation(shaderID, "gMap");
    location_bMap = glGetUniformLocation(shaderID, "bMap");

    location_projection = glGetUniformLocation(shaderID, "projection");
    location_model = glGetUniformLocation(shaderID, "model");
    location_view = glGetUniformLocation(shaderID, "view");

    location_num_lights = glGetUniformLocation(shaderID, "num_lights");

    location_use_fog = glGetUniformLocation(shaderID, "use_fog");
}

void TerrainShader::loadLights(std::vector<Light*> lights){
    loadUniformValue(location_num_lights, int(lights.size()));
    for(size_t i = 0; i < lights.size(); i++){
        loadLight(lights[i], i);
    }
}

void TerrainShader::loadLight(Light* light, int i){
    loadLightUniform("position", i, light->position);
    loadLightUniform("specular", i, light->specular);
    loadLightUniform("diffuse", i, light->diffuse);
    loadLightUniform("ambient", i, light->ambient);
    loadLightUniform("radius", i, light->radius);
    loadLightUniform("coneAngle", i, light->coneAngle);
    loadLightUniform("coneDirection", i, light->coneDirection);
}

void TerrainShader::loadView(glm::mat4 view){
    loadUniformValue(location_view, view);
}

void TerrainShader::loadTerrain(Terrain* terrain){
    loadUniformValue(location_blendMap, 0);
    loadUniformValue(location_backMap, 1);
    loadUniformValue(location_rMap, 2);
    loadUniformValue(location_gMap, 3);
    loadUniformValue(location_bMap, 4);

    glm::mat4 model = terrain->getModelMatrix();
    loadUniformValue(location_model, model);
}

void TerrainShader::loadProjection(glm::mat4 proj){
    loadUniformValue(location_projection, proj);
}

void TerrainShader::loadUseFog(bool use_fog) {
    loadUniformValue(location_use_fog, use_fog);
}