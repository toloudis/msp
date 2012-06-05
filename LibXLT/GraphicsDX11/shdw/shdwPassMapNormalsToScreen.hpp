/****************************************************************************\
**	shdwPassMapNormalsToScreen.hpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef SHDW_PASSMAPNORMALSTOSCREEN_HPP
#error shdwPassMapNormalsToScreen.hpp multiply included
#endif
#define SHDW_PASSMAPNORMALSTOSCREEN_HPP

#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif

class matTexture;
class g2dRenderTarget;

class shdwPassMapNormalsToScreen
{
public:
	shdwPassMapNormalsToScreen(matTexture* i_pNormals, g2dRenderTarget* i_pScreen);
	~shdwPassMapNormalsToScreen();

	virtual int Render(float i_Time);

private:
	matTexture* m_pNormals;
	g2dRenderTarget* m_pDestination;

	ID3D11SamplerState* m_pSampleStatePoint;
};