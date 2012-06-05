/****************************************************************************\
**  matTextureDX11.hpp
**
**      matTextureDX11.hpp is the windows implementation of the Terawatt
**	matTexture.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_TEXTUREDX11_HPP
#error matTextureDX11.hpp multiply included
#endif
#define MAT_TEXTUREDX11_HPP

#ifndef MAT_TEXTURE_HPP
#include "Graphics/mat/matTexture.hpp"
#endif

#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif

class matTextureDX11 : public matTexture
{
public:

	//--------------------------------------------------------------------
	//	This constructor makes an "empty" surface.  The surface pointer
	//	will be NULL, reflecting this.
	//--------------------------------------------------------------------
	matTextureDX11();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~matTextureDX11();

	//--------------------------------------------------------------------
	//	GetSurface returns a pointer usable for shaders.
	//--------------------------------------------------------------------
	virtual ID3D11ShaderResourceView* GetSurface() const = 0;
	virtual g2dD3D11ResourcePtr GetResource() const = 0;

	//--------------------------------------------------------------------
	//	UnloadSurface releases the DirectDraw Surface
	//--------------------------------------------------------------------
	// void UnloadSurface();

	//--------------------------------------------------------------------
	//	MakeTransparencyMap will build a transparency map for this texture
	//	i_Bias specifies the range (0-i_Bias) that will be considered transparent
	//--------------------------------------------------------------------
	char *MakeTransparencyMap(float i_Bias, const g2dPFD& i_PFD, int i_Width, int i_Height);

private:
	char *m_TransparencyMap;
};



