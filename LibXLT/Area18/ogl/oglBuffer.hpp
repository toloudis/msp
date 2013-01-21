#pragma once

#include "Area18/ogl/oglTypes.hpp"

class oglDevice;
class MemoryTracker;

class oglBuffer 
{
public:
	oglBuffer(oglDevice* i_pDevice,
			GLenum i_Target,
			size_t i_Size,
            void* i_pInitialData,
            MemoryTracker* memTracker = NULL);
	virtual ~oglBuffer(void);

	virtual GLuint GetResource() {return mBuffer;}
	virtual GLuint GetBuffer() {return mBuffer;}

	size_t GetSize() {return mSize;}

private:
	GLenum mTarget;
	GLuint mBuffer;
	size_t mSize;
	MemoryTracker* mMemoryTracker;
};
