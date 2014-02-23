/*****************************************************************************
**  g2dFontUtilDX11.cpp
**
**      g2dFontUtilDX11 contains the windows implementation of the
**	g2dFontUtil.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g2d/g2dResetHandler.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/scr/scrText.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "GraphicsDX11/mat/matTextureDX11.hpp"
#include "GraphicsDX11/bump/bumpTriMeshBumpFrag.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/g2dFullscreenQuad.hpp"
#include "GraphicsDX11/g2d/private/g2dFontUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"

#include <string>


//------------------------------------------------------------------------------
//	library pragmas
//------------------------------------------------------------------------------
//#pragma comment(lib,"gdi32.lib")



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
namespace
{
g3dBlendStateMgr::BlendState* stp_Blend;
g3dDepthStencilStateMgr::DepthStencilState* dsp_State;
float l_baseFontSize = 10.0f;
fsLocator l_GloablFontMap;

struct CB_PS_Color
{
	float m_Color[4];
};

class g2dFontReloader : public g2dResetHandler
{
	public:

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		g2dFontReloader(g2dFontUtilDX11::FontMap &i_Fonts)
			: m_Fonts(i_Fonts) {}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Deallocate();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Reallocate();

	private:
		g2dFontUtilDX11::FontMap& m_Fonts;
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void g2dFontReloader::Deallocate()
{
	DBG_LOG("Destroying D3DXFONTs");

	//	dump all our ID3DXFONT
	g2dFontUtilDX11::FontMapIt it;
	g2dFontUtilDX11::FontMapIt end = m_Fonts.end();

	for( it = m_Fonts.begin() ; it != end ; ++it )
	{
//		it->second.m_Font->Release();
//		it->second.m_Font = NULL;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void g2dFontReloader::Reallocate()
{
	//	remake all our D3DXFONT
	g2dFontUtilDX11::FontMapIt it;
	g2dFontUtilDX11::FontMapIt end = m_Fonts.end();

	DBG_LOG("Remaking D3DXFONTs");

	for( it = m_Fonts.begin() ; it != end ; ++it )
	{
		// TO DO: implement reallocate font!
//		::D3DX11CreateFontIndirect(g2dDX11Global::g_pDevice, &(it->second.m_FontDesc), &(it->second.m_Font));
	}

	DBG_LOG("finished g2dFontReloader::Reallocate");
}

}

//------------------------------------------------------------------------
//	g2dFontObjectDX11
//------------------------------------------------------------------------
g2dFontObjectDX11::g2dFontObjectDX11(matTexture* i_FontMap): 
	m_TextSize(10), m_FontMap(i_FontMap)
{
	m_TextSprite = new scrText();
	m_TextSprite->SetFontMapImage(i_FontMap, 320, 256);
	m_TextSprite->SetText(itString(""));
	m_TextSprite->SetScreenBased(true);
	m_TextSprite->SetTextSize( m_TextSize / l_baseFontSize);
	m_TextSprite->SetBackgroundColor(maFloatRGBA(0,0,0,0));
	m_TextSprite->SetForegroundColor(maFloatRGBA(1,1,1,1));
}

g2dFontObjectDX11::~g2dFontObjectDX11()
{
	m_FontMap = NULL;
	if (m_TextSprite)
		delete m_TextSprite;
	m_TextSprite = NULL;
}

void g2dFontObjectDX11::DrawText(const itString& i_Text, const maFloatRGBA& i_Color, int i_OffsetX, int i_OffsetY)
{
	// Save the old viewport
	D3D11_VIEWPORT vpOld[D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX];
	UINT nViewPorts = 1;
	g2dDX11Global::g_pDeviceContext->RSGetViewports( &nViewPorts, vpOld );

	// Setup Render state
	g3dBlendStateMgr::SetBlendState(stp_Blend);
	g3dDepthStencilStateMgr::SetDepthStencilState(dsp_State);
	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	// Setup the viewport to match the backbuffer
	D3D11_VIEWPORT vp;
	vp.Width = (float)200;
	vp.Height = (float)150;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = (float)i_OffsetX;
	vp.TopLeftY = (float)i_OffsetY;
	g2dDX11Global::g_pDeviceContext->RSSetViewports( 1, &vp );

	m_TextSprite->SetTextSize( m_TextSize / l_baseFontSize);
	m_TextSprite->SetForegroundColor(i_Color);
	m_TextSprite->SetText(i_Text);

	const tmeshFrag* pFrag = dynamic_cast<const tmeshFrag*>( m_TextSprite->GetSceneNode()->GetFragment() );

	ID3D11Buffer* buf[1] = { pFrag->GetVertexBuffer()->GetVertexBuffer() };
	UINT strides[1] = { pFrag->GetVertexStride() };
	UINT offsets[1] = {0};

	int numInds = 0;
	if( pFrag->GetPrimitiveType() == tmeshFrag::e_TriangleList )
	{
		numInds = pFrag->GetNumNonShadowIndices();
	}
	else
	{
		numInds = pFrag->GetNumIndices();
	}

	int startIndex = 0;
	if (pFrag->GetSubFragments().size() > 0)
	{
		startIndex = pFrag->GetSubFragments()[m_TextSprite->GetSceneNode()->GetSubFragment()].m_startIndex;
		numInds = pFrag->GetSubFragments()[m_TextSprite->GetSceneNode()->GetSubFragment()].m_numTris * 3; // assumes tri list.
	}

	g2dDX11Global::g_pDeviceContext->IASetInputLayout(m_pQuadLayout);
	g2dDX11Global::g_pDeviceContext->IASetVertexBuffers(0, 1, buf, strides, offsets);
	g2dDX11Global::g_pDeviceContext->IASetIndexBuffer(pFrag->GetIndexBuffer()->GetIndexBuffer(), 
		(pFrag->GetIndexBuffer()->GetSizeOfIndex() == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
	D3D11_PRIMITIVE_TOPOLOGY type = (D3D11_PRIMITIVE_TOPOLOGY)pFrag->GetPrimitiveType();
	g2dDX11Global::g_pDeviceContext->IASetPrimitiveTopology(type);

	g2dDX11Global::g_pDeviceContext->VSSetShader( m_pQuadVS, NULL, 0 );
	g2dDX11Global::g_pDeviceContext->PSSetShader( m_pQuadPS, NULL, 0 );

	ID3D11ShaderResourceView* aRes = g3dDX11TextureUtil::GetD3DTexture(m_FontMap);
	g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 1, &aRes);

	// set draw color into constnat buffer
    D3D11_MAPPED_SUBRESOURCE MappedResource;
    ( g2dDX11Global::g_pDeviceContext->Map( m_pcbPSColor, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource ) );
    CB_PS_Color* pPSColor = ( CB_PS_Color* )MappedResource.pData;  
	pPSColor->m_Color[0] = i_Color.GetRed();
	pPSColor->m_Color[1] = i_Color.GetGreen();
	pPSColor->m_Color[2] = i_Color.GetBlue();
	pPSColor->m_Color[3] = i_Color.GetAlpha();
    g2dDX11Global::g_pDeviceContext->Unmap( m_pcbPSColor, 0 );
    g2dDX11Global::g_pDeviceContext->PSSetConstantBuffers( 0, 1, &m_pcbPSColor );

	g2dDX11Global::g_pDeviceContext->DrawIndexed(numInds, startIndex, 0);

	// Restore the Old viewport
	g2dDX11Global::g_pDeviceContext->RSSetViewports( nViewPorts, vpOld );
}

void g2dFontObjectDX11::SetFontMap(matTexture* i_fontMap)
{
	m_FontMap = i_fontMap;

	if (m_TextSprite)
		m_TextSprite->SetFontMapImage(i_fontMap, 320, 256);
}

void g2dFontObjectDX11::Init()
{
	stp_Blend = new g3dBlendStateMgr::BlendState(false, TRUE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE);
	
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );
	dsp_State = new g3dDepthStencilStateMgr::DepthStencilState(
		FALSE, FALSE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT);

	InitDefaultShader();
}
void g2dFontObjectDX11::Cleanup()
{
	delete dsp_State;
	delete stp_Blend;

	CleanupDefaultShader();
}

ID3D11InputLayout* g2dFontObjectDX11::m_pQuadLayout = NULL;
ID3D11VertexShader* g2dFontObjectDX11::m_pQuadVS = NULL;
ID3D11PixelShader* g2dFontObjectDX11::m_pQuadPS = NULL;
ID3D11Buffer* g2dFontObjectDX11::m_pcbPSColor = NULL;

void g2dFontObjectDX11::InitDefaultShader()
{
	HRESULT hr;
	
	// the dx input layout, corresponds to the struct in the shader
    const D3D11_INPUT_ELEMENT_DESC quadlayout[] =
    {
        //{ "SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        //{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		{"SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

	const char* fullscreenQuadVSSource = "\
struct QuadVS_Input\
{\
    float3 Pos : SV_POSITION;\
	float3 Normal	: NORMAL;\
	float2 Tex : TEXCOORD0;\
};\
\
struct QuadVS_Output\
{\
    float4 Pos : SV_POSITION;\
    float2 Tex : TEXCOORD0;\
};\
\
QuadVS_Output QuadVS( QuadVS_Input Input )\
{\
    QuadVS_Output Output;\
	Output.Pos = float4(Input.Pos.xyz, 1.0f);\
	Output.Tex = Input.Tex;\
    return Output;\
}\
";

const char* fullscreenQuadPSSource = "\
Texture2D fontMap;\
SamplerState DefaultSampler\
{\
	FILTER = MIN_MAG_MIP_LINEAR;\
    AddressU = WRAP;\
    AddressV = WRAP;\
};\
struct QuadVS_Output\
{\
    float4 Pos : SV_POSITION;\
    float2 Tex : TEXCOORD0;\
};\
struct pixelOutput {\
	float4 col : SV_TARGET;\
};\
cbuffer cb0\
{\
	float4 g_Color;\
};\
pixelOutput QuadPS( QuadVS_Output Input )\
{\
	float4 color = g_Color;\
	color *= fontMap.Sample(DefaultSampler, Input.Tex);\
	/*THIS ALPHA ONLY WORKS WITH SATURATED COLORS - NEED ALPHA IN THE FONT TEXTURE*/\
	color.a = max(color.r, max(color.g, color.b));\
	return (pixelOutput)(color);\
}\
";

	// compile shader to bytecode

	ID3DBlob* shaderCode = NULL;
	ID3DBlob* errors = NULL;
	// Compile effect 
    hr = ::D3DCompile(fullscreenQuadVSSource, strlen(fullscreenQuadVSSource),
        "FontUtilVS", NULL, NULL, "QuadVS", "vs_5_0",
#ifdef _DEBUG
		D3D10_SHADER_OPTIMIZATION_LEVEL0 | D3D10_SHADER_DEBUG,
#else
		D3D10_SHADER_OPTIMIZATION_LEVEL0, 
#endif
        0, &shaderCode, &errors);

    if (FAILED(hr))
    { 
		g2dDX11Global::PrintDXError(hr);
        if(errors) 
        { 
			const char* err_msg = reinterpret_cast<const char*>(errors->GetBufferPointer());
			DBG_ERROR(err_msg);
        } 
		// any specified exception to throw?
		throw;
		return;
    } 
 
	// create vtx shader from bytecode

	hr = ( g2dDX11Global::g_pDevice->CreateVertexShader( shaderCode->GetBufferPointer(),
		shaderCode->GetBufferSize(), NULL, &m_pQuadVS ) );
    if (FAILED(hr))
    { 
		g2dDX11Global::PrintDXError(hr);
        if(errors) 
        { 
			const char* err_msg = reinterpret_cast<const char*>(errors->GetBufferPointer());
			DBG_ERROR(err_msg);
        } 
		throw;
		return;
    } 

	// Create PS Shader
	// Compile effect 
	ID3DBlob* shaderCode2 = NULL;
    hr = ::D3DCompile(fullscreenQuadPSSource, strlen(fullscreenQuadPSSource),
        "FontUtilPS", NULL, NULL, "QuadPS", "ps_5_0",
#ifdef _DEBUG
		D3D10_SHADER_OPTIMIZATION_LEVEL0 | D3D10_SHADER_DEBUG,
#else
		D3D10_SHADER_OPTIMIZATION_LEVEL0, 
#endif
        0, &shaderCode2, &errors);

    if (FAILED(hr))
    { 
		g2dDX11Global::PrintDXError(hr);
        if(errors) 
        { 
			const char* err_msg = reinterpret_cast<const char*>(errors->GetBufferPointer());
			DBG_ERROR(err_msg);
        } 
		// any specified exception to throw?
		throw;
		return;
    } 
 
	// create vtx shader from bytecode

	hr = ( g2dDX11Global::g_pDevice->CreatePixelShader( shaderCode2->GetBufferPointer(),
		shaderCode2->GetBufferSize(), NULL, &m_pQuadPS ) );
    if (FAILED(hr))
    { 
		g2dDX11Global::PrintDXError(hr);
        if(errors) 
        { 
			const char* err_msg = reinterpret_cast<const char*>(errors->GetBufferPointer());
			DBG_ERROR(err_msg);
        } 
		throw;
		return;
    }

	// Create input layout from shader bytecode and input layout desc
	hr = ( g2dDX11Global::g_pDevice->CreateInputLayout( quadlayout, 3, 
		shaderCode->GetBufferPointer(), shaderCode->GetBufferSize(), &m_pQuadLayout ) );


    // Setup constant buffer
    D3D11_BUFFER_DESC Desc;
    Desc.Usage = D3D11_USAGE_DYNAMIC;
    Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    Desc.MiscFlags = 0;
    Desc.ByteWidth = sizeof( CB_PS_Color );
    hr = g2dDX11Global::g_pDevice->CreateBuffer( &Desc, NULL, &m_pcbPSColor );


	// done with the blob now
	shaderCode->Release( );
	shaderCode2->Release();
}

