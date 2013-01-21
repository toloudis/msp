#pragma once

#include "Area18/ogl/oglTypes.hpp"

class oglDevice;

class oglFramebuffer
{
public:
	oglFramebuffer(oglDevice* i_pDevice);
	virtual ~oglFramebuffer(void);

	virtual GLuint id() {return mBuffer;}

private:
	GLuint mBuffer;
};

