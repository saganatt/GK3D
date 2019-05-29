#include "Camera.h"

#include <stdio.h>
#include <iostream>
#include <algorithm>

#include "glm/gtc/matrix_transform.hpp"

#define DEG2RAD(x) ((x)*M_PI/180.0)
#define RAD2DEG(x) ((x)*180.0/M_PI)

template <typename T>
T clamp(const T& n, const T& lower, const T& upper) {
    return std::max(lower, std::min(n, upper));
}

Camera::Camera()
        : position(glm::vec3(0.0f, 10.0f, 0.0f))
        , focalPoint(glm::vec3(0.0f))
        , viewMtx(glm::mat4(1.0f))
        , sensitivity(0.01f)
        , zoom(45.0f)
{ }

const glm::mat4 Camera::getViewMtx() const {
    return viewMtx;
}

glm::vec3 Camera::getPosition(){
    return position;
}
void Camera::setPosition(glm::vec3 position){
    this->position = position;
}
void Camera::update(InputState &input){
    // Input has no effect in the base object.
}

void Camera::look(glm::vec3 at){
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    viewMtx = glm::lookAt(position, at, up);
}

PlayerCamera::PlayerCamera(Player* player) : Camera(){
    this->player = player;
    this->distance = 5.0f;
    this->pitch = (float)M_PI/8;
    this->yaw = 0.0f;
}

void PlayerCamera::update(InputState &input){
    distance -= input.scrollDeltaY;
    distance = clamp(distance, 2.5f, 20.0f);

    if(input.lMousePressed){
        pitch -= input.deltaY * sensitivity;
        pitch = clamp(pitch, 0.1f, (float)M_PI / 2.0f - 0.0001f);

        yaw -= input.deltaX * sensitivity;

        if(yaw > (float)2*M_PI){
            yaw -= (float)2*M_PI;
        }
        if(yaw < (float)-2*M_PI){
            yaw += (float)2*M_PI;
        }
    }
    input.scrollDeltaY = 0.0;
    input.deltaY = 0;
    input.deltaX = 0;

    float change = RESET_SPEED * GameTime::getGameTime()->getDt();

    if(player->getThrottle() > 0.1f){
        if(fabs(yaw) < change){
            yaw = 0.0f;
        }else if(yaw > 0){
            yaw -= change;
        } else if (yaw < 0){
            yaw += change;
        }
    }

    float angle = yaw + player->getRotationY();

    float hDist = distance * glm::cos(pitch);
    float vDist = distance * glm::sin(pitch);
    float offsetX = hDist * glm::sin(angle);
    float offsetZ = hDist * glm::cos(angle);

    this->focalPoint = player->getPosition();
    this->position = glm::vec3(-offsetX, vDist, -offsetZ) + this->focalPoint;

    look(this->focalPoint);
}

StaticCamera::StaticCamera(glm::vec3 playerPosition) : Camera() {
    this->yaw = -(float)M_PI / 4.0f;
    this->pitch = 0.0f;
    this->position = playerPosition + glm::vec3(-20.0f, 10.0f, 20.0f);
}

void StaticCamera::update(InputState &input) {
    zoom -= input.scrollDeltaY;
    zoom = clamp(zoom, 1.0f, 45.0f);

    pitch -= input.deltaY * sensitivity;
    pitch = clamp(pitch, 0.0001f - (float)M_PI / 2.0f, (float)M_PI / 2.0f - 0.0001f);

    yaw += input.deltaX * sensitivity;

    if(yaw > (float)2*M_PI){
        yaw -= (float)2*M_PI;
    }
    if(yaw < (float)-2*M_PI){
        yaw += (float)2*M_PI;
    }

    input.scrollDeltaY = 0.0;
    input.deltaY = 0;
    input.deltaX = 0;

    glm::vec3 front;
    front.x = cos(pitch) * cos(yaw);
    front.y = sin(pitch);
    front.z = cos(pitch) * sin(yaw);
    front = glm::normalize(front);

    this->focalPoint = this->position + front;
    look(this->focalPoint);
}

TrackingCamera::TrackingCamera(Player* player) : Camera(){
    this->player = player;
    this->pitch = 0.0f;
    this->yaw = 0.0f;
}

void TrackingCamera::update(InputState &input) {
    pitch -= input.deltaY * sensitivity;
    pitch = clamp(pitch, 0.0001f - (float)M_PI / 2.0f, (float)M_PI / 2.0f - 0.0001f);

    yaw += input.deltaX * sensitivity;

    if(yaw > (float)2*M_PI){
        yaw -= (float)2*M_PI;
    }
    if(yaw < (float)-2*M_PI){
        yaw += (float)2*M_PI;
    }

    input.scrollDeltaY = 0.0;
    input.deltaY = 0;
    input.deltaX = 0;

    float angle = yaw - player->getRotationY();

    glm::vec3 front;
    front.x = cos(pitch) * cos(angle);
    front.y = sin(pitch);
    front.z = cos(pitch) * sin(angle);
    front = glm::normalize(front);

    this->position = player->getPosition() + glm::vec3(0.0f, 1.0f, 0.0f);

    this->focalPoint = this->position + front;
    look(this->focalPoint);
}
