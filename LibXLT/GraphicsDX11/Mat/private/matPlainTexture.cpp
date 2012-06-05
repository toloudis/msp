/****************************************************************************\
**  matPlainTexture.cpp
**
**      matPlainTexture is a matTexture which represents an ordinary
**	rectangular texture with no special ability.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/mat/matPlainTexture.hpp"

#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/mat/matExceptionX.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/g2dTextureDX11.hpp"

//--------------------------------------------------------------------
//	This constructor makes an "empty" surface
//--------------------------------------------------------------------
matPlainTexture::matPlainTexture()
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
matPlainTexture::~matPlainTexture()
{
}

//--------------------------------------------------------------------
//	ReloadInfo causes the matPlainTexture to regenerate its
//	parameters from the PAC.  Again, this function should only be
//	called by the Terawatt PAC components.
//--------------------------------------------------------------------
void matPlainTexture::ReloadInfo()
{
	D3D11_TEXTURE2D_DESC desc;
	m_Texture->GetResource()->GetDesc(&desc);

	this->SetWidth(desc.Width);
	this->SetHeight(desc.Height);
	g2dPFD pfd;
	g2dDX11Global::PFDFromD3DFormat(desc.Format, pfd);
	this->SetPixelFormat(pfd);
}

//--------------------------------------------------------------------
//	Make makes the surface into one with the given dimensions and
//	pixel format.  If the pixel format is not supported it will
//	throw a matUnsupportedPixelFormatX, and leave the old surface
//	intact.
//--------------------------------------------------------------------
void matPlainTexture::Make(int i_Width, int i_Height, const g2dPFD& i_PFD, 
						   void* i_PixelData /*= NULL*/)
{
	if (g2dDX11Global::RestrictPow2Textures())
	{
		if (!((i_Width & (i_Width - 1)) == 0) || !((i_Height & (i_Height - 1)) == 0))
		{
			throw matInvalidTextureSizeX(fsLocator());
		}
	}

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = i_Width;
	desc.Height = i_Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = g2dDX11Global::D3DFormatFromPFD(i_PFD);
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0; // no cpu access
	desc.MiscFlags = 0;

	// big assumption: initial data is packed tightly and pitch
	// is related directly to pixel size for this format
	D3D11_SUBRESOURCE_DATA initialData;
	initialData.pSysMem = i_PixelData;
	initialData.SysMemPitch = i_PFD.BitsPerPixel()*i_Width/8;
	initialData.SysMemSlicePitch = 0;

	g2dD3D11TexturePtr new_texture = NULL;
	HRESULT op_result = g2dDX11Global::g_pDevice->CreateTexture2D(&desc, i_PixelData ? &initialData : NULL, &new_texture);
	if( !SUCCEEDED(op_result) )
	{
		g2dDX11Global::PrintDXError(op_result);
		if ( E_OUTOFMEMORY == op_result )
			throw g2dOutOfVideoMemoryX();

		DBG_ASSERT(SUCCEEDED(op_result), "Unhandled error creating texture");
	}

	// if we get to this point, the allocation succeeded and we can get rid of our old surface
	this->SetSurface(new_texture);
	this->SetWidth(i_Width);
	this->SetHeight(i_Height);
	this->SetPixelFormat(i_PFD);
}

//--------------------------------------------------------------------
//	GetSurface returns a pointer usable for shaders.
//--------------------------------------------------------------------
ID3D11ShaderResourceView* matPlainTexture::GetSurface() const
{
	return m_Texture ? m_Texture->GetShaderResourceView() : NULL;
}
g2dD3D11ResourcePtr matPlainTexture::GetResource() const
{
	return m_Texture ? m_Texture->GetResource() : NULL;
}

//--------------------------------------------------------------------
//	SetSurface sets the DirectDraw surface pointer
//--------------------------------------------------------------------
void matPlainTexture::SetSurface(g2dD3D11ResourcePtr i_Surface)
{
	// out with the old, in with the new
	m_Texture.reset(new g2dTextureDX11);

	ID3D11Texture2D* pTex2D = NULL;
	HRESULT hr = i_Surface->QueryInterface(__uuidof(ID3D11Texture2D), (void**)(&pTex2D)); 
	if (pTex2D && SUCCEEDED(hr))
	{
		m_Texture->Make(pTex2D);
		i_Surface->Release(); 
	}
	else
	{
		DBG_ERROR("SetSurface called with incorrect resource type");
	}
}

