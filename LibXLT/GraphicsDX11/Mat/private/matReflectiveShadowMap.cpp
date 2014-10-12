/****************************************************************************\
**  matReflectiveShadowMap.cpp
**
**      matReflectiveShadowMap is a matTexture which represents an ordinary
**	rectangular texture with no special ability.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/mat/matReflectiveShadowMap.hpp"

#include "Core/fs/fsLocator.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#include "Graphics/mat/matExceptionX.hpp"
#include "GraphicsDX11/g2d/g2dDepthStencilBufferDX11.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/g2dRenderTargetTextureDX11.hpp"
#include "GraphicsDX11/g2d/private/g2dWindowDrawUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"

namespace 
{
	DXGI_FORMAT l_DepthFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DXGI_FORMAT l_ColorFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
};

//--------------------------------------------------------------------
//	This constructor makes an "empty" surface
//--------------------------------------------------------------------
matReflectiveShadowMap::matReflectiveShadowMap()
{
	//for (int i = 0; i < e_NumRSMRenderTarget; i++)
	//{
	//	//m_MainTexture[i] = NULL;
	//	m_pColorBuffer[i] = NULL;
	//	m_pShaderView[i] = NULL;
	//}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
matReflectiveShadowMap::~matReflectiveShadowMap()
{
	//for (int i = 0; i < e_NumRSMRenderTarget; i++)
	//{
	//	if( m_pColorBuffer[i] )
	//		m_pColorBuffer[i]->Release();
	//}

	//for (int i = 0; i < e_NumRSMRenderTarget; i++)
	//{
	//	if (m_pShaderView[i])
	//		m_pShaderView[i]->Release();

	//	/*if (m_MainTexture)
	//		m_MainTexture[i]->Release();*/
	//}
}

//--------------------------------------------------------------------
//	ReloadInfo causes the matShadowMap to regenerate its
//	parameters from the PAC.  Again, this function should only be
//	called by the Terawatt PAC components.
//--------------------------------------------------------------------
void matReflectiveShadowMap::ReloadInfo()
{
	D3D11_TEXTURE2D_DESC desc;
	m_MainTexture[0]->GetResource()->GetDesc(&desc);

	this->SetWidth(desc.Width);
	this->SetHeight(desc.Height);
	g2dPFD pfd;
	g2dDX11Global::PFDFromD3DFormat(desc.Format, pfd);
	this->SetPixelFormat(pfd);

	//	try for a g2dD3D11VolumeTexturePtr - if it's not one, there's an error
	/*ID3D11Texture2D* pTex = NULL;
	HRESULT qi_result = GetResource()->QueryInterface(g2dIID_ID3D11Texture2D, (void**)&pTex);
	if( !SUCCEEDED(qi_result) )
	{
		g2dDX11Global::PrintDXError( qi_result );
		DBG_ASSERT( false, "Error getting texture2d surface from query" );
	}

	D3D11_TEXTURE2D_DESC desc;
	pTex->GetDesc(&desc);

	m_pMainTextureD3D[0] = pTex;
	this->SetWidth(desc.Width);
	this->SetHeight(desc.Height);
	g2dPFD pfd;
	g2dDX11Global::PFDFromD3DFormat(desc.Format, pfd);
	this->SetPixelFormat(pfd);*/

	//pTex->Release();
}