void g2dFontObjectDX11::CleanupDefaultShader()
{
	if (m_pcbPSColor)
	{
		m_pcbPSColor->Release( );
		m_pcbPSColor = NULL;
	}
	if (m_pQuadPS)
	{
		m_pQuadPS->Release( );
		m_pQuadPS = NULL;
	}
	if (m_pQuadVS)
	{
		m_pQuadVS->Release( );
		m_pQuadVS = NULL;
	}
	if (m_pQuadLayout)
	{
		m_pQuadLayout->Release( );
		m_pQuadLayout = NULL;
	}
}

//------------------------------------------------------------------------
//	g2dFontUtilDX11
//------------------------------------------------------------------------
g2dFontUtilDX11* g2dFontUtilDX11::Implementation()
{
	return dynamic_cast<g2dFontUtilDX11*>(g2dFontUtil::Implementation());
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
g2dFontUtilDX11::g2dFontUtilDX11()
{
	g2dResetHandler::AddResetHandler(new g2dFontReloader(this->m_Fonts));
}

g2dFontUtilDX11::~g2dFontUtilDX11()
{
}

//------------------------------------------------------------------------
//	LoadFont make the given font name available for use and returns
//	a g2dFontHandle which can be used later.  When you are done with the
//	font, call ReleaseFont.
//------------------------------------------------------------------------
g2dFontHandle g2dFontUtilDX11::LoadFont(const itString& i_Name, int i_Size)
{
#ifdef BITMAP_FONT_SPRITE
	fsLocator filePath = l_GloablFontMap;
	//fsFileUtil::UnicodeStringToLocator(i_Name, filePath);

	if (!fsFileUtil::FileExists(filePath))
		return NULL;

	bool bWasSkipTextures = matTextureMgr::IsSkipAllTextures();
	matTextureMgr::SetSkipAllTextures(false);
	matTexture* pTextFormat = matTextureMgr::LoadTexture(filePath);
	matTextureMgr::SetSkipAllTextures(bWasSkipTextures);

	if (!pTextFormat)
		throw g2dUnknownFontX(filePath.GetLastName());

	g2dFontObjectDX11* fontObj = new g2dFontObjectDX11(pTextFormat);
	fontObj->SetTextSize( (float)i_Size );

	g2dFontHandle ret_val = reinterpret_cast<g2dFontHandle>(fontObj);

	FontMapIt it = m_Fonts.find(ret_val);
	if( it != m_Fonts.end() )
	{
		//	increase ref count
		++(it->second.m_Reference);
	}
	else
	{
		//	make new ID3DXFONT and add
		FontInfo info;
		info.m_Reference = 1;
		info.m_Font = fontObj;

		m_Fonts[ret_val] = info;
	}

	return ret_val;

#else

	DBG_ASSERT(g2dDX11Global::g_pDevice, "D3D device must have been created before making a font");

	// should bury in itLocale!!!
	WCHAR localeName[LOCALE_NAME_MAX_LENGTH+1];
	int localeNameLength = ::GetUserDefaultLocaleName(
		localeName,
		LOCALE_NAME_MAX_LENGTH);


	IDWriteTextFormat* pTextFormat = NULL;
	HRESULT hr = g2dDX11Global::g_pDWriteFactory->CreateTextFormat(
            i_Name.GetString(),                // Font family name.
            NULL,                       // Font collection (NULL sets it to use the system font collection).
            DWRITE_FONT_WEIGHT_REGULAR,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            (FLOAT)i_Size,//72.0f,
            localeName,//L"en-us",
            &pTextFormat
            );
	if (FAILED(hr))
		throw g2dUnknownFontX(i_Name);

	g2dFontHandle ret_val = reinterpret_cast<g2dFontHandle>(pTextFormat);

	FontMapIt it = m_Fonts.find(ret_val);
	if( it != m_Fonts.end() )
	{
		//	increase ref count
		++(it->second.m_Reference);
	}
	else
	{
		//	make new ID3DXFONT and add
		FontInfo info;
		info.m_Reference = 1;
		info.m_Font = pTextFormat;
//#if 0
		// fill font info into D3DXFONT struct
		info.m_FontDesc.Height = i_Size;
		info.m_FontDesc.Width = 0;
		info.m_FontDesc.Weight = 0;
		info.m_FontDesc.MipLevels = 0;	// 0 - full mipmap chain, 1 - only one level 
		info.m_FontDesc.Italic = FALSE;
		info.m_FontDesc.CharSet = DEFAULT_CHARSET;
		info.m_FontDesc.OutputPrecision = OUT_OUTLINE_PRECIS;
		info.m_FontDesc.Quality = ANTIALIASED_QUALITY;
		info.m_FontDesc.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	
		// Set the FaceName from i_Name.
		// Copy no more than LF_FACESIZE characters, saving one for Null termination:
		int lenWithoutNull = min(LF_FACESIZE-1, i_Name.GetLength());
		//bga - The second argument is the size of the destination buffer for securtiy reasons,
		// not a request to transfer a limited number of characters
		//wcscpy_s(info.m_FontDesc.FaceName, lenWithoutNull, i_Name.GetString());
		wcscpy_s(info.m_FontDesc.FaceName, LF_FACESIZE, i_Name.GetString());
		// null term
		info.m_FontDesc.FaceName[lenWithoutNull] = 0;
	
		::D3DX11CreateFontIndirect(g2dDX11Global::g_pDevice, &info.m_FontDesc, &info.m_Font);
//#endif
		m_Fonts[ret_val] = info;
	}

	return ret_val;
#endif
}

//------------------------------------------------------------------------
//	ReleaseFont informs the system that you are done with the given
//	g2dFontHandle.
//------------------------------------------------------------------------
void g2dFontUtilDX11::ReleaseFont(g2dFontHandle i_Handle)
{
	if (reinterpret_cast<g2dFontObjectDX11*>(i_Handle) == NULL)
		return;

	FontMapIt it = m_Fonts.find(i_Handle);

	if( it != m_Fonts.end() )
	{
		--(it->second.m_Reference);
		if( it->second.m_Reference == 0 )
		{
#ifdef BITMAP_FONT_SPRITE
			delete it->second.m_Font;
			it->second.m_Font = NULL;
#else
			it->second.m_Font->Release();
#endif
			m_Fonts.erase(it);
		}
	}
	else
		DBG_ASSERT(false, "Unknown g2dFontHandle");
	
#ifdef BITMAP_FONT_SPRITE
	::DeleteObject(reinterpret_cast<matTexture*>(i_Handle));
#else
	::DeleteObject(reinterpret_cast<HFONT>(i_Handle));
#endif
}

//------------------------------------------------------------------------
// GetLoadedFontName returns the name of the font being used for the given
// g2dFontHandle.  It looks this up in the system to learn exactly what
// the system loaded.
//------------------------------------------------------------------------
void g2dFontUtilDX11::GetLoadedFontName( g2dFontHandle i_Font, itString& o_Name )
{
#ifdef BITMAP_FONT_SPRITE
	// does this function need to be implemented?
#else
	IDWriteTextFormat* pTextFormat = reinterpret_cast<IDWriteTextFormat*>(i_Font);

	UINT32 size = pTextFormat->GetFontFamilyNameLength();
	o_Name.SetLength(size);

	pTextFormat->GetFontFamilyName(&o_Name[0], size);
#endif
}

void g2dFontUtilDX11::GetLoadedFontName( g2dFontHandle i_Font, std::string& o_Name )
{
#ifdef BITMAP_FONT_SPRITE
#else
	itString temp;
	GetLoadedFontName( i_Font, temp );
	o_Name = itStringUtil::GetStdString(temp);
#endif
}

//------------------------------------------------------------------------
//	GetTextSizeInPixels returns the width and height in pixels of the given text and font
//------------------------------------------------------------------------
void g2dFontUtilDX11::GetTextSizeInPixels(g2dFontHandle i_Font, const itString& i_Text, int& o_Width, int& o_Height)
{
	//DBG_ASSERT(false, "g2dFontUtilDX11::GetTextSizeInPixels not implemented");
#ifdef BITMAP_FONT_SPRITE
	o_Width = 1;
	o_Height = 1;
#else
	IDWriteTextFormat* pTextFormat = reinterpret_cast<IDWriteTextFormat*>(i_Font);
	float fsize = pTextFormat->GetFontSize();

	// Create a DC render target.
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(
			DXGI_FORMAT_B8G8R8A8_UNORM,
			D2D1_ALPHA_MODE_IGNORE),
		0,
		0,
		D2D1_RENDER_TARGET_USAGE_NONE,
		D2D1_FEATURE_LEVEL_DEFAULT
		);
	ID2D1DCRenderTarget* m_pDCRT = NULL;
	HRESULT hr = m_pD2DFactory->CreateDCRenderTarget(&props, &m_pDCRT);


	HDC dc;
	dc = ::CreateCompatibleDC(NULL);
	hr = m_pDCRT->BindDC(dc, NULL);

	GetDpi

	HGDIOBJ old_obj = ::SelectObject(dc, reinterpret_cast<HFONT>(i_Font));
	DBG_ASSERT0(old_obj, "SelectObject failed");

	SIZE Size;
	GetTextExtentPoint32W(dc, i_Text.GetString(), i_Text.GetLength(), &Size);

	::SelectObject(dc, old_obj);
	::DeleteDC(dc);

	o_Width = Size.cx;
	o_Height = Size.cy;
#endif 
}

