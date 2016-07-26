/****************************************************************************\
**  matVolumeTexture.hpp
**
**      matVolumeTexture is a matTexture which represents a 3d volume
**	or 1D array of 2d Texture layers.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/mat/matVolumeTexture.hpp"

#include "Graphics/g2d/g2dExceptionX.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"

//--------------------------------------------------------------------
//	This constructor makes an "empty" surface
//--------------------------------------------------------------------
matVolumeTexture::matVolumeTexture()
:	m_NumLayers(0), m_Texture3D(NULL), m_pUnorderedAccessView(NULL),
	m_Texture3DSRV(NULL)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
matVolumeTexture::~matVolumeTexture()
{
	SAFE_RELEASE( m_Texture3DSRV );
	SAFE_RELEASE( m_pUnorderedAccessView );
	SAFE_RELEASE( m_Texture3D );
}

//--------------------------------------------------------------------
//	ReloadInfo causes the matVolumeTexture to regenerate its
//	parameters from the PAC.  Again, this function should only be
//	called by the Terawatt PAC components.
//--------------------------------------------------------------------
void matVolumeTexture::ReloadInfo()
{
	D3D11_TEXTURE3D_DESC desc;
	m_Texture3D->GetDesc(&desc);

	// Find the volume texture's number of layers:
	m_NumLayers = desc.Depth;
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
void matVolumeTexture::Make(int i_Width, int i_Height, int i_Depth, const g2dPFD& i_PFD, BIND_TYPE i_Bindings, void* i_pInitialData )
{
	ID3D11Texture3D* new_texture = NULL;
	CD3D11_TEXTURE3D_DESC desc(g2dDX11Global::D3DFormatFromPFD(i_PFD),
		i_Width, i_Height, i_Depth, 
		1, i_Bindings
	);

	// big assumption: initial data is packed tightly and pitch
	// is related directly to pixel size for this format
	D3D11_SUBRESOURCE_DATA initialData;
	initialData.pSysMem = i_pInitialData;
	initialData.SysMemPitch = (i_PFD.BitsPerPixel()*i_Width)>>3;	//convert from bit to bytes
	initialData.SysMemSlicePitch = (i_PFD.BitsPerPixel()*i_Width*i_Height)>>3;

	HRESULT op_result = g2dDX11Global::g_pDevice->CreateTexture3D(&desc, i_pInitialData ? &initialData : NULL, &new_texture);

	if( FAILED(op_result) )
	{
		g2dDX11Global::PrintDXError(op_result);
		if ( E_OUTOFMEMORY == op_result )
			throw g2dOutOfVideoMemoryX();
		DBG_ASSERT(false, "Error creating texture");
	}

	if( i_Bindings & BIND_UNORDERED_ACCESS )
	{
		SAFE_RELEASE( m_pUnorderedAccessView );
		g2dDX11Global::g_pDevice->CreateUnorderedAccessView( new_texture, NULL, &m_pUnorderedAccessView );
	}
	// create SRV:
	CreateSRV(new_texture);

	// if we get to this point, the allocation succeeded and we can get rid of our old surface
	m_Texture3D = new_texture;
	this->ReloadInfo();
}

//----------------------------------------------------------------------------
//	CreateSRV
//----------------------------------------------------------------------------
void matVolumeTexture::CreateSRV(ID3D11Texture3D* i_pNewTexture)
{
	// create SRV:
	HRESULT op_result = g2dDX11Global::g_pDevice->CreateShaderResourceView( i_pNewTexture, NULL, &m_Texture3DSRV );
	if( !SUCCEEDED(op_result) )
	{
		m_Texture3DSRV = NULL;
		g2dDX11Global::PrintDXError(op_result);
		if (op_result == E_OUTOFMEMORY)
		{
			throw g2dOutOfSystemMemoryX();
		}
		else if ( op_result == DXGI_ERROR_INVALID_CALL)
		{
			throw g2dGeneralX();
		}
		DBG_ASSERT(SUCCEEDED(op_result), "Error creating render target depth surface");
	}
}

//----------------------------------------------------------------------------
//	GetSize returns approximate amount of memory (in bytes) being
// used by this texture
//----------------------------------------------------------------------------
float matVolumeTexture::GetSize() const
{
	// volume texture is w x h x d x Bpp
	float face_size = matTexture::GetSize();
	float volume_size = face_size * m_NumLayers;

	// also, account for possiblity of mipmaps.
	D3D11_TEXTURE3D_DESC desc;
	m_Texture3D->GetDesc(&desc);
	if (desc.MipLevels > 1)
		return (volume_size * 4) / 3;

	return volume_size;
}

//--------------------------------------------------------------------
//	GetSurface returns a pointer usable for shaders.
//--------------------------------------------------------------------
ID3D11ShaderResourceView* matVolumeTexture::GetSurface() const
{
	return m_Texture3DSRV;
}
g2dD3D11ResourcePtr matVolumeTexture::GetResource() const
{
	return m_Texture3D;
}

//--------------------------------------------------------------------
//	SetSurface sets the DirectDraw surface pointer
//--------------------------------------------------------------------
void matVolumeTexture::SetSurface(g2dD3D11ResourcePtr i_Surface)
{
	// out with the old, in with the new
	ID3D11Texture3D* pTex3D = NULL;
	HRESULT hr = i_Surface->QueryInterface(__uuidof(ID3D11Texture3D), (void**)(&pTex3D)); 
	if (pTex3D && SUCCEEDED(hr))
	{
		SAFE_RELEASE( m_Texture3DSRV );
		SAFE_RELEASE( m_pUnorderedAccessView );
		SAFE_RELEASE( m_Texture3D );
		CreateSRV(pTex3D);
		m_Texture3D = pTex3D;
	}
	else
	{
		DBG_ERROR("SetSurface called with incorrect resource type");
	}
}
