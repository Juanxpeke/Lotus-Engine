#include "application.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "util/opengl_entry.h"

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
    }
  }
  
  void Application::update(float deltaTime) {}
  void Application::render() {}
  void Application::renderGUI() {}

}