//--------------------------------------------------------------------
//	Make makes the surface into one with the given dimensions and
//	pixel format.  If the pixel format is not supported it will
//	throw a matUnsupportedPixelFormatX, and leave the old surface
//	intact.
//--------------------------------------------------------------------
void matReflectiveShadowMap::Make(int i_Width, int i_Height)
{
	if (g2dDX11Global::RestrictPow2Textures())
	{
		if (!((i_Width & (i_Width - 1)) == 0) || !((i_Height & (i_Height - 1)) == 0))
		{
			throw matInvalidTextureSizeX(fsLocator());
		}
	}

	// Set up color buffer
	//m_MainTexture[0] = NULL;	

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = i_Width;
	desc.Height = i_Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = l_ColorFormat;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0; // no cpu access
	desc.MiscFlags = 0;

	// main texture for storing depth/wpos, normal, flux
	for (int i = 0; i < e_NumRSMRenderTarget; i++)
	{
		/*HRESULT op_result = g2dDX11Global::g_pDevice->CreateTexture2D(&desc, NULL, &m_pMainTextureD3D[i]);
		if( !SUCCEEDED(op_result) )
		{
			g2dDX11Global::PrintDXError(op_result);
			if ( E_OUTOFMEMORY == op_result )
				throw g2dOutOfVideoMemoryX();

			DBG_ASSERT(false, "Unhandled error creating texture");
			}*/
		m_MainTexture[i].reset(new g2dRenderTargetTextureDX11());
		m_MainTexture[i]->Make(i_Width,i_Height,
			l_ColorFormat,
			g2dDX11Global::DefaultSampleDesc(),
			g2dResourceCounterDX11::eShadow
			);
	}

	// Get surface view from our texture for rendering
	/*for (int i = 0; i < e_NumRSMRenderTarget; i++)
	{
		HRESULT op_result = g2dDX11Global::g_pDevice->CreateRenderTargetView(m_MainTexture[i]->GetResource(), NULL, &m_pColorBuffer[i]);
		if( !SUCCEEDED(op_result) )
		{
			g2dDX11Global::PrintDXError(op_result);
			DBG_ASSERT(false, "Error getting rendertarget view from render target texture");
		}
	}*/

	// Set up depth buffer
	m_DepthStencil.reset(new g2dDepthStencilBufferDX11());
	m_DepthStencil->Make(i_Width,i_Height,
		l_DepthFormat,
		g2dDX11Global::DefaultSampleDesc(),
		g2dResourceCounterDX11::eShadow
	);

	//this->SetSurface(m_pMainTextureD3D);
	//SetAllSurfaces();


	// sync up new size and pixel format info
	ReloadInfo();

//	DBG_LOG4("render target texture created %05dx%05d %d buffers, size %d", 
//		this->GetWidth(), this->GetHeight(), (depth_buffer==NULL)?1:2, this->GetSize());
}

//------------------------------------------------------------------------
//	GetPixelFormat returns the current pixel format of the render target.
//------------------------------------------------------------------------
const g2dPFD& matReflectiveShadowMap::GetPixelFormat() const
{
	return matTexture::GetPixelFormat();
}

//------------------------------------------------------------------------
//	GetDimensions returns the width and height of the render target.
//------------------------------------------------------------------------
void matReflectiveShadowMap::GetDimensions(int& o_Width, int& o_Height) const
{
	o_Width = GetWidth();
	o_Height = GetHeight();
}

//--------------------------------------------------------------------
//	GetTextureSurface returns the main D3D texture pointer.
//--------------------------------------------------------------------
g2dD3D11TexturePtr matReflectiveShadowMap::GetTextureSurface(int i_index)
{
	DBG_ASSERT(i_index >= 0 && i_index < e_NumRSMRenderTarget, "matReflectiveSHadowMap: request render target view index out of range");
	return m_MainTexture[i_index]->GetResource();
}


//--------------------------------------------------------------------
// Return an object pointer to use for rendering to this texture.
//	Only certain texture types can return this object, most will
//	return NULL.  The object pointed to will be owned by this 
//	texture, it should not be deleted by the user.
//--------------------------------------------------------------------
//virtual 
g2dRenderTarget* matReflectiveShadowMap::GetRenderTargetAPI()
{
	// In this case we are using multiple inheritance, but we could 
	// also have used containment and made the render target a member
	// of this class.
	return this;
}


//------------------------------------------------------------------------
// BeginScene must be called before rendering to this window
//------------------------------------------------------------------------
//virtual 
void matReflectiveShadowMap::BeginScene()
{
	// direct device to output to our window
	BeginScene(0);
}

//------------------------------------------------------------------------
// BeginScene must be called before rendering to this window 
//------------------------------------------------------------------------
void matReflectiveShadowMap::BeginScene(int i_index)
{
	DBG_ASSERT(i_index >= 0 && i_index < e_NumRSMRenderTarget, "index out of range");
	configure_device(i_index);
	g2dDX11Global::BeginScene();
}

//------------------------------------------------------------------------
// similar to BeginScene, call this to make this the current target of all
// subsequent rendering calls.  Need not be followed by EndScene.
//------------------------------------------------------------------------
void matReflectiveShadowMap::MakeCurrent()
{
	// direct device to output to our window
	ID3D11RenderTargetView* rtview[e_NumRSMRenderTarget];
	for (int i = 0; i < e_NumRSMRenderTarget; i++)
		rtview[i] = m_MainTexture[i]->GetRenderTargetView();
	
	if (m_DepthStencil)
		g2dDX11Global::g_pDeviceContext->OMSetRenderTargets(e_NumRSMRenderTarget, rtview, m_DepthStencil->GetDepthView());
	else
		g2dDX11Global::SetColorTarget(m_MainTexture[0]->GetRenderTargetView());

	D3D11_VIEWPORT vprt;
	vprt.TopLeftX = vprt.TopLeftY = 0;
	vprt.Width = (float)this->GetWidth();
	vprt.Height = (float)this->GetHeight();
	vprt.MinDepth = 0;
	vprt.MaxDepth = 1;
	g2dDX11Global::g_pDeviceContext->RSSetViewports(1, &vprt);
	//configure_device(0);
}

