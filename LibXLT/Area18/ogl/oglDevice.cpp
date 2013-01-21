#include "Area18/ogl/oglDevice.hpp"

#include "Area18/ogl/oglBuffer.hpp"
#include "Area18/shdr/shdrShaders.hpp"
#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"

#include "oglTextDraw.h"

oglDevice::oglDevice(oglContext* context)
:	mContext(context)
{
}

oglDevice::~oglDevice(void)
{
}

oglBufferHandle oglDevice::CreateBuffer(GLenum i_Target, UINT i_Size,
            void* i_pInitialData)
{
	oglBuffer* b = new oglBuffer(this, i_Target, i_Size, i_pInitialData);
	return oglBufferHandle(b);
}


