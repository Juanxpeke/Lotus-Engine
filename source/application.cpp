#include "application.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "util/opengl_entry.h"
#include "util/profile.h"

namespace Lotus
{

  Application::Application(const std::string& applicationName, int windowWidth, int windowHeight) :
    name(applicationName),
    width(windowWidth),
    height(windowHeight)
  {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);

    if (window == nullptr)
    {
      glfwTerminate();
      LOTUS_ASSERT(false, "[Application Error] Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window);

    int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    if (!status)
    {
      glfwTerminate();
      LOTUS_ASSERT(false, "[Application Error] Failed to initialize Glad");
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    ImGuiStyle& style = ImGui::GetStyle();

    style.Colors[ImGuiCol_Button] = ImVec4(0.0f, 0.5f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.0f, 0.6f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 0.4f, 0.0f, 1.0f);

    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 0.5f, 0.0f, 1.0f);

    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.0f, 0.5f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f, 0.6f, 0.0f, 1.0f);

    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);

    glViewport(0, 0, width, height);
  }

  Application::~Application()
  {
    glfwDestroyWindow(window);
	  glfwTerminate();
  }

  void Application::run()
  {
    float lastFrame = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
      glfwPollEvents();

      float currentFrame = glfwGetTime();
      
      update(currentFrame - lastFrame);

      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      render();
      renderGUI();

      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      lastFrame = currentFrame;
      
      glfwSwapBuffers(window);

      LOTUS_PROFILE_END_FRAME();
    }
  }
  
  void Application::update(float deltaTime) {}
  void Application::render() {}
  void Application::renderGUI() {}

  void Application::disableVSync()
  {
    glfwSwapInterval(0);
  }

}