//------------------------------------------------------------------------
// similar to BeginScene, call this to make this the current target of all
// subsequent rendering calls.  Need not be followed by EndScene.
//------------------------------------------------------------------------
void matReflectiveShadowMap::MakeCurrent(int i_index)
{
	DBG_ASSERT(i_index >= 0 && i_index < e_NumRSMRenderTarget, "index out of range");
	configure_device(i_index);
}

//------------------------------------------------------------------------
//	EndScene must be called when you are done with the drawing operations
//	on the current frame.  It will cause whatever drawing you have
//	requested to be visible on the screen.
//------------------------------------------------------------------------
//virtual 
void matReflectiveShadowMap::EndScene()
{
	// nothing needed for render target textures?
	g2dDX11Global::EndScene();
}

//------------------------------------------------------------------------
//	Clear fills the screen with the given color.
//------------------------------------------------------------------------
//virtual 
void matReflectiveShadowMap::Clear(const g2dRGBColor& i_Color)
{
	Clear(0, i_Color);
}

//------------------------------------------------------------------------
//	Clear fills the screen with the given color.
//------------------------------------------------------------------------
void matReflectiveShadowMap::Clear(int i_index, const g2dRGBColor& i_Color)
{
	DBG_ASSERT(i_index >= 0 && i_index < e_NumRSMRenderTarget, "index out of range");
	configure_device(i_index);
	g2dWindowDrawUtilDX11::Clear(i_Color);
}

//------------------------------------------------------------------------
//	Clear fills the screen with the given color and sets depth and stencil
//------------------------------------------------------------------------
//virtual 
void matReflectiveShadowMap::Clear( const maFloatRGBA& i_Color, bool i_ClearDepth /*= false*/, 
								   float i_Depth /*= 1*/, bool i_ClearStencil /*= true*/, unsigned int i_Stencil /*= 0*/)
{
	// don't need to set render target to clear.
//	configure_device();

	float c[4];
	c[0] = i_Color.m_Red;
	c[1] = i_Color.m_Green;
	c[2] = i_Color.m_Blue;
	c[3] = i_Color.m_Alpha;
	for (int i = 0; i < e_NumRSMRenderTarget; i++)
		g2dDX11Global::g_pDeviceContext->ClearRenderTargetView(m_MainTexture[i]->GetRenderTargetView(), c);

	// temp: does it work to clear here?
	if (i_ClearDepth && m_DepthStencil)
	{
		g2dDX11Global::g_pDeviceContext->ClearDepthStencilView(this->m_DepthStencil->GetDepthView(), 
			D3D11_CLEAR_DEPTH | (i_ClearStencil ? D3D11_CLEAR_STENCIL : 0), i_Depth, i_Stencil);
	}
}

//------------------------------------------------------------------------
//	Clear fills the depthstencil buffer with the given values.
//	If there is no depthstencil, then this does nothing.
//------------------------------------------------------------------------
//virtual 
void matReflectiveShadowMap::ClearDepthStencil(float i_Depth /*= 1*/, bool i_ClearStencil /*= true*/, unsigned int i_Stencil /*= 0*/)
{
	if (this->m_DepthStencil)
	{
		configure_device(0);
		g2dDX11Global::g_pDeviceContext->ClearDepthStencilView(m_DepthStencil->GetDepthView(), 
			i_ClearStencil ? D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL : D3D11_CLEAR_DEPTH,
			i_Depth, i_Stencil);
	}
}

