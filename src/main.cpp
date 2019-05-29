#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
 
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "utils/Model.h"
#include "utils/Loader.h"
#include "utils/GameTime.h"
#include "utils/InputState.h"
#include "utils/FrameBuffer.h"

#include "objects/Entity.h"
#include "objects/Player.h"
#include "objects/Light.h"
#include "objects/Terrain.h"
#include "objects/Camera.h"

#include "renderers/EntityRenderer.h"
#include "renderers/TerrainRenderer.h"
#include "renderers/SkyboxRenderer.h"

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
const float SKYBOX_SIZE = 200.0f;

// Data structure storing mouse input info
InputState input;

glm::mat4 projection;

// Player must be declared here so that keyboard callbacks can be sent
Player* player;

// Global things necessary for callback switching
// Cameras
enum CameraType {
    tracking, // Camera that shows driver perspective
    moving, // Moves past the car
    standing // Camera standing on the ground
};
CameraType cameraType = tracking;
Camera* trackingCamera;
Camera* movingCamera;
Camera* staticCamera;

//EntityRenderer* renderer;
//TerrainRenderer* terrainRenderer;
SkyboxRenderer* skyboxRenderer; // global to enable skybox switch in key callback
std::vector<std::string> daySkybox = std::vector<std::string>{
    "../res/textures/hw_sahara/sahara_left.tga",
    "../res/textures/hw_sahara/sahara_right.tga",
    "../res/textures/hw_sahara/sahara_top.tga",
    "../res/textures/hw_sahara/sahara_bottom.tga",
    "../res/textures/hw_sahara/sahara_front.tga",
    "../res/textures/hw_sahara/sahara_back.tga"
};
std::vector<std::string> nightSkybox = std::vector<std::string>{
        "../res/textures/ame_nebula/purplenebula_left.tga",
        "../res/textures/ame_nebula/purplenebula_right.tga",
        "../res/textures/ame_nebula/purplenebula_top.tga",
        "../res/textures/ame_nebula/purplenebula_bottom.tga",
        "../res/textures/ame_nebula/purplenebula_front.tga",
        "../res/textures/ame_nebula/purplenebula_back.tga"
};

Light* skyLight;
Light* headlight;
Light* sheriffLight;
float headlightPitch;
float headlightYaw;
std::vector<Light*> lights;

bool use_fog = true;
bool use_phong = true;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void error_callback(int error, const char* description);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_pos_callback(GLFWwindow* window, double x, double y);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void setProjection(int winX, int winY);
void renderScene(const std::vector<Entity*>& entities, const std::vector<Light*>& lights, Terrain* terrain,
                 SkyboxRenderer& skyboxRenderer, EntityRenderer& entityRenderer, TerrainRenderer& terrainRenderer,
                 const glm::mat4& projection);

void setHeadlightAngles(GLFWwindow* window, int key, int scancode, int action, int mods);
void updateHeadlightsDirections();
void createSunAndPushBack();
void createNightLightAndPushBack();
void createSheriffLightAndPushBack();

