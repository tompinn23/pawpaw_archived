#include "window.hxx"


const std::string vertex = R"(
#version 410
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTexCoord;
layout (location = 2) in float vTexIndex;

out vec3 TexCoord;

uniform mat4 mvp;

void main() {
    gl_Position = mvp * vec4(vPos, 1.0);
    TexCoord = vec3(vTexCoord, vTexIndex);
}
)";


int main(int argc, char** argv) {
    auto window = Window("Hello World.");
    if(window.Init() < 0) {
        return -1;
    }
    window.Run([](Window& self) {
        glClearColor(0.2, 0.6, 0.7, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        return 0;
    });
    return 0;
}