//------------------------------------------------------------------------
// set up device to render to our window
//------------------------------------------------------------------------
void matReflectiveShadowMap::configure_device(int i_index)
{
	/*DBG_ASSERT(i_pColorBuffer && m_DepthStencil, 
		"Need to call Make() before rendering to texture");*/
	if (m_DepthStencil)
		g2dDX11Global::SetRenderTargets(m_MainTexture[i_index]->GetRenderTargetView(), m_DepthStencil->GetDepthView());
	else
		g2dDX11Global::SetColorTarget(m_MainTexture[i_index]->GetRenderTargetView());

	D3D11_VIEWPORT vprt;
	vprt.TopLeftX = vprt.TopLeftY = 0;
	vprt.Width = (float)this->GetWidth();
	vprt.Height = (float)this->GetHeight();
	vprt.MinDepth = 0;
	vprt.MaxDepth = 1;
	g2dDX11Global::g_pDeviceContext->RSSetViewports(1, &vprt);
}

//----------------------------------------------------------------------------
//	GetSize returns approximate amount of memory (in kbytes) being
// used by this texture
//----------------------------------------------------------------------------
float matReflectiveShadowMap::GetSize() const
{
	// get size of main surface (could be color or depth buffer)
	float mainSize = matTexture::GetSize();
	
	// now add in size of 2nd buffer (depth buffer or the R32 color buffer)
	// todo: add in correct other buffers' memory usage
	float otherSize = 0;

	return mainSize + otherSize;

}

ID3D11RenderTargetView* matReflectiveShadowMap::GetColorBuffer() const 
{ 
	return m_MainTexture[0]->GetRenderTargetView(); 
}

//------------------------------------------------------------------------
// call this to make this depth buffer the current depth buffer for all
// subsequent rendering calls.  
//------------------------------------------------------------------------
void matReflectiveShadowMap::MakeDepthCurrent()
{
	if (m_DepthStencil)
	{
		g2dDX11Global::SetDepthTarget(m_DepthStencil->GetDepthView());
	}
	else
	{
		DBG_TRACE("MakeDepthCurrent called with no depth buffer.");
	}
}

//------------------------------------------------------------------------
//	Returns true if this object has allocated a depth buffer.
//------------------------------------------------------------------------
bool matReflectiveShadowMap::GetHasDepthBuffer() const 
{
	return m_DepthStencil != NULL;
}


//--------------------------------------------------------------------
//	SetSurface sets the DirectDraw surface pointer
//--------------------------------------------------------------------
//void matReflectiveShadowMap::SetAllSurfaces()
//{
//	/*for (int i = 0; i < e_NumRSMRenderTarget; i++)
//	{
//		if (m_pShaderView[i])
//		{
//			HRESULT op_result = m_pShaderView[i]->Release();
//			if( !SUCCEEDED(op_result) )
//			{
//				g2dDX11Global::PrintDXError(op_result);
//				DBG_ASSERT(false, "Error releasing shader view of texture");
//			}
//		}
//
//		HRESULT op_result = g2dDX11Global::g_pDevice->CreateShaderResourceView(m_MainTexture[i]->GetResource(), NULL, &m_pShaderView[i]);
//		if( !SUCCEEDED(op_result) )
//		{
//			g2dDX11Global::PrintDXError(op_result);
//			DBG_ASSERT(false, "Error creating shader view of texture");
//		}
//	}*/
//}

g2dD3D11RenderTargetPtr matReflectiveShadowMap::GetRenderTargetView(int i_index) const
{
	DBG_ASSERT(i_index >= 0 && i_index < e_NumRSMRenderTarget, "matReflectiveSHadowMap: request render target view index out of range");
	return m_MainTexture[i_index]->GetRenderTargetView();
}

ID3D11ShaderResourceView* matReflectiveShadowMap::GetShaderView(int i_index) const
{
	DBG_ASSERT(i_index >= 0 && i_index < e_NumRSMRenderTarget, "matReflectiveSHadowMap: request surface view index out of range");
	return m_MainTexture[i_index]->GetShaderResourceView();
}

//--------------------------------------------------------------------
//	GetSurface returns a pointer usable for shaders.
//--------------------------------------------------------------------
ID3D11ShaderResourceView* matReflectiveShadowMap::GetSurface() const
{
	return m_MainTexture[0]->GetShaderResourceView();
}

g2dD3D11ResourcePtr matReflectiveShadowMap::GetResource() const
{
	return m_MainTexture[0]->GetResource();
}

shared_ptr<g2dDepthStencilBuffer> matReflectiveShadowMap::GetDepthStencilBuffer() const
{
	return m_DepthStencil;
}

void matReflectiveShadowMap::SetDepthBuffer( shared_ptr<g2dDepthStencilBuffer> i_DepthStencil )
{
	m_DepthStencil = boost::dynamic_pointer_cast<g2dDepthStencilBufferDX11>(i_DepthStencil);
}
