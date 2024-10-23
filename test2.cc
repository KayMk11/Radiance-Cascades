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

int main()
{
    WindowManager wm;
    wm.init(WIDTH, HEIGHT, "test");

    gladLoadGL();

    float quadVertices[] = {// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
                            // positions   // texCoords
                            -1.0f, 1.0f, 0.0f, 1.0f,
                            -1.0f, -1.0f, 0.0f, 0.0f,
                            1.0f, -1.0f, 1.0f, 0.0f,

                            -1.0f, 1.0f, 0.0f, 1.0f,
                            1.0f, -1.0f, 1.0f, 0.0f,
                            1.0f, 1.0f, 1.0f, 1.0f};

    Shader sdfshader("./shaders/sdf.vs", "./shaders/sdf.fs");
    Shader rendershader("./shaders/framebuffer.vs", "./shaders/framebuffer.fs");

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
    FrameBuffer sdf1(WIDTH, HEIGHT, true);
    FrameBuffer sdf2(WIDTH, HEIGHT, true);
    rendershader.use();

    while (wm.isWindowActive())
    {
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (glfwGetMouseButton(wm.window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            sdf2.bind();
            sdfshader.use();
            sdfshader.setInt("tex", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, sdf1.getTextureBuffer());
            // glActiveTexture(GL_TEXTURE1);
            // glBindTexture(GL_TEXTURE_2D, NULL);
            glfwGetCursorPos(wm.window, &xPos, &yPos);

            sdfshader.setVec2("linestart", prevMX, prevMY);
            sdfshader.setVec2("lineend", (float)xPos, HEIGHT - (float)yPos);
            std::cout << "MousePress" << xPos << " " << HEIGHT - yPos << std::endl;

            sdfshader.setVec3("color", 1, 1, 1);

            quadVAO.bind();
            glDrawArrays(GL_TRIANGLES, 0, 6);

            std::swap(sdf1, sdf2);
        }
        glfwGetCursorPos(wm.window, &xPos, &yPos);
        prevMX = (float)xPos;
        prevMY = HEIGHT - (float)yPos;
        wm.bindDefaultFrameBuffer();
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        rendershader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sdf2.getTextureBuffer());
        rendershader.setInt("screenTexture", 0);
        quadVAO.bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);

        wm.updateWindow();
    }
}