//------------------------------------------------------------------------
//	Get and set the global font path
//------------------------------------------------------------------------
void g2dFontUtilDX11::SetGlobalFontBitmap(const fsLocator& i_FontPath)
{
	l_GloablFontMap = i_FontPath;
}

void g2dFontUtilDX11::GetGlobalFontBitmap(fsLocator& o_FontPath)
{
	o_FontPath = l_GloablFontMap;
}

#if 0
//------------------------------------------------------------------------
//	GetD3DFont returns a ID3DXFont for the given font handle.
//------------------------------------------------------------------------
ID3DX11Font* g2dFontUtilDX11::GetD3DFont(g2dFontHandle i_Handle)
{
	FontMapIt it = m_Fonts.find(i_Handle);

	if( it != m_Fonts.end() )
		return it->second.m_Font;
	else
	{
		DBG_ASSERT0(false, "Unknown g2dFontHandle");
		return NULL;
	}
}
#endif

//#include c_g2dD3DX11_H
//#include "Core/ma/maFloatRGBA.hpp"
//#include "Core/ma/maVector3d.hpp"
//#include "Core/ma/maVector2d.hpp"

#if defined(DEBUG) || defined(_DEBUG)
#ifndef V
#define V(x)           { hr = (x); if( FAILED(hr) ) { DXUTTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true ); } }
#endif
#ifndef V_RETURN
#define V_RETURN(x)    { hr = (x); if( FAILED(hr) ) { return hr; } }
#endif
#else
#ifndef V
#define V(x)           { hr = (x); }
#endif
#ifndef V_RETURN
#define V_RETURN(x)    { hr = (x); if( FAILED(hr) ) { return hr; } }
#endif
#endif

