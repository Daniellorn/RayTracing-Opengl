#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>

#include "Shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);


int main()
{ 
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int width = 1280;
    int height = 720;


    GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    std::cout << "OpenGL Info: \n";
    std::cout << "	Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "	Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "	Version: " << glGetString(GL_VERSION) << std::endl;


    uint32_t frameBuffertextureID;
    glCreateTextures(GL_TEXTURE_2D, 1, &frameBuffertextureID);

    glTextureParameteri(frameBuffertextureID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(frameBuffertextureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureParameteri(frameBuffertextureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(frameBuffertextureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(frameBuffertextureID, 1, GL_RGBA32F, width, height);

    uint32_t framebufferID;
    glCreateFramebuffers(1, &framebufferID);

    glNamedFramebufferTexture(framebufferID, GL_COLOR_ATTACHMENT0, frameBuffertextureID, 0);

    auto fbostatus = glCheckNamedFramebufferStatus(framebufferID, GL_FRAMEBUFFER);
    if (fbostatus != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer error" << std::endl;
    }


    Shader ComputeShader("res/Shaders/ComputeShader.glsl");

    ComputeShader.Bind();
    ComputeShader.UnBind();

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glfwGetFramebufferSize(window, &width, &height);

        ComputeShader.Bind();
        glBindImageTexture(0, frameBuffertextureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        const uint32_t workGroupSizeX = 16;
        const uint32_t workGroupSizeY = 16;

        GLuint numGroupsX = (width + workGroupSizeX - 1) / workGroupSizeX;
        GLuint numGroupsY = (height + workGroupSizeY - 1) / workGroupSizeY;

        glDispatchCompute(numGroupsX, numGroupsY, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferID);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // default framebuffer (ekran)
        glBlitFramebuffer(0, 0, width, height,
            0, 0, width, height,
            GL_COLOR_BUFFER_BIT, GL_NEAREST);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    ComputeShader.UnBind();

    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}