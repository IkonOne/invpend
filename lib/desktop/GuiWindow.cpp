#include "GuiWindow.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace std;

void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void glfw_error_callback(int code, const char *description) {
    cerr << "GLFW ERROR [" << code << "]: " << description << '\n';
}


namespace iplib {
namespace gui {

GuiWindow::~GuiWindow() {
    if (_window != nullptr)
        Close();
}

GuiWindow::GuiWindow(int width, int height, const char *title)
    : _width(width), _height(height), _title(title)
{ }

bool GuiWindow::GetShouldClose() const {
    return glfwWindowShouldClose(_window);
}

void GuiWindow::Open() {
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit())
        throw new runtime_error("Failed to initialize glfw.");
    
#if __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    _window = glfwCreateWindow(_width, _height, _title, NULL, NULL);
    glfwMakeContextCurrent(_window);

    if (_window == nullptr)
        throw new runtime_error("Failed to create a glfw Window.");

    glfwSetKeyCallback(_window, glfw_key_callback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        throw new runtime_error("Failed to get glfw process address");

    glViewport(0, 0, _width, _height);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // ImGuiIO &io = ImGui::GetIO();
    ImGui::GetIO();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void GuiWindow::Close() {
    if (_window == nullptr)
        throw new runtime_error("There is no window currently open.");

    glfwTerminate();
    _window = nullptr;
}

void GuiWindow::BeginRender() {
    glfwPollEvents();

    glClearColor(0.08f, 0.08f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GuiWindow::EndRender() {
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(_window);
}

}   // gui
}   // iplib