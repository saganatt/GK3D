#ifndef CAMERA_H
#define CAMERA_H

#define _USE_MATH_DEFINES

#include <glad/glad.h>

#include <cmath>

#include "../utils/InputState.h"
#include "Player.h"

#include <glm/glm.hpp>

class Camera {
protected:
    glm::vec3 position;
    glm::vec3 focalPoint;
    float sensitivity;

    glm::mat4 viewMtx;
public:
    float zoom;

    Camera();

    const glm::mat4 getViewMtx() const;

    glm::vec3 getPosition();
    void setPosition(glm::vec3 position);

    virtual void update(InputState &input);

    void look(glm::vec3 at);
};


class PlayerCamera : public Camera {
private:
    Player* player;
    float distance;
    float pitch;
    float yaw;

    float RESET_SPEED = (float)M_PI/2;
public:
    PlayerCamera(Player* player);

    virtual void update(InputState &input);
};


class StaticCamera : public Camera {
private:
    float pitch;
    float yaw;
public:
    StaticCamera(glm::vec3 playerPosition);
    virtual void update(InputState &input);
};

class TrackingCamera : public Camera {
private:
    Player* player;
    float pitch;
    float yaw;

    float RESET_SPEED = (float)M_PI/2;
public:
    TrackingCamera(Player* player);
    virtual void update(InputState &input);
};

#endif