#include <iostream>

#include <iplib/GuiWindow.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;
using namespace iplib;

gui::GuiWindow window(800, 600, "Pendulum Sim");

int main() {
    window.Open();

    while (!window.GetShouldClose()) {
        window.BeginRender();

        window.EndRender();
    }

    window.Close();
    return 0;
}