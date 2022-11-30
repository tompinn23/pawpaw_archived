#pragma once

#include <stdlib.h>
#include <vector>
#include <deque>

#include <glm/glm.hpp>
#include <glad/gl.h>

using namespace glm;

typedef unsigned int uint;

struct DAIC {
    DAIC(){}
    DAIC(uint c, uint iC, uint s, uint bV, uint* i, uint g) {
        cnt = c;
        instCnt = iC;
        start = s;
        baseVert = bV;
        baseInst = 0;
        index = i;
        group = g;
    }
    uint cnt;
    uint instCnt;
    uint start;
    uint baseVert;
    uint baseInst;
    uint* index = NULL;
    uint group;
    vec3 pos;
};

template<typename T>
class VertexPool {
private:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLuint indbo;

    T* start;
    std::deque<T*> free;

    size_t K = 0;
    size_t N = 0;
    size_t M = 0;
    size_t MAXSIZE = 0;

    std::vector<DAIC> indirect;
    std::vector<GLuint> indices;
    std::vector<int> groupInd;

    VertexPool() {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glGenBuffers(1, &indbo);
        T::format(vbo);
    }

public:
    VertexPool(int k, int n) : VertexPool() {
        reserve(k, n);
        index();
    }

    ~VertexPool() {
        for(size_t i = 0; i< indirect.size(); i++) {
            unsection(indirect[i].index);
        }
        glBindVertexArray(vao);
        glUnmapBuffer(vbo);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        glDeleteBuffers(1, &indbo);
        glDeleteVertexArrays(1, &vao);
    }

    void reserve(const int k, const int n) {
        K = k; N = n; M = n;
        // const GLbitfield flag = GL_MAP_WRITE_BIT |
        //                         GL_MAP_PERSISTENT_BIT |
        //                         GL_MAP_COHERENT_BIT;
    }
};