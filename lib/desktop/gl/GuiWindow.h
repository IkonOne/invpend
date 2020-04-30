#ifndef GUIWINDOW_H
#define GUIWINDOW_H

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace iplib {
namespace gui {

class GuiWindow {
  public:
    GuiWindow() = delete;
    ~GuiWindow();

    GuiWindow(int width, int height, const char *title);

    bool GetShouldClose() const;

    void Open();
    void Close();

    void BeginRender();
    void EndRender();

  private:
    const int _width;
    const int _height;
    const char *_title;
    GLFWwindow *_window;
};

} // gui
} // iplib

#endif // GUIWINDOW_H