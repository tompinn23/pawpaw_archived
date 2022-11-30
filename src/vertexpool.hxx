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
        const GLbitfield flag = GL_MAP_WRITE_BIT |
                                GL_MAP_PERSISTENT_BIT |
                                GL_MAP_COHERENT_BIT;
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferStorage(GL_ARRAY_BUFFER, N * K * sizeof(T), NULL, flag);
        start = (T*)glMapBufferRange(GL_ARRAY_BUFFER, 0, N * K * sizeof(T), flag);
        MAXSIZE = N * K;
        for (int i = 0; i < N; i++) {
            free.push_front(start + i * K);
        }
    }

    void clear() {
        while (!indirect.empty()) {
            unsection(indirect.back().index);
        }
        while (!free.empty())
            free.pop_back();
        for (size_t i = 0; i < N; i++) {
            free.push_front(start + i * K);
        }
        update();
    }

    uint* section(const int size, const int group = 0, vec3 pos = vec3(0)) {
        if (size == 0 || size > K) {
            printf("VertexPool Error: Insufficient Bucket Size\n");
            return NULL;
        }
        if (free.empty()) {
            printf("VertexPool Error: No buckets available\n");
            return NULL;
        }
        const int first = groupind[group];
        const int base = (free.back() - start);
        indirect.emplace_back(size, 1, first, base, new uint(indirect.size()), group);
        free.pop_back();
        indirect.back().pos = pos;
        return indirect.back().index;
    }

    void unsection(uint* index) {
        if (index == NULL) {
            printf("VertexPool Error: Can't unsection - index is NULL\n");
            return;
        }
        for (int k = indirect[*index].baseVert; k < K; k++) {
            (start + k)->~T();
        }
        free.push_front(start + indirect[*index].baseVert);
        swap(indirect[*index], indirect.back());
        indirect.pop_back();
        *indirect[*index].index = *index;
        delete index;
    }

    template<typename... Args>
    void fill(uint* ind, int k, Args&& ..args) {
        T* place = start + indirect[*ind].baseVert + k;
        if (size_t(place - start) > MAXSIZE) {
            printf("VertexPool Error: Out of bounds write\n");
        }
        try {
            new (place) T(std::forward<Args>(args)...);
        }
        catch (...) { throw; }
    }

    template<typename F, typename... Args>
    void mask(F function, Args&&... args) {
        if (indirect.empty()) return;
        M = 0;
        int J = indirect.size() - 1;
        while (M <= J) {

            while (function(indirect[M], args...) && M < J) M++;
            while (!function(indirect[J], args...) && M < J) J--;

            *indirect[M].index = J;
            *indirect[J].index = M;
            swap(indirect[M++], indirect[J--]);

        }
    }

    template<typename F, typename... Args>
    void order(F function, Args&&... args) {

        if (indirect.empty()) return;

        sort(indirect.begin(), indirect.begin() + M, [&](const DAIC& a, const DAIC& b) {
            return function(a, b, args...);
            });
        for (size_t i = 0; i < indirect.size(); i++)
            *indirect[i].index = i;

    }

    void resize(const uint* index, const int newsize) {

        if (index != NULL && *index < N)
            indirect[*index].cnt = newsize;

    }

    void index() {

        for (size_t j = 0; j < K; j++) {
            indices.push_back(j * 4 + 0);
            indices.push_back(j * 4 + 1);
            indices.push_back(j * 4 + 2);
            indices.push_back(j * 4 + 3);
            indices.push_back(j * 4 + 1);
            indices.push_back(j * 4 + 0);
        }

        for (size_t j = 0; j < K; j++) {
            indices.push_back(j * 4 + 0);
            indices.push_back(j * 4 + 2);
            indices.push_back(j * 4 + 1);
            indices.push_back(j * 4 + 1);
            indices.push_back(j * 4 + 3);
            indices.push_back(j * 4 + 0);
        }

        groupind.push_back(0);
        groupind.push_back(6 * K);
        groupind.push_back(0);
        groupind.push_back(6 * K);
        groupind.push_back(0);
        groupind.push_back(6 * K);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
    }

    void update() {

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indbo);
        glBufferData(GL_DRAW_INDIRECT_BUFFER, indirect.size() * sizeof(DAIC), &indirect[0], GL_DYNAMIC_DRAW);

    }

    /*
    ================================================================================
                          Synchronization and Rendering
    ================================================================================
    */

    void render(const GLenum mode = GL_TRIANGLES, size_t first = 0, size_t length = 0) {

        if (indirect.size() == 0)
            return;
        if (length > indirect.size())
            length = indirect.size();
        else if (length == 0)
            length = M;

        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indbo);

        glMultiDrawElementsIndirect(mode, GL_UNSIGNED_INT, (void*)(first * (sizeof(DAIC))), length, sizeof(DAIC));

    }
};