#pragma once

#include <string>
#include <functional>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

class Window {
public:
    Window(const std::string& title, const int width = 1280, const int height = 720);
    ~Window();
    int Init();
    void Run(std::function<int(Window&)> runnable);
    int Width() const {
        return width;
    }
    int Height() const {
        return height;
    }
    float Aspect() const;
private:
    std::string title;
    GLFWwindow* window;
    int width;
    int height;
};