GLFWwindow* initWindow();

 int main()
 {
    GLFWwindow* window = initWindow();

    srand(time(NULL));

    // Create Terrain using blend map, height map and all of the remaining texture components.
    std::vector<std::string> terrainImages = {
            "../res/terrain/blendMap.png",
            "../res/terrain/dirt.png",
            "../res/terrain/sand.jpeg",
            "../res/terrain/mud.jpg"
    };
    Terrain* terrain = Terrain::loadTerrain(terrainImages,
            "../res/terrain/heightmap.png");
    // Moves the terrain model to be centered about the origin.
    terrain->setPosition(glm::vec3(-Terrain::TERRAIN_SIZE/2, 0.0f, -Terrain::TERRAIN_SIZE/2));
    TerrainRenderer* terrainRenderer = new TerrainRenderer();

    // Vector to hold all of the world entities.
    std::vector<Entity*> entities;

    Model playerModel = Loader::getLoader()->loadModel("../res/objects/mustang_shelby_gt500_1967/mustang.obj");
    Model wagonModel = Loader::getLoader()->loadModel("../res/objects/wild_west_wagon/wild_west_wagon.obj");
    Model barrelModel = Loader::getLoader()->loadModel("../res/objects/barrel/barrel.obj");
    Model horseModel = Loader::getLoader()->loadModel("../res/objects/horse/horse.obj");
    Model windmillModel = Loader::getLoader()->loadModel("../res/objects/wooden_windmill/windmill.obj");
    Model bisonCraniumModel = Loader::getLoader()->loadModel("../res/objects/baby_bison_cranium/bison_cranium.obj");
    Model woodenHouseModel = Loader::getLoader()->loadModel("../res/objects/wooden_house/wooden_house.obj");

    // Create the player object, scaling for the model, and setting its position in the world to somewhere interesting.
    player = new Player(&playerModel, terrain, true);
    player->setScale(glm::vec3(0.1f, 0.1f, 0.1f));
    player->setPosition(terrain->getPositionFromPixel(300, 400));
    entities.push_back(player);

    // Initialisation of camera, projection matrix
    setProjection(SCR_WIDTH, SCR_HEIGHT);
    trackingCamera = new TrackingCamera(player);
    movingCamera = new PlayerCamera(player);
    staticCamera = new StaticCamera(player->getPosition());

    // Lights
    createSunAndPushBack();

    headlight = new Light();
    headlight->position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    headlight->specular = glm::vec3(0.8f, 0.8f, 0.4f);
    headlight->diffuse = glm::vec3(0.8f, 0.8f, 0.4f);
    headlight->coneDirection = glm::vec3(0.0f, 0.0f, 0.0f);
    headlight->coneAngle = (float)M_PI/8.0f;
    headlight->radius = 5.0f;
    headlightYaw = (float)M_PI/2.0f;
    headlightPitch = 0.0f;
    lights.push_back(headlight);

    // Adds entities to random positions on the map
    const size_t RAND_ENTITIES = 500;
    for(size_t i = 0; i < RAND_ENTITIES; i++){
        Entity* ent;
        int selection = rand() % 6;
        switch(selection) {
            case 0:
                ent = new Entity(&wagonModel);
                ent->setScale(glm::vec3(0.1f, 0.1f, 0.1f));
                break;
            case 1:
                ent = new Entity(&barrelModel);
                ent->setScale(glm::vec3(0.1f, 0.1f, 0.1f));
                break;
            case 2:
                ent = new Entity(&windmillModel);
                ent->setScale(glm::vec3(0.005f, 0.005f, 0.005f));
                break;
            case 3:
                ent = new Entity(&horseModel);
                ent->setScale(glm::vec3(0.001f, 0.001f, 0.001f));
                break;
            case 4:
                ent = new Entity(&bisonCraniumModel);
                ent->setScale(glm::vec3(0.05f, 0.05f, 0.05f));
                break;
            case 5:
                ent = new Entity(&woodenHouseModel);
                ent->setScale(glm::vec3(0.005f, 0.005f, 0.005f));
                break;
        }
        ent->setPosition(terrain->getPositionFromPixel(rand() % 1024, rand() % 1024));
        entities.push_back(ent);
    }

     // Set of pre calculated barrel positions on corners of the track
     std::vector<int> barrelPositions = {
             263, 262, 226, 250, 209, 273,
             213, 299, 342, 717, 329, 734,
             326, 751, 354, 755, 372, 754,
             750, 400, 765, 396, 748, 381,
             828, 480, 842, 476, 854, 478,
             852, 500, 852, 521, 842, 547,
             772, 402
     };

     // Creates barrels from the positions and adds them.
     for(size_t i = 0; i < barrelPositions.size(); i+= 2){
         Entity* barrel = new Entity(&barrelModel);
         barrel->setPosition(terrain->getPositionFromPixel(barrelPositions[i], barrelPositions[i+1]));
         barrel->setScale(glm::vec3(0.1f, 0.1f, 0.1f));
         entities.push_back(barrel);
     }

    // Goes through each entity and aligns its bottom edge with the terrain at that position.
    for(size_t i = 0; i < entities.size(); i++){
        entities[i]->placeBottomEdge(terrain->getHeight(entities[i]->getPosition().x, entities[i]->getPosition().z));
    }

    skyboxRenderer = new SkyboxRenderer(daySkybox, SKYBOX_SIZE);
    EntityRenderer* entityRenderer = new EntityRenderer();

    while (!glfwWindowShouldClose(window)) {
        GameTime::getGameTime()->update();
        if(cameraType == tracking) {
            trackingCamera->update(input);
        }
        else if(cameraType == standing) {
            staticCamera->update(input);
        }
        else {
            movingCamera->update(input);
        }

        for(size_t i = 0; i < entities.size(); i++){
            entities[i]->update();
        }

        // Update the postion of the car headlights
        headlight->position = glm::vec4(player->getPosition(), 1.0f);
        if(sheriffLight) {
            sheriffLight->position = glm::vec4(player->getPosition(), 1.0f);
        }
        updateHeadlightsDirections();

        // Render entire scene
        renderScene(entities, lights, terrain, *skyboxRenderer, *entityRenderer, *terrainRenderer, projection);

        glFlush();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup program, delete all the dynamic entities.
    delete player;
    for(size_t i = 0; i < entities.size(); i++){
        delete entities[i];
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    setProjection(width, height);
    glViewport( 0, 0, width, height);
}

// Error callback for GLFW. Simply prints error message to stderr.
void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Terminate program if escape is pressed
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwTerminate();
        exit(0);
    }
    
    // Day / night switch
    if(key == GLFW_KEY_C && action == GLFW_PRESS) {
        createNightLightAndPushBack();
        use_fog = false;
        skyboxRenderer = new SkyboxRenderer(nightSkybox, SKYBOX_SIZE);
    }
    if(key == GLFW_KEY_Z && action == GLFW_PRESS) {
        createSunAndPushBack();
        use_fog = true;
        skyboxRenderer = new SkyboxRenderer(daySkybox, SKYBOX_SIZE);
    }
    if(key == GLFW_KEY_X && action == GLFW_PRESS) {
        skyboxRenderer->disable();
    }

    // Fog switch
    if(key == GLFW_KEY_F && action == GLFW_PRESS) {
        use_fog = !use_fog;
    }

    // Phong / Gouraud switch
    if(key == GLFW_KEY_P && action == GLFW_PRESS) {
        use_phong = !use_phong;
    }

    // Cameras switch
    if(key == GLFW_KEY_T && action == GLFW_PRESS) {
        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        cameraType = tracking;
    }
    if(key == GLFW_KEY_Y && action == GLFW_PRESS) {
        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        cameraType = moving;
    }
    if(key == GLFW_KEY_U && action == GLFW_PRESS) {
        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        cameraType = standing;
    }

    // Sheriff switch
    if(key == GLFW_KEY_Q && action == GLFW_PRESS) {
        if(!sheriffLight) {
            createSheriffLightAndPushBack();
        }
        else {
            lights.erase(remove(lights.begin(), lights.end(), sheriffLight));
            free(sheriffLight);
            sheriffLight = NULL; // otherwise getting SEGV
        }
    }

    player->handleKeyboardEvents(window, key, scancode, action, mods);
    setHeadlightAngles(window, key, scancode, action, mods);
}

