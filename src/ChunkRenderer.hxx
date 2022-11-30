#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>

using namespace glm;

struct BlockVertex {
	vec3 pos;
	vec2 uv;
	float index;
	static void format(unsigned int vbo) {
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)(sizeof(vec3)));
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)(sizeof(vec3) + sizeof(vec2));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
	}
};

class Chunk;

class ChunkRenderer {
public:
	ChunkRenderer(Chunk* chunk) : chunk(chunk) {}
	virtual ~ChunkRenderer() {}
	virtual void Render() = 0;
	virtual void Update() = 0;
protected:
	Chunk* chunk;
};

template<typename T>
class BasicChunkRenderer : public ChunkRenderer {
public:
	BasicChunkRenderer(Chunk* chunk) : ChunkRenderer(chunk) {
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
		T::format(vbo);
	}
	void Render() {
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glDrawArrays(GL_TRIANGLES, )
	}

	void Update() {

	}
private:
	unsigned int vbo, vao, ebo;
	std::vector<T> cachedVertices;
};

class VertexPoolChunkRenderer {

};