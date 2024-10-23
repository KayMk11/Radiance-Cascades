#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include "window_manager.h"
#include "buffers.h"
#include "framebuffers.h"
#include "shader.h"

int main()
{
    WindowManager wm;
    wm.init(800, 800, "test");

    gladLoadGL();

    float vertices1[] = {
        // positions
        0.0f, 0.5f, 0.0f,   // top right
        0.0f, -0.5f, 0.0f,  // bottom right
        -1.0f, -0.5f, 0.0f, // bottom left
        -1.0f, 0.5f, 0.0f,  // top left
    };

    unsigned int indices[] = {
        // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    Shader shader("./shaders/test.vs", "./shaders/test.fs");
    Shader fbshader("./shaders/framebuffer.vs", "./shaders/framebuffer.fs");

    VertexArray vao;
    vao.bind();
    VertexBuffer vbo(vertices1, sizeof(vertices1));
    vbo.setLayout(0, 3, 3, 0);
    IndexBuffer ebo(indices, sizeof(indices));
    vbo.bind();
    ebo.bind();

    vao.unbind();
    vbo.unbind();
    ebo.unbind();

    float quadVertices[] = {// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
                            // positions   // texCoords
                            -1.0f, 1.0f, 0.0f, 1.0f,
                            -1.0f, -1.0f, 0.0f, 0.0f,
                            1.0f, -1.0f, 1.0f, 0.0f,

                            -1.0f, 1.0f, 0.0f, 1.0f,
                            1.0f, -1.0f, 1.0f, 0.0f,
                            1.0f, 1.0f, 1.0f, 1.0f};
    VertexArray quadVAO;
    quadVAO.bind();
    VertexBuffer quadVBO(quadVertices, sizeof(quadVertices));
    quadVBO.setLayout(0, 2, 4, 0);
    quadVBO.setLayout(1, 2, 4, 2);
    quadVBO.bind();
    quadVAO.unbind();
    quadVBO.unbind();

    fbshader.use();
    fbshader.setInt("screenTexture", 0);

    FrameBuffer fbo(800, 800, true);

    while (wm.isWindowActive())
    {
        fbo.bind();
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        shader.use();
        vao.bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        fbo.unbind();
        glClear(GL_COLOR_BUFFER_BIT);

        fbshader.use();
        quadVAO.bind();
        glBindTexture(GL_TEXTURE_2D, fbo.getTextureBuffer());
        glDrawArrays(GL_TRIANGLES, 0, 6);
        wm.updateWindow();
    }
}