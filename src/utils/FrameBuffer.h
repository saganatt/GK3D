#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#define _USE_MATH_DEFINES

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <GL/glext.h>

#include "../objects/Light.h"
#include "../objects/Player.h"

#include <glm/glm.hpp>
#include <vector>
#include <cfloat>
#include <utility>
#include <algorithm>
#include <iostream>

class FrameBuffer {
private:
    GLuint framebufferID;

    GLuint depthTexture;
    GLuint depthBuffer;
    GLuint colourTexture;

    GLuint width;
    GLuint height;
public:
    FrameBuffer(int width, int height);
    void addColourTexture();
    void addDepthTexture();
    void addDepthBuffer();
    bool isOkay();

    GLuint getColourTexture();
    GLuint getDepthTexture();
    GLuint getDepthBuffer();

    int getWidth();
    int getHeight();

    virtual void bind();
    virtual void unbind();
};

#endif