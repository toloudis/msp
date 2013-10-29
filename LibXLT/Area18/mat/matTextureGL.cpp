/****************************************************************************\
**  matTextureGL.cpp
**
**      matTextureDX11.cpp is the windows implementation of the Terawatt
**	matPlainTexture.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Area18/mat/matTextureGL.hpp"

#include "Graphics/mat/matExceptionX.hpp"
//#include "Area18/g2d/g2dDX11GlobalWin.hpp"
//#include "Aea18/g2d/g2dImageDX11.hpp"

//--------------------------------------------------------------------
//	This constructor makes an "empty" surface.  The surface pointer
//	will be NULL, reflecting this.
//--------------------------------------------------------------------
matTextureGL::matTextureGL()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
matTextureGL::~matTextureGL()
{
}

//--------------------------------------------------------------------
//	UnloadSurface releases the DirectDraw Surface
//--------------------------------------------------------------------
//void matTextureGL::UnloadSurface()
//{
//	DBG_ERROR("not implemented");
//	if( m_Surface )
//	{
//		m_Surface->Release();
//		m_Surface = NULL;
//	}
//}

//--------------------------------------------------------------------
//	MakeTransparencyMap will build a transparency map for this texture
//	i_Bias specifies the range (0-i_Bias) that will be considered transparent
//--------------------------------------------------------------------
char * matTextureGL::MakeTransparencyMap(float i_Bias, const g2dPFD& i_PFD, int i_Width, int i_Height)
{
	DBG_ERROR("not implemented");
	return NULL;
}
