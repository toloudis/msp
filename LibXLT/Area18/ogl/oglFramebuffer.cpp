#include "oglFramebuffer.h"


oglFramebuffer::oglFramebuffer(oglDevice* i_pDevice)
{
	glGenFramebuffers(1, &mBuffer);
	//glBindBuffer(i_Target, m_Buffer);
	//glBufferData(i_Target, i_Size, i_pInitialData, GL_STATIC_DRAW);
}


oglFramebuffer::~oglFramebuffer(void)
{
	glDeleteFramebuffers(1, &mBuffer);
}
