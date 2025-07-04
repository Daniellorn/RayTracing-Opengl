﻿#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <iostream>
#include <filesystem>

#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Scene.h"
#include "Timer.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

void ResetFrameIndex(uint32_t& frameIndex)
{
    frameIndex = 1;
}


int main()
{ 
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    int width = 1280;
    int height = 720;


    GLFWwindow* window = glfwCreateWindow(width, height, "RayTracing OpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;        // IF using Docking Branch
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    std::cout << "OpenGL Info: \n";
    std::cout << "	Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "	Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "	Version: " << glGetString(GL_VERSION) << std::endl;

    Camera camera(window, 45.0f, 0.1f, 100.0f, width, height);

    Texture pathTracingTexture = CreateTexture(width, height);
    Texture accumulationTexture = CreateTexture(width, height);
    uint32_t HDRTexID = CreateCubeMap(CUBE_MAP "lakeside_sunrise_4k.hdr");
    //uint32_t HDRTexID = CreateCubeMap(CUBE_MAP "passendorf_snow_4k.hdr");

    Framebuffer fb = CreateFramebuffer(pathTracingTexture);

    Shader ComputeShader(RESOURCE_PATH "ComputeShader.glsl");

    ComputeShader.Bind();
    ComputeShader.UnBind();

    Scene scene;

    Material emissiveMaterial;
    emissiveMaterial.albedo = glm::vec4{ 0.8f, 0.7f, 0.1f, 0.0f };
    emissiveMaterial.roughness = 1.0f;
    emissiveMaterial.EmissionColor = glm::vec4{ 0.8f, 0.7f, 0.1f, 0.0f };
    emissiveMaterial.EmissionPower = 2.0f;

    scene.AddObject(Sphere(glm::vec4{ 0.0f, 0.0f, 0.0f, 0.0f }, 1.0f, 0, static_cast<int>(Model::DIFFUSE)));
    scene.AddObject(Sphere(glm::vec4{ -6.0f, 0.0f, -6.0f, 0.0f }, 2.5f, 1, static_cast<int>(Model::DIFFUSE)));
    scene.AddObject(Sphere(glm::vec4{ -6.0f, 0.0f, 6.0f, 0.0f }, 2.5f, 3, static_cast<int>(Model::DIFFUSE)));
    scene.AddObject(Sphere(glm::vec4{ 0.0f, -102.5f, 0.0f, 0.0f }, 100.0f, 2, static_cast<int>(Model::DIFFUSE)));
    scene.AddObject(Sphere(glm::vec4{ 1.4f, -4.5f, -50.0f, 0.0f }, 40.0f, 4, static_cast<int>(Model::EMISSIVE)));

    scene.AddMaterial(Material(glm::vec4(1.0f, 0.0f, 1.0f, 0.0f), 1.0f, 0.0f, { 0.0f, 0.0f }, glm::vec4(1.0f, 0.0f, 1.0f, 0.0f), 0.0f));
    scene.AddMaterial(Material(glm::vec4(0.2f, 0.3f, 1.0f, 0.0f), 1.0f));
    scene.AddMaterial(Material(glm::vec4(0.7f, 0.7f, 0.6f, 0.0f), 0.1f));
    scene.AddMaterial(Material(glm::vec4(0.7f, 0.5f, 0.9f, 0.0f), 0.1f));
    scene.AddMaterial(emissiveMaterial);

    auto& spheres = scene.GetSpheres();
    auto& materials = scene.GetMaterials();


    uint32_t ubo[2];
    glGenBuffers(1, &ubo[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo[0]);
    glBufferData(GL_UNIFORM_BUFFER, spheres.size() * sizeof(Sphere), spheres.data(), GL_STREAM_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenBuffers(1, &ubo[1]);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo[1]);
    glBufferData(GL_UNIFORM_BUFFER, materials.size() * sizeof(Material), materials.data(), GL_STREAM_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo[1]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);


    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    float currentFrame = 0.0f;

    float lastRenderTime = 0.0f;

    int MAX_BOUNCE = 5;
    int MAX_SAMPLES = 10;
    float exposure = 0.1;
    bool accumulate = false;
    bool reset = false;

    uint32_t frameIndex = 1;

    while (!glfwWindowShouldClose(window))
    {
        Timer timer;

        //ZROB VIEWPORT (DOCKSPACE)

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Settings");
        ImGui::Text("Last render: %.3fms", lastRenderTime);
        ImGui::DragInt("Max bounce", &MAX_BOUNCE, 1, 1, 50);
        ImGui::DragInt("Max samples", &MAX_SAMPLES, 1, 1, 100);
        ImGui::Checkbox("Accumulate", &accumulate);
        ImGui::DragFloat("Exposure HDR", &exposure, 0.1f, 0.0f, 1.0f);
        if (ImGui::Button("Reset"))
        {
            reset = true;
            ResetFrameIndex(frameIndex);
        };
        ImGui::End();

        ImGui::Begin("Spheres");

        for (size_t i = 0; i < spheres.size(); i++)
        {
            ImGui::PushID(i);

            Sphere& sphere = spheres[i];
            ImGui::DragFloat3("Position", glm::value_ptr(sphere.position), 0.1f);
            ImGui::DragFloat("Radius", &sphere.radius, 0.1f);

            ImGui::Separator();

            ImGui::PopID();
        }
        ImGui::End();

        ImGui::Begin("Materials");
        for (size_t i = 0; i < materials.size(); i++)
        {
            ImGui::PushID(i);

            Material& material = materials[i];
            ImGui::ColorEdit3("Albedo", glm::value_ptr(material.albedo));
            ImGui::DragFloat("Roughness", &material.roughness, 0.05f, 0.0f, 1.0f);
            ImGui::DragFloat("Metallic", &material.metallic, 0.05f, 0.0f, 1.0f);
            ImGui::ColorEdit3("Emission Color", glm::value_ptr(material.EmissionColor));
            ImGui::DragFloat("Emission Power", &material.EmissionPower, 0.05f, 0.0f, FLT_MAX);

            ImGui::Separator();

            ImGui::PopID();
        }

        ImGui::End();


        if (width != pathTracingTexture.width || height != pathTracingTexture.height)
        {
            glDeleteTextures(1, &pathTracingTexture.textureID);
            glDeleteTextures(1, &accumulationTexture.textureID);
            pathTracingTexture = CreateTexture(width, height);
            accumulationTexture = CreateTexture(width, height);

            if (!AttachTextureToFramebuffer(fb, pathTracingTexture))
            {
                std::cerr << "Resize failed\n";
                return -1;
            }

            camera.OnResize(width, height);
        }


        glBindBuffer(GL_UNIFORM_BUFFER, ubo[0]);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, spheres.size() * sizeof(Sphere), spheres.data());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBuffer(GL_UNIFORM_BUFFER, ubo[1]);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, materials.size() * sizeof(Material), materials.data());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

/*#ifdef _DEBUG
        ImGui::ShowDemoWindow();
#endif
*/
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glfwGetFramebufferSize(window, &width, &height);

        ComputeShader.Bind();
        glBindImageTexture(0, fb.frameBufferTex.textureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        glBindImageTexture(1, accumulationTexture.textureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        glBindImageTexture(2, HDRTexID, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

        if (accumulate)
        {
            frameIndex++;
        }

        bool cameraMoved = camera.OnUpdate(deltaTime);

        static glm::mat4 lastView = glm::mat4(1.0f);
        lastView = camera.GetView();

        const auto& cameraPosition = camera.GetPosition();
        const auto& inverseProjection = camera.GetInverseProjection();
        const auto& inverseView = camera.GetInverseView();

        ComputeShader.SetUniform3f("u_CameraPosition", cameraPosition.x, cameraPosition.y, cameraPosition.z);
        ComputeShader.SetUniformMat4fm("u_InverseProjection", inverseProjection);
        ComputeShader.SetUniformMat4fm("u_InverseView", inverseView);
        ComputeShader.SetUniform1i("u_NumOfSpheres", spheres.size());
        ComputeShader.SetUniform1i("u_MAX_BOUNCE", MAX_BOUNCE);
        ComputeShader.SetUniform1i("u_MAX_SAMPLES", MAX_SAMPLES);
        ComputeShader.SetUniform1i("u_SETTINGS", accumulate ? 1 : 0);
        ComputeShader.SetUniform1i("u_FrameIndex", frameIndex);
        ComputeShader.SetUniform1f("u_Exposure", exposure);

        const uint32_t workGroupSizeX = 16;
        const uint32_t workGroupSizeY = 16;

        GLuint numGroupsX = (width + workGroupSizeX - 1) / workGroupSizeX;
        GLuint numGroupsY = (height + workGroupSizeY - 1) / workGroupSizeY;

        glDispatchCompute(numGroupsX, numGroupsY, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        BlitFrambuffer(fb);


        if ((cameraMoved || reset) && accumulate)
        {
            ResetFrameIndex(frameIndex);
            std::vector<glm::vec4> clearColor(width * height, glm::vec4(0.0f));
            glTextureSubImage2D(accumulationTexture.textureID, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT, clearColor.data());
        }
      

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        lastRenderTime = timer.Elapsed();
        reset = false;
    }


    ComputeShader.UnBind();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

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