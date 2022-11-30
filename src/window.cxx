#include "window.hxx"
#include "GLFW/glfw3.h"


Window::Window(const std::string& title, const int width, const int height) :
    title(title), width(width), height(height) {

}
Window::~Window() {

}

void glfw_err(int code, const char* desc) {
    printf("[GLFW] [ERROR] - %d - %s\n", code, desc);
}

int Window::Init() {
    if(!glfwInit()) {
        return -1;
    }
    glfwSetErrorCallback(glfw_err);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    this->window = glfwCreateWindow(Width(), Height(), title.c_str(), NULL, NULL);
    if(!this->window) {
        glfwTerminate();
        return -1;
    }
    glfwSetWindowUserPointer(this->window, this);
    glfwMakeContextCurrent(this->window);
    if(gladLoadGL((GLADloadfunc)glfwGetProcAddress) < 0) {
        return -1;
    }
    printf("GL version: %s\n", glGetString(GL_VERSION));
    return 0;
}
void Window::Run(std::function<int(Window&)> runnable) {
    while(!glfwWindowShouldClose(this->window)) {
        glfwPollEvents();
        if(runnable(*this) < 0) {
            break;
        }
        glfwSwapBuffers(window);
    }
}