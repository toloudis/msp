/****************************************************************************\
**  matTextureDX11.cpp
**
**      matTextureDX11.cpp is the windows implementation of the Terawatt
**	matPlainTexture.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/mat/matTextureDX11.hpp"

#include "Graphics/mat/matExceptionX.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/g2dImageDX11.hpp"

//--------------------------------------------------------------------
//	This constructor makes an "empty" surface.  The surface pointer
//	will be NULL, reflecting this.
//--------------------------------------------------------------------
matTextureDX11::matTextureDX11()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
matTextureDX11::~matTextureDX11()
{
}

//--------------------------------------------------------------------
//	UnloadSurface releases the DirectDraw Surface
//--------------------------------------------------------------------
//void matTextureDX11::UnloadSurface()
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
char * matTextureDX11::MakeTransparencyMap(float i_Bias, const g2dPFD& i_PFD, int i_Width, int i_Height)
{
	DBG_ERROR("not implemented");
	return NULL;
}
