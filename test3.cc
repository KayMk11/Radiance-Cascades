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

    Shader sdfshader("./shaders/sdf.vs", "./shaders/sdf2.fs");
    Shader rendershader("./shaders/sdf.vs", "./shaders/radiance2.fs");

    double xPos, yPos;

    VertexArray quadVAO;
    quadVAO.bind();
    VertexBuffer quadVBO(quadVertices, sizeof(quadVertices));
    quadVBO.setLayout(0, 2, 4, 0);
    quadVBO.setLayout(1, 2, 4, 2);
    quadVBO.bind();
    quadVAO.unbind();
    quadVBO.unbind();
    FrameBuffer sdf(WIDTH, HEIGHT, true);

    while (wm.isWindowActive())
    {
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        sdf.bind();
        sdfshader.use();

        glfwGetCursorPos(wm.window, &xPos, &yPos);
        sdfshader.setVec2("center", ((float)xPos / HEIGHT), (HEIGHT - (float)yPos) / HEIGHT);
        sdfshader.setFloat("radius", .2);
        sdfshader.setVec3("color", 1, 1, 1);
        // std::cout << "MousePos" << xPos << " " << HEIGHT - yPos << std::endl;

        quadVAO.bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);

        wm.bindDefaultFrameBuffer();
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        rendershader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sdf.getTextureBuffer());
        rendershader.setInt("screenTexture", 0);
        quadVAO.bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);

        wm.updateWindow();
    }
}