struct DXUTSpriteVertex
{
	maVector3d vPos;
    //D3DXVECTOR3 vPos;
	maFloatRGBA vColor;
    //D3DXCOLOR vColor;
    maVector2d vTex;
	//D3DXVECTOR2 vTex;
};
ID3D11Buffer* g_pFontBuffer11 = NULL;
UINT g_FontBufferBytes11 = 0;
std::vector<DXUTSpriteVertex> g_FontVertices;
ID3D11ShaderResourceView* g_pFont11 = NULL;
ID3D11InputLayout* g_pInputLayout11 = NULL;
/*
HRESULT InitFont11( ID3D11Device* pd3d11Device, ID3D11InputLayout* pInputLayout )
{
    HRESULT hr = S_OK;
    
	V_RETURN( D3DX11CreateShaderResourceViewFromFile( pd3d11Device, 
		L"c:\\projects\\sourcecode\\libxlt\\graphicsdx11\\g2d\\private\\Font.dds", 
		NULL, NULL, &g_pFont11, &hr) );

	g_pInputLayout11 = pInputLayout;
    return hr;
}
*/
void EndFont11()
{
    SAFE_RELEASE( g_pFontBuffer11 );
    g_FontBufferBytes11 = 0;
    SAFE_RELEASE( g_pFont11 );
}
void EndText11( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3d11DeviceContext );

