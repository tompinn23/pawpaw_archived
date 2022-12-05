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
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)(sizeof(vec3) + sizeof(vec2)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
	}
};

class Chunk;

/**
 * ChunkRenderer interface
 * 
 * The interface for different chunk rendering methods currently handles static blocks.
*/
class ChunkRenderer {
public:
	/**
	 * Constructs a chunk renderer for the given chunk.
	 * @param chunk The owning chunk of this renderer.
	*/
	ChunkRenderer(Chunk* chunk) : chunk(chunk) {}
	virtual ~ChunkRenderer() {}
	/**
	 * Render function is called when the Chunk is to be rendered to the screen.
	*/
	virtual void Render() = 0;
	/**
	 * Update function is called to update the mesh of the given Chunk.
	*/
	virtual void Update() = 0;
protected:
	Chunk* chunk;
};

/**
 * BasicChunkRenderer is a ChunkRenderer implementation for OpenGL < 4.2 (MacOS :( )
*/
template<typename T>
class BasicChunkRenderer : public ChunkRenderer {
public:
	/**
	 * Constructs a BasicChunkRenderer 
	 * 
	 * @param chunk The owning Chunk
	*/
	BasicChunkRenderer(Chunk* chunk) : ChunkRenderer(chunk) {
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
		T::format(vbo);
	}
	/**
	 * Renders a chunk using a vbo for each chunk.
	*/
	void Render() {
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	}

	void Update() {
	}
private:
	unsigned int vbo, vao, ebo;
	std::vector<T> cachedVertices;
};

class VertexPoolChunkRenderer {

};