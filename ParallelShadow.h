#ifndef PARALLEL_SHADOW_H
#define PARALLEL_SHADOW_H

#include <glad/glad.h>
#include <Texture.h>

class ParallelShadow {

public:

	const unsigned int active_texture_unit;

	int width;
	int height;

	ParallelShadow(int width = 1024, int height = 1024);
	~ParallelShadow();

	void Bind();
	void Unbind();

	unsigned int FBO() { return fbo; }
	unsigned int TID() { return tid; }
	
private:

	unsigned int fbo;
	unsigned int tid;

	void setup();
};

#endif