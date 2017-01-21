/****************************************************************************\
**  matStaticCubeTexture.hpp
**
**      matStaticCubeTexture is a matTexture which represents a cubic
**	environment map which is preset at creation (not rendered to).
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/mat/matStaticCubeTexture.hpp"

#include "Graphics/g2d/g2dExceptionX.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"

//--------------------------------------------------------------------
//	This constructor makes an "empty" surface
//--------------------------------------------------------------------
matStaticCubeTexture::matStaticCubeTexture()
: m_Texture2D(NULL), m_Texture2DSRV(NULL)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
matStaticCubeTexture::~matStaticCubeTexture()
{
	if (m_Texture2DSRV)
		m_Texture2DSRV->Release();
	if (m_Texture2D)
		m_Texture2D->Release();
}

//--------------------------------------------------------------------
//	ReloadInfo causes the matStaticCubeTexture to regenerate its
//	parameters from the PAC.  Again, this function should only be
//	called by the Terawatt PAC components.
//--------------------------------------------------------------------
void matStaticCubeTexture::ReloadInfo()
{
	D3D11_TEXTURE2D_DESC desc;
	m_Texture2D->GetDesc(&desc);

    DBG_ASSERT(desc.ArraySize == 6, "matStaticCubeTexture is not a true cube texture");

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
void matStaticCubeTexture::Make(int i_Width, int i_Height, const g2dPFD& i_PFD)
{
	DBG_ASSERT(i_Width == i_Height, "Width must equal height for a cube texture");

	g2dD3D11TexturePtr new_texture = NULL;

	// use floating point format for texture?
	DXGI_FORMAT format = g2dDX11Global::D3DFormatFromPFD(i_PFD);//DXGI_FORMAT_R8G8B8A8_UNORM;

	// cube tex is array of 6 2d tex
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = i_Width;
    desc.Height = i_Height;
    desc.MipLevels = 0;
    //dstex.MipLevels = 1;
    desc.ArraySize = 6;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Format = format;
    desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;

	HRESULT op_result = g2dDX11Global::g_pDevice->CreateTexture2D( &desc, NULL, &new_texture );
	if( !SUCCEEDED(op_result) )
	{
		g2dDX11Global::PrintDXError(op_result);
		DBG_ASSERT(false, "Error creating texture");
	}

	// if we get to this point, the allocation succeeded and we can get rid of our old surface
	this->SetSurface(new_texture);
	this->ReloadInfo();
}

//----------------------------------------------------------------------------
//	GetSize returns approximate amount of memory (in kbytes) being
// used by this texture
//----------------------------------------------------------------------------
float matStaticCubeTexture::GetSize() const
{
	// cube map texture size is 6 times bigger, accounting for the 6 cube faces.
	float face_size = matTexture::GetSize();
	float cube_size = face_size * 6;

	// also, account for possiblity of mipmaps.
	D3D11_TEXTURE2D_DESC desc;
	m_Texture2D->GetDesc(&desc);
	if (desc.MipLevels > 1)
		return (cube_size * 4) / 3;

	return cube_size;
}

//--------------------------------------------------------------------
//	GetSurface returns a pointer usable for shaders.
//--------------------------------------------------------------------
ID3D11ShaderResourceView* matStaticCubeTexture::GetSurface() const
{
	return m_Texture2DSRV;
}
g2dD3D11ResourcePtr matStaticCubeTexture::GetResource() const
{
	return m_Texture2D;
}

//--------------------------------------------------------------------
//	SetSurface sets the DirectDraw surface pointer
//--------------------------------------------------------------------
void matStaticCubeTexture::SetSurface(g2dD3D11ResourcePtr i_Surface)
{
	// out with the old, in with the new
	ID3D11Texture2D* pTex2D = NULL;
	HRESULT hr = i_Surface->QueryInterface(__uuidof(ID3D11Texture2D), (void**)(&pTex2D)); 
	if (pTex2D && SUCCEEDED(hr))
	{
		if (m_Texture2D)
			m_Texture2D->Release();
		m_Texture2D = pTex2D;

		// create SRV:
		hr = g2dDX11Global::g_pDevice->CreateShaderResourceView( m_Texture2D, NULL, &m_Texture2DSRV );
		if( !SUCCEEDED(hr) )
		{
			m_Texture2DSRV = NULL;
			g2dDX11Global::PrintDXError(hr);
			if (hr == E_OUTOFMEMORY)
			{
				throw g2dOutOfSystemMemoryX();
			}
			else if ( hr == DXGI_ERROR_INVALID_CALL)
			{
				throw g2dGeneralX();
			}
			DBG_ASSERT(SUCCEEDED(hr), "Error creating render target depth surface");
		}
	}
	else
	{
		DBG_ERROR("SetSurface called with incorrect resource type");
	}
}
