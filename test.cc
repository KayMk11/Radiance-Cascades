#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <vector>

#include "window_manager.h"
#include "buffers.h"
#include "framebuffers.h"
#include "shader.h"

const int WIDTH = 800;
const int HEIGHT = 800;
const int NUM_CASCADES = 5;
uint8_t data[WIDTH * HEIGHT * 4] = {0};

std::vector<FrameBuffer *> cascadeFrameBuffers;

void generateFrameBuffer(uint8_t *data)
{
    cascadeFrameBuffers.push_back(new FrameBuffer(WIDTH, HEIGHT, true, data));
}

int main()
{
    WindowManager wm;
    wm.init(WIDTH, HEIGHT, "test");

    gladLoadGL();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float quadVertices[] = {// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
                            // positions   // texCoords
                            -1.0f, 1.0f, 0.0f, 1.0f,
                            -1.0f, -1.0f, 0.0f, 0.0f,
                            1.0f, -1.0f, 1.0f, 0.0f,

                            -1.0f, 1.0f, 0.0f, 1.0f,
                            1.0f, -1.0f, 1.0f, 0.0f,
                            1.0f, 1.0f, 1.0f, 1.0f};

    Shader sdfshader("./shaders/sdf.vs", "./shaders/sdf.fs");
    Shader rendershader("./shaders/sdf.vs", "./shaders/radiance.fs");

    double xPos, yPos;
    float prevMX = 0, prevMY = 0;

    VertexArray quadVAO;
    quadVAO.bind();
    VertexBuffer quadVBO(quadVertices, sizeof(quadVertices));
    quadVBO.setLayout(0, 2, 4, 0);
    quadVBO.setLayout(1, 2, 4, 2);
    quadVBO.bind();
    quadVAO.unbind();
    quadVBO.unbind();

    for (int i = 0; i < WIDTH * HEIGHT * 4; i += 4)
    {
        data[i + 3] = 255;
    }

    for (int i = 0; i < NUM_CASCADES; i++)
    {
        // generateTexture();
        if (i == 0)
            cascadeFrameBuffers.push_back(NULL);
        generateFrameBuffer(data);
    }

    FrameBuffer sdf1(WIDTH, HEIGHT, true);
    FrameBuffer sdf2(WIDTH, HEIGHT, true);
    std::vector<FrameBuffer> sdfFB;

    sdfFB.push_back(sdf1);
    sdfFB.push_back(sdf2);

    int ping = 0, pong = 1;

    while (wm.isWindowActive())
    {
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (glfwGetMouseButton(wm.window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            sdfFB[ping].bind();
            sdfshader.use();
            sdfshader.setInt("tex", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, sdfFB[pong].getTextureBuffer());
            // glActiveTexture(GL_TEXTURE1);
            // glBindTexture(GL_TEXTURE_2D, NULL);
            glfwGetCursorPos(wm.window, &xPos, &yPos);

            sdfshader.setVec2("linestart", prevMX, prevMY);
            sdfshader.setVec2("lineend", (float)xPos, HEIGHT - (float)yPos);
            std::cout << "MousePress" << xPos << " " << HEIGHT - yPos << std::endl;

            sdfshader.setVec3("color", 1, 1, 1);

            quadVAO.bind();
            glDrawArrays(GL_TRIANGLES, 0, 6);

            std::swap(ping, pong);
        }
        glfwGetCursorPos(wm.window, &xPos, &yPos);
        prevMX = (float)xPos;
        prevMY = HEIGHT - (float)yPos;

        for (int i = cascadeFrameBuffers.size() - 1; i--;)
        {
            rendershader.use();
            FrameBuffer *cascade = cascadeFrameBuffers[i];
            if (cascade)
                cascade->bind();
            else
                wm.bindDefaultFrameBuffer();

            // glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
            // glClear(GL_COLOR_BUFFER_BIT);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, sdfFB[ping].getTextureBuffer());
            if (i < cascadeFrameBuffers.size() - 1)
            {
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, cascadeFrameBuffers[i + 1]->getTextureBuffer());
            }
            else
            {
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            rendershader.setInt("sdfTex", 0);
            rendershader.setInt("lastCascade", 1);
            rendershader.setVec2("cascadeLevel", i + 1, NUM_CASCADES);

            quadVAO.bind();
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        wm.updateWindow();
    }
}