void BeginText11()
{
    g_FontVertices.clear();
}

void DrawText11DXUT( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3d11DeviceContext,
                 LPCWSTR strText, RECT rcScreen, const maFloatRGBA& vFontColor,
                 float fBBWidth, float fBBHeight, bool bCenter )
{
    float fCharTexSizeX = 0.010526315f;
    //float fGlyphSizeX = 14.0f / fBBWidth;
    //float fGlyphSizeY = 32.0f / fBBHeight;
    float fGlyphSizeX = 15.0f / fBBWidth;
    float fGlyphSizeY = 42.0f / fBBHeight;


    float fRectLeft = rcScreen.left / fBBWidth;
    float fRectTop = 1.0f - rcScreen.top / fBBHeight;

    fRectLeft = fRectLeft * 2.0f - 1.0f;
    fRectTop = fRectTop * 2.0f - 1.0f;

    int NumChars = (int)wcslen( strText );
    if (bCenter) {
        float fRectRight = rcScreen.right / fBBWidth;
        fRectRight = fRectRight * 2.0f - 1.0f;
        float fRectBottom = 1.0f - rcScreen.bottom / fBBHeight;
        fRectBottom = fRectBottom * 2.0f - 1.0f;
        float fcenterx = ((fRectRight - fRectLeft) - (float)NumChars*fGlyphSizeX) *0.5f;
        float fcentery = ((fRectTop - fRectBottom) - (float)1*fGlyphSizeY) *0.5f;
        fRectLeft += fcenterx ;    
        fRectTop -= fcentery;
    }
    float fOriginalLeft = fRectLeft;
    float fTexTop = 0.0f;
    float fTexBottom = 1.0f;

    float fDepth = 0.5f;
    for( int i=0; i<NumChars; i++ )
    {
        if( strText[i] == '\n' )
        {
            fRectLeft = fOriginalLeft;
            fRectTop -= fGlyphSizeY;

            continue;
        }
        else if( strText[i] < 32 || strText[i] > 126 )
        {
            continue;
        }

        // Add 6 sprite vertices
        DXUTSpriteVertex SpriteVertex;
        float fRectRight = fRectLeft + fGlyphSizeX;
        float fRectBottom = fRectTop - fGlyphSizeY;
        float fTexLeft = ( strText[i] - 32 ) * fCharTexSizeX;
        float fTexRight = fTexLeft + fCharTexSizeX;

        // tri1
        SpriteVertex.vPos = maVector3d( fRectLeft, fRectTop, fDepth );
        SpriteVertex.vTex = maVector2d( fTexLeft, fTexTop );
        SpriteVertex.vColor = vFontColor;
        g_FontVertices.push_back( SpriteVertex );

        SpriteVertex.vPos = maVector3d( fRectRight, fRectTop, fDepth );
        SpriteVertex.vTex = maVector2d( fTexRight, fTexTop );
        SpriteVertex.vColor = vFontColor;
        g_FontVertices.push_back( SpriteVertex );

        SpriteVertex.vPos = maVector3d( fRectLeft, fRectBottom, fDepth );
        SpriteVertex.vTex = maVector2d( fTexLeft, fTexBottom );
        SpriteVertex.vColor = vFontColor;
        g_FontVertices.push_back( SpriteVertex );

        // tri2
        SpriteVertex.vPos = maVector3d( fRectRight, fRectTop, fDepth );
        SpriteVertex.vTex = maVector2d( fTexRight, fTexTop );
        SpriteVertex.vColor = vFontColor;
        g_FontVertices.push_back( SpriteVertex );

        SpriteVertex.vPos = maVector3d( fRectRight, fRectBottom, fDepth );
        SpriteVertex.vTex = maVector2d( fTexRight, fTexBottom );
        SpriteVertex.vColor = vFontColor;
        g_FontVertices.push_back( SpriteVertex );

        SpriteVertex.vPos = maVector3d( fRectLeft, fRectBottom, fDepth );
        SpriteVertex.vTex = maVector2d( fTexLeft, fTexBottom );
        SpriteVertex.vColor = vFontColor;
        g_FontVertices.push_back( SpriteVertex );

        fRectLeft += fGlyphSizeX;

    }

    // TODO: We have to end text after every line so that rendering order between sprites and fonts is preserved
    EndText11( pd3dDevice, pd3d11DeviceContext );
}

