#include "Area18/ogl/oglBuffer.hpp"
#include "Area18/ogl/MemoryTracker.h"

oglBuffer::oglBuffer(oglDevice* i_pDevice,
		GLenum i_Target,
		size_t i_Size,
		void* i_pInitialData,
		MemoryTracker* memoryTracker)
:	mBuffer(0)
, mMemoryTracker(memoryTracker)
{
	if (mMemoryTracker) {
		mMemoryTracker->increment(mSize);
	}
	glGenBuffers(1, &mBuffer);
	glBindBuffer(i_Target, mBuffer);
	glBufferData(i_Target, i_Size, i_pInitialData, GL_STATIC_DRAW);

	mTarget = i_Target;
	mSize = i_Size;
}

oglBuffer::~oglBuffer(void)
{
	glDeleteBuffers(1, &mBuffer);
	if (mMemoryTracker) {
		mMemoryTracker->decrement(mSize);
	}
}