void mouse_pos_callback(GLFWwindow* window, double x, double y) {
     input.update((float) x, (float) y);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
     input.updateScroll((float)xoffset, (float)yoffset);
     if(cameraType == standing)
         projection = glm::perspective(glm::radians(staticCamera->zoom),
                 float(SCR_WIDTH) / float(SCR_HEIGHT), 1.0f, 800.0f);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        input.rMousePressed = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        input.rMousePressed = false;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        input.lMousePressed = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        input.lMousePressed = false;
    }
}

void setProjection(int winX, int winY) {
    // float aspect = (float) winX / winY;
    // FOV angle is in radians
    projection = glm::perspective(M_PI/4.0, double(winX) / double(winY), 1.0, 800.0);
}

GLFWwindow* initWindow() {
     glfwInit();
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) exit(1);

     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "The Wild West", NULL, NULL);
    if (window == NULL) {
         std::cout << "Failed to create GLFW window" << std::endl;
         glfwTerminate();
        exit(1);
     }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(1);
    }

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return window;
}

void setHeadlightAngles(GLFWwindow* window, int key, int scancode, int action, int mods) {
    float angleDelta = 0.05f;
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_I) {
            headlightPitch += angleDelta;
        } else if (key == GLFW_KEY_K) {
            headlightPitch -= angleDelta;
        }
        if (key == GLFW_KEY_J) {
            headlightYaw -= angleDelta;
        } else if (key == GLFW_KEY_L) {
            headlightYaw += angleDelta;
        }
    }
    if (headlightPitch > (float) M_PI * 2.0f) {
        headlightPitch -= (float) M_PI * 2.0f;
    }
    if (headlightPitch < (float) M_PI * -2.0f) {
        headlightPitch += (float) M_PI * 2.0f;
    }
    if (headlightYaw > (float) M_PI * 2.0f) {
        headlightYaw -= (float) M_PI * 2.0f;
    }
    if (headlightYaw < (float) M_PI * -2.0f) {
        headlightYaw += (float) M_PI * 2.0f;
    }
}

