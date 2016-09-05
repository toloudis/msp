/****************************************************************************\
**  matCubeRenderTargetTexture.cpp
**
**      matCubeRenderTargetTexture is a matTexture which represents an ordinary
**	rectangular texture with no special ability.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/mat/matCubeRenderTargetTexture.hpp"

#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/mat/matExceptionX.hpp"
#include "GraphicsDX11/g2d/g2dDepthStencilBufferDX11.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/private/g2dWindowDrawUtilDX11.hpp"
#include "DirectXTex/DirectXTex/DirectXTex.h"

#include <sstream>

//--------------------------------------------------------------------
//	This constructor makes an "empty" surface
//--------------------------------------------------------------------
matCubeRenderTargetTexture::matCubeRenderTargetTexture()
:	matRenderTargetTexture(false, false),
	m_CurrentFaceTarget(0)
{
	m_pCubeTextureView = NULL;
	m_pCubeFaces[0] = NULL; 
	m_pCubeFaces[1] = NULL; 
	m_pCubeFaces[2] = NULL; 
	m_pCubeFaces[3] = NULL; 
	m_pCubeFaces[4] = NULL; 
	m_pCubeFaces[5] = NULL; 
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
matCubeRenderTargetTexture::~matCubeRenderTargetTexture()
{
	m_pCubeTextureView->Release();

//	m_pColorBuffer = NULL;
	if( m_pCubeFaces[0] )
		m_pCubeFaces[0]->Release();
	if( m_pCubeFaces[1] )
		m_pCubeFaces[1]->Release();
	if( m_pCubeFaces[2] )
		m_pCubeFaces[2]->Release();
	if( m_pCubeFaces[3] )
		m_pCubeFaces[3]->Release();
	if( m_pCubeFaces[4] )
		m_pCubeFaces[4]->Release();
	if( m_pCubeFaces[5] )
		m_pCubeFaces[5]->Release();

	m_pCubeTextureD3D->Release();
}

//--------------------------------------------------------------------
//	ReloadInfo causes the matRenderTargetTexture to regenerate its
//	parameters from the PAC.  Again, this function should only be
//	called by the Terawatt PAC components.
//--------------------------------------------------------------------
void matCubeRenderTargetTexture::ReloadInfo()
{
    D3D11_TEXTURE2D_DESC desc;
	m_pCubeTextureD3D->GetDesc(&desc);

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
void matCubeRenderTargetTexture::Make(int i_Width, int i_Height, const g2dPFD& i_PFD,
								  bool i_bAllocDepthBuf/*=true*/)
{
	if (g2dDX11Global::RestrictPow2Textures())
	{
		if (!((i_Width & (i_Width - 1)) == 0) || !((i_Height & (i_Height - 1)) == 0))
		{
			throw matInvalidTextureSizeX(fsLocator());
		}
	}

	// Set up color buffer
	m_pCubeTextureD3D = NULL;

	// use floating point format for texture?
	DXGI_FORMAT format = g2dDX11Global::D3DFormatFromPFD(i_PFD);//DXGI_FORMAT_R8G8B8A8_UNORM;

	DBG_ASSERT(i_Width == i_Height, "Width must equal height for a cube texture");

	g2dD3D11TexturePtr new_texture = NULL;

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
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;

	HRESULT op_result = g2dDX11Global::g_pDevice->CreateTexture2D( &desc, NULL, &new_texture );
	if( !SUCCEEDED(op_result) )
	{
		g2dDX11Global::PrintDXError(op_result);
		if ( E_OUTOFMEMORY == op_result )
			throw g2dOutOfVideoMemoryX();
		DBG_ASSERT(false, "Error creating texture");
	}

	op_result = g2dDX11Global::g_pDevice->CreateShaderResourceView( new_texture, NULL, &m_pCubeTextureView );
	if( !SUCCEEDED(op_result) )
	{
		m_pCubeTextureView = NULL;
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

	// if we get to this point, the allocation succeeded and we can get rid of our old surface
	m_bHasMipMaps = true;
	m_pCubeTextureD3D = new_texture;


	this->ReloadInfo();

	// set up faces as render targets.
	D3D11_RENDER_TARGET_VIEW_DESC cubeFaceDesc;
	cubeFaceDesc.Format = format;
	cubeFaceDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	cubeFaceDesc.Texture2DArray.ArraySize = 1;
	cubeFaceDesc.Texture2DArray.FirstArraySlice = 0;
	cubeFaceDesc.Texture2DArray.MipSlice = 0;
	for (int i = 0; i < 6; i++)
	{
		cubeFaceDesc.Texture2DArray.FirstArraySlice = i;
		HRESULT hr = g2dDX11Global::g_pDevice->CreateRenderTargetView(m_pCubeTextureD3D, 
			&cubeFaceDesc, &m_pCubeFaces[i]);

		if( !SUCCEEDED(hr) )
		{
			g2dDX11Global::PrintDXError(op_result);
			if ( E_OUTOFMEMORY == op_result )
				throw g2dOutOfVideoMemoryX();

			DBG_ASSERT(false, "Error getting surface from render target texture");
		}
	}


	// Set up depth buffer
	if (i_bAllocDepthBuf)
	{
		m_DepthStencil.reset(new g2dDepthStencilBufferDX11());
		m_DepthStencil->Make(i_Width,i_Height,
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			g2dDX11Global::DefaultSampleDesc()
		);
	}

	if (i_bAllocDepthBuf == false)
		m_bNoDepthRequested = true;

	// sync up new size and pixel format info
	ReloadInfo();
	SetFaceTarget(0);
}

//------------------------------------------------------------------------
// set one of the 6 faces as the current render target.
//------------------------------------------------------------------------
void matCubeRenderTargetTexture::SetFaceTarget(int i_CubeFace)
{
	DBG_ASSERT(i_CubeFace>=0 && i_CubeFace<6, "Set bad cube face index");
	m_CurrentFaceTarget = i_CubeFace;
	configure_device();
}

void matCubeRenderTargetTexture::SaveFaces()
{
	for (int i = 0; i < 6; i++)
	{
        std::wstringstream fname;
        fname << L"face" << i << L".png";

		ID3D11Resource* res = NULL;
		m_pCubeFaces[i]->GetResource(&res);

        DirectX::ScratchImage s;
        HRESULT hr = DirectX::CaptureTexture(g2dDX11Global::g_pDevice, g2dDX11Global::g_pDeviceContext, res, s);
        const DirectX::Image* img = s.GetImage(0, 0, 0);
        assert(img);
        hr = DirectX::SaveToWICFile(*img, DirectX::WIC_FLAGS::WIC_FLAGS_NONE,
            DirectX::GetWICCodec(DirectX::WICCodecs::WIC_CODEC_PNG), fname.str().c_str());
		
        res->Release();
	}
}

//------------------------------------------------------------------------
//	Clear fills the screen with the given color.
//------------------------------------------------------------------------
void matCubeRenderTargetTexture::Clear(const g2dRGBColor& i_Color)
{
	for (int i = 0; i < 6; i++)
	{
		SetFaceTarget(i);
		g2dWindowDrawUtilDX11::Clear(i_Color);
	}
}

//------------------------------------------------------------------------
//	Clear fills the screen with the given color.
//------------------------------------------------------------------------
//void matCubeRenderTargetTexture::Clear(const maFloatRGBA& i_Color)
//{
//	for (int i = 0; i < 6; i++)
//	{
//		SetFaceTarget(i);
//		g2dWindowDrawUtilDX11::Clear(i_Color);
//	}
//}
//------------------------------------------------------------------------
//	Clear fills the screen with the given color and sets depth and stencil
//------------------------------------------------------------------------
//virtual 
void matCubeRenderTargetTexture::Clear( const maFloatRGBA& i_Color, bool i_ClearDepth /*= false*/, 
								   float i_Depth /*= 1*/, bool i_ClearStencil /*= true*/, unsigned int i_Stencil /*= 0*/)
{
	// don't need to set render target to clear.
//	configure_device();

	float c[4];
	c[0] = i_Color.m_Red;
	c[1] = i_Color.m_Green;
	c[2] = i_Color.m_Blue;
	c[3] = i_Color.m_Alpha;
	g2dDX11Global::g_pDeviceContext->ClearRenderTargetView(this->m_pCubeFaces[m_CurrentFaceTarget], c);

	// temp: does it work to clear here?
	if (i_ClearDepth && m_DepthStencil)
	{
		g2dDX11Global::g_pDeviceContext->ClearDepthStencilView(this->m_DepthStencil->GetDepthView(), 
			D3D11_CLEAR_DEPTH | (i_ClearStencil ? D3D11_CLEAR_STENCIL : 0), i_Depth, i_Stencil);
	}
}

//----------------------------------------------------------------------------
//	GetSize returns approximate amount of memory (in kbytes) being
// used by this texture
//----------------------------------------------------------------------------
float matCubeRenderTargetTexture::GetSize() const
{
	// 6 color buffers and 1 depth buffer. 

	// get size of main surface, times 6 for the cube faces.
	float mainSize = matTexture::GetSize() * 6;

	// we always generate mipmaps. see Make().
	mainSize = (mainSize * 4) / 3;
	
	// now add in size of depth buffer 
	float otherSize = 0;
	// Don't add the size of a depthbuffer because it automatically adds itself
	// when created.

	// For the internal memory counter, this is correct, but if we really want to
	// know the total burden of this object, then we might want to add it in,
	// even if it is shared.

	return mainSize + otherSize;
}

//------------------------------------------------------------------------
// set up device to render to our window
//------------------------------------------------------------------------
void matCubeRenderTargetTexture::configure_device()
{
	DBG_ASSERT(m_pCubeFaces[m_CurrentFaceTarget] && (m_DepthStencil || m_bNoDepthRequested), 
		"Need to call Make() before rendering to texture");
	if (m_DepthStencil)
		g2dDX11Global::SetRenderTargets(m_pCubeFaces[m_CurrentFaceTarget], m_DepthStencil->GetDepthView());
	else
		g2dDX11Global::SetColorTarget(m_pCubeFaces[m_CurrentFaceTarget]);

	D3D11_VIEWPORT vprt;
	vprt.TopLeftX = vprt.TopLeftY = 0;
	vprt.Width = (float)this->GetWidth();
	vprt.Height = (float)this->GetHeight();
	vprt.MinDepth = 0;
	vprt.MaxDepth = 1;
	g2dDX11Global::g_pDeviceContext->RSSetViewports(1, &vprt);
}

//--------------------------------------------------------------------
//	GetSurface returns a pointer usable for shaders.
//--------------------------------------------------------------------
ID3D11ShaderResourceView* matCubeRenderTargetTexture::GetSurface() const
{
	return m_pCubeTextureView;
}
g2dD3D11ResourcePtr matCubeRenderTargetTexture::GetResource() const
{
	return m_pCubeTextureD3D;
}