void EndText11( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3d11DeviceContext )
{

    // ensure our buffer size can hold our sprites
    UINT FontDataBytes = g_FontVertices.size() * sizeof( DXUTSpriteVertex );
    if( g_FontBufferBytes11 < FontDataBytes )
    {
        SAFE_RELEASE( g_pFontBuffer11 );
        g_FontBufferBytes11 = FontDataBytes;

        D3D11_BUFFER_DESC BufferDesc;
        BufferDesc.ByteWidth = g_FontBufferBytes11;
        BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        BufferDesc.MiscFlags = 0;

        pd3dDevice->CreateBuffer( &BufferDesc, NULL, &g_pFontBuffer11 );
    }

    // Copy the sprites over
    D3D11_BOX destRegion;
    destRegion.left = 0;
    destRegion.right = FontDataBytes;
    destRegion.top = 0;
    destRegion.bottom = 1;
    destRegion.front = 0;
    destRegion.back = 1;
    D3D11_MAPPED_SUBRESOURCE MappedResource;
    if ( S_OK == pd3d11DeviceContext->Map( g_pFontBuffer11, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource ) ) { 
        CopyMemory( MappedResource.pData, (void*)(&g_FontVertices[0]), FontDataBytes );
        pd3d11DeviceContext->Unmap(g_pFontBuffer11, 0);
    }

    ID3D11ShaderResourceView* pOldTexture = NULL;
    pd3d11DeviceContext->PSGetShaderResources( 0, 1, &pOldTexture );
    pd3d11DeviceContext->PSSetShaderResources( 0, 1, &g_pFont11 );

    // Draw
    UINT Stride = sizeof( DXUTSpriteVertex );
    UINT Offset = 0;
    pd3d11DeviceContext->IASetVertexBuffers( 0, 1, &g_pFontBuffer11, &Stride, &Offset );
    pd3d11DeviceContext->IASetInputLayout( g_pInputLayout11 );
    pd3d11DeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    pd3d11DeviceContext->Draw( g_FontVertices.size(), 0 );

    pd3d11DeviceContext->PSSetShaderResources( 0, 1, &pOldTexture );
    SAFE_RELEASE( pOldTexture );

    g_FontVertices.clear();
}