void updateHeadlightsDirections() {
    float angle = headlightYaw - player->getRotationY();

    glm::vec3 front;
    front.x = cos(headlightPitch) * cos(angle);
    front.y = sin(headlightPitch);
    front.z = cos(headlightPitch) * sin(angle);

    headlight->coneDirection = glm::normalize(front);

    if(sheriffLight) {
        float sheriffLightYaw = (float) glfwGetTime() * 3.0f;// GameTime::getGameTime()->getTime();
        glm::vec3 sheriffDir;
        sheriffDir.x = cos(sheriffLightYaw);
        sheriffDir.y = 0.0f;
        sheriffDir.z = sin(sheriffLightYaw);

        sheriffLight->coneDirection = glm::normalize(sheriffDir);
    }
}

void createSunAndPushBack() {
    if(skyLight) {
        lights.erase(remove(lights.begin(), lights.end(), skyLight));
        free(skyLight);
        skyLight = NULL; // otherwise getting SEGV
    }
    skyLight = new Light();
    skyLight->position = glm::vec4(-1.25*SKYBOX_SIZE/10, 2.5*SKYBOX_SIZE/10, 3*SKYBOX_SIZE/10, 0.0f); // w = 0 - directional
    skyLight->specular = glm::vec3(1.0f, 1.0f, 1.0f);
    skyLight->diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    skyLight->ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    lights.push_back(skyLight);
}

void createNightLightAndPushBack() {
     if(skyLight) {
         lights.erase(remove(lights.begin(), lights.end(), skyLight));
         free(skyLight);
         skyLight = NULL; // otherwise getting SEGV
     }
    skyLight = new Light();
    skyLight->position = glm::vec4(-1.25*SKYBOX_SIZE/10, 2.5*SKYBOX_SIZE/10, 3*SKYBOX_SIZE/10, 0.0f); // w = 0 - directional
    skyLight->specular = glm::vec3(1.0f, 1.0f, 1.0f);
    skyLight->diffuse = glm::vec3(0.1f, 0.1f, 0.1f);
    skyLight->ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    lights.push_back(skyLight);
 }

void createSheriffLightAndPushBack() {
    sheriffLight = new Light();
    sheriffLight->position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    sheriffLight->specular = glm::vec3(1.0f, 1.0f, 1.0f);
    sheriffLight->diffuse = glm::vec3(0.0f, 0.0f, 1.0f);
    sheriffLight->coneDirection = glm::vec3(0.0f, 0.0f, 0.0f);
    sheriffLight->coneAngle = (float)M_PI/8.0f;
    sheriffLight->radius = 5.0f;
    lights.push_back(sheriffLight);
 }

void renderScene(const std::vector<Entity*>& entities, const std::vector<Light*>& lights, Terrain* terrain,
        SkyboxRenderer& skybox, EntityRenderer& renderer, TerrainRenderer& terrainRenderer,
        const glm::mat4& projection) {
    glDisable(GL_CLIP_DISTANCE0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    glm::mat4 view;
    if(cameraType == tracking){
        view = trackingCamera->getViewMtx();
    }
    else if(cameraType == moving) {
        view = movingCamera->getViewMtx();
    }
    else if(cameraType == standing) {
        view = staticCamera->getViewMtx();
    }

    skybox.render(view, projection);
    renderer.render(entities, lights, view, projection, use_fog, use_phong);
    terrainRenderer.render(terrain, lights, view, projection, use_fog);
 }