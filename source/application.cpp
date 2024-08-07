#include "application.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <stb_image.h>
#include "util/opengl_entry.h"
#include "util/profile.h"
#include "util/path_manager.h"

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

    vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    device = reinterpret_cast<const char*>(glGetString(GL_RENDERER));

    GLFWimage icons[1];
    std::string iconPath = assetPath("icons/lotus_engine_icon.png").string();
    icons[0].pixels = stbi_load(iconPath.c_str(), &icons[0].width, &icons[0].height, 0, 4);
    glfwSetWindowIcon(window, 1, icons);
    stbi_image_free(icons[0].pixels);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    ImGuiStyle& style = ImGui::GetStyle();

    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.5f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.04f, 0.04f, 0.04f, 1.0f);

    style.Colors[ImGuiCol_Button] = ImVec4(0.0f, 0.5f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.0f, 0.6f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 0.4f, 0.0f, 1.0f);

    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 0.5f, 0.0f, 1.0f);

    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.0f, 0.5f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f, 0.6f, 0.0f, 1.0f);

    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);

    style.Colors[ImGuiCol_Tab] = ImVec4(0.0f, 0.4f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.0f, 0.6f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.0f, 0.5f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.0f, 0.3f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.0f, 0.4f, 0.0f, 1.0f);

    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.0f, 0.5f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.0f, 0.4f, 0.0f, 1.0f);

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

  void Application::setBackgroundColor(const glm::vec3& color)
  {
    glClearColor(color.r, color.g, color.b, 1.0f);
  }

  void Application::disableVSync()
  {
    glfwSwapInterval(0);
  }

}