//--------------------------------------------------------------------------------------
HRESULT DrawText11( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3d11DeviceContext,
				   LPCWSTR strText, const maFloatRGBA& pElement, RECT* prcDest, bool bShadow, int nCount, bool bCenter  )
{
    //HRESULT hr = S_OK;

    // No need to draw fully transparent layers
    if( pElement.GetAlpha() == 0 )
        return S_OK;

	static const int windowPosX=0, windowPosY=0;
	static const int windowSizeX=0, windowSizeY=0;
    RECT rcScreen = *prcDest;
    OffsetRect( &rcScreen, windowPosX, windowPosY);

    // If caption is enabled, offset the Y position by its height.
//    if( m_bCaption )
  //      OffsetRect( &rcScreen, 0, m_nCaptionHeight );

    float fBBWidth = ( float )windowSizeX;//m_pManager->m_nBackBufferWidth;
    float fBBHeight = ( float )windowSizeY;//m_pManager->m_nBackBufferHeight;

    if( bShadow )
    {
        RECT rcShadow = rcScreen;
        OffsetRect( &rcShadow, 1, 1 );

        maFloatRGBA vShadowColor( 0,0,0, 1.0f );
        DrawText11DXUT( pd3dDevice, pd3d11DeviceContext,
                 strText, rcShadow, vShadowColor,
                 fBBWidth, fBBHeight, bCenter );

    }

    DrawText11DXUT( pd3dDevice, pd3d11DeviceContext,
             strText, rcScreen, pElement,
             fBBWidth, fBBHeight, bCenter );

    return S_OK;
}
