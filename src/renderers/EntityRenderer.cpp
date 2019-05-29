#include "EntityRenderer.h"

EntityRenderer::EntityRenderer():
    PhongShader(ENTITY_PHONG_VERTEX_SHADER, ENTITY_PHONG_FRAGMENT_SHADER),
    GouraudShader(ENTITY_GOURAUD_VERTEX_SHADER, ENTITY_GOURAUD_FRAGMENT_SHADER) {
}

void EntityRenderer::render(std::vector<Entity*> entities, std::vector<Light*> lights, glm::mat4 view,
        glm::mat4 proj, bool use_fog, bool use_phong){
    EntityShader shader = use_phong ? PhongShader : GouraudShader;

    shader.enable();
    shader.loadProjection(proj);
    shader.loadLights(lights);
    shader.loadView(view);

    shader.loadUseFog(use_fog);

    for(size_t i = 0; i < entities.size(); ++i){
        shader.loadEntity(entities[i]);
        if(entities[i]->getModel() != NULL){
            renderModel(entities[i]->getModel(), use_phong);
        }
    }

    shader.disable();
}

void EntityRenderer::renderModel(Model* model, bool use_phong){
    std::vector<ModelComponent>* components = model->getModelComponents();
    for(size_t i = 0; i < components->size(); ++i){
        ModelComponent current = components->at(i);

        if(use_phong) {
            PhongShader.loadModelComponent(current);
        }
        else {
            GouraudShader.loadModelComponent(current);
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, current.getTextureID());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glBindVertexArray(current.getVaoID());

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glDrawElements(GL_TRIANGLES, current.getIndexCount(), GL_UNSIGNED_INT, (void*)0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glBindVertexArray(0);
    }
}