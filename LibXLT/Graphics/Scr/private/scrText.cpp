/*****************************************************************************
**	scrText.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/scr/scrText.hpp"

#include "Core/it/itStringUtil.hpp"
#include "Graphics/eff/effTexturedData.hpp"
#include "Graphics/g2d/g2dScreenUtil.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dFragmentCreate.hpp"
#include "Graphics/g3d/g3dPackage.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"


//============================================================================
//============================================================================
namespace
{
	int lc_NumHorizontalChars = 16;
	int lc_NumVerticalChars = 8;
}


//--------------------------------------------------------------------
// Construction
//--------------------------------------------------------------------
scrText::scrText()
:	m_bCentered(false),
	m_CenterOffsetX(0),
	m_FragmentLengthInChar(0),
	m_ForegroundColor(1,0,0,0),
	m_BackgroundColor(0,0,0,1),
	m_bScreenBased(false),
	m_CharSize(1.0f)
{
}


//--------------------------------------------------------------------
// Destruction
//--------------------------------------------------------------------
scrText::~scrText()
{
	free_geometry();
	free_texture();
	free_model();
}

//--------------------------------------------------------------------
//	SetSize sets the current height and width
//--------------------------------------------------------------------
void scrText::SetSize(int i_Width, int i_Height)
{
	scrPrimitive::SetSize(i_Width, i_Height);

	center_text();
	update_transform();
}

//--------------------------------------------------------------------
//	GetText returns the text string to be displayed
//--------------------------------------------------------------------
const itString& scrText::GetText() const
{
	return m_Text;
}

//--------------------------------------------------------------------
//	SetText sets the text string to be displayed
//--------------------------------------------------------------------
void scrText::SetText(const itString& i_Text)
{
	if (m_Text == i_Text)
	{
		return;
	}

	m_Text = i_Text;

	//DBG_LOG( "setting string (" << itStringUtil::GetStdString( m_Text ).c_str() << ")" );

	//if (m_pFragment && m_Text.GetLength() <= m_FragmentLengthInChar)
	//{
	//	remap_geometry_uvs();
	//}
	//else
	{
		make_geometry();
	}
}

//--------------------------------------------------------------------
//	SetFontMapImage passes an fsLocator to the Image to be used as a
//	font map
//--------------------------------------------------------------------
void scrText::SetFontMapImage(const fsLocator& i_Locator, int i_Width/* = 0*/, int i_Height/* = 0*/)
{
	//delete any old objects
	free_texture();
	free_geometry();

	// The Font has to be loaded, so mess with the 
	// SkipAllTextures flag, if needed.
	bool bWasSkipTextures = matTextureMgr::IsSkipAllTextures();
	matTextureMgr::SetSkipAllTextures(false);
	m_pTexture = matTextureMgr::LoadTexture(i_Locator);
	matTextureMgr::SetSkipAllTextures(bWasSkipTextures);

	if (m_pTexture == NULL)
		return;

	effTexturedData* d = m_pMaterial->TypedData<effTexturedData>();
	if (d!=NULL) {
		d->m_Texture = m_pTexture;
	}

	if (!i_Width)
	{
		m_CharWidth = m_pTexture->GetWidth() / lc_NumHorizontalChars;
		m_CharHeight = m_pTexture->GetHeight() / lc_NumVerticalChars;
		m_UVWidth	= 1.0f / static_cast<float>(lc_NumHorizontalChars);
		m_UVHeight	= 1.0f / static_cast<float>(lc_NumVerticalChars);
	}
	else
	{
		m_CharWidth		= i_Width / lc_NumHorizontalChars;
		m_CharHeight	= i_Height / lc_NumVerticalChars;
		m_UVWidth	= (static_cast<float>(i_Width) / static_cast<float>(m_pTexture->GetWidth())) /
						static_cast<float>(lc_NumHorizontalChars);
		m_UVHeight	= (static_cast<float>(i_Height) / static_cast<float>(m_pTexture->GetHeight())) /
						static_cast<float>(lc_NumVerticalChars);
	}

	//DBG_LOG2( "scrText::SFMI w(%d) h(%d)", i_Width, i_Height );
	//DBG_LOG2( "scrText::SFMI hc(%d) vc(%d)", lc_NumHorizontalChars, lc_NumVerticalChars );
	//DBG_LOG2( "scrText::SFMI cw(%d) ch(%d)", m_CharWidth, m_CharHeight );
	//DBG_LOG2( "scrText::SFMI uvw(%6.2f) uvh(%6.2f)", m_UVWidth, m_UVHeight );

	SetText(GetText());
}

//--------------------------------------------------------------------
//	SetFontMapImage passes a matTexture to be used as a font map
//--------------------------------------------------------------------
void scrText::SetFontMapImage(matTexture* i_FontMap, int i_Width, int i_Height)
{
	//delete any old objects
	free_texture();
	free_geometry();

	m_pTexture = i_FontMap;

	m_pMaterial->TypedData<effTexturedData>()->m_Texture = m_pTexture;

	if (!m_pTexture)
		return;

	if (!i_Width)
	{
		m_CharWidth = m_pTexture->GetWidth() / lc_NumHorizontalChars;
		m_CharHeight = m_pTexture->GetHeight() / lc_NumVerticalChars;
		m_UVWidth	= 1.0f / static_cast<float>(lc_NumHorizontalChars);
		m_UVHeight	= 1.0f / static_cast<float>(lc_NumVerticalChars);
	}
	else
	{
		m_CharWidth		= i_Width / lc_NumHorizontalChars;
		m_CharHeight	= i_Height / lc_NumVerticalChars;
		m_UVWidth	= (static_cast<float>(i_Width) / static_cast<float>(m_pTexture->GetWidth())) /
						static_cast<float>(lc_NumHorizontalChars);
		m_UVHeight	= (static_cast<float>(i_Height) / static_cast<float>(m_pTexture->GetHeight())) /
						static_cast<float>(lc_NumVerticalChars);
	}

	SetText(GetText());
}

//--------------------------------------------------------------------
//	SetCentered sets whether the text should be centered around its
//	position
//--------------------------------------------------------------------
void scrText::SetCentered(bool i_bCentered)
{
	m_bCentered = i_bCentered;
	if (!m_bCentered)
	{
		m_CenterOffsetX = 0;
	}
	else if (m_pTexture && m_Text.GetLength())
	{
		center_text();
		update_transform();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void scrText::SetForegroundColor( const maFloatRGBA& i_Color )
{
	m_ForegroundColor = i_Color;

	SetEmissive( i_Color );
}
const maFloatRGBA& scrText::GetForegroundColor() const
{
	return m_ForegroundColor;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void scrText::SetBackgroundColor( const maFloatRGBA& i_Color )
{
	m_BackgroundColor = i_Color;

//	SetDiffuse( i_Color );
//	SetEmissive( i_Color );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void scrText::SetScreenBased(bool i_bScreenBased)
{
	m_bScreenBased = i_bScreenBased;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void scrText::SetTextSize(float i_Size)
{
	m_CharSize = i_Size;
}

//--------------------------------------------------------------------
//	make_geometry handles initial creation of the geomtry, should be
//	called when the new text length exceeds m_FragmentLengthInChar
//--------------------------------------------------------------------
void scrText::make_geometry()
{
	//delete any old objects
	free_geometry();

	int i, text_len = m_Text.GetLength();
	m_FragmentLengthInChar = text_len;

	if (!m_pTexture || !text_len)
	{
		return;
	}

	char ch;
	float start_uvX, start_uvY;
	maPoint3d *vertices = new maPoint3d[text_len * 4];
	maPoint3d *normals = new maPoint3d[text_len * 4];
	maPoint2d *textureVertices = new maPoint2d[text_len * 4];
	unsigned short *indices = new unsigned short[text_len * 6];
	for (i = 0; i < text_len; ++i)
	{
		ch = m_Text[i];
		start_uvX = ch % lc_NumHorizontalChars * m_UVWidth;
		start_uvY = ch / lc_NumHorizontalChars * m_UVHeight;

		//DBG_LOG4( "%02d) scrText::MG ch(%c) w(%d) h(%d)", i, ch, start_uvX, start_uvY );

		if (!m_bScreenBased)
		{
			vertices[(i * 4) + 0].Set( i,			0.0f, 0.0f );
			vertices[(i * 4) + 1].Set( i + 1.0f,	0.0f, 0.0f );
			vertices[(i * 4) + 2].Set( i,			-1.0f, 0.0f );
			vertices[(i * 4) + 3].Set( i + 1.0f,	-1.0f, 0.0f );
		}
		else
		{
			float widthScale = 1.5;
			vertices[(i * 4) + 0].Set( -1.0f + i * (m_UVWidth * widthScale) * m_CharSize, 1.0f, 0.5f );
			vertices[(i * 4) + 1].Set( -1.0f + (i+1) * (m_UVWidth * widthScale) * m_CharSize, 1.0f, 0.5f );
			vertices[(i * 4) + 2].Set( -1.0f + i * (m_UVWidth * widthScale) * m_CharSize, 1.0f - m_UVHeight * m_CharSize, 0.5f );
			vertices[(i * 4) + 3].Set( -1.0f + (i+1) * (m_UVWidth * widthScale) * m_CharSize, 1.0f - m_UVHeight * m_CharSize, 0.5f );
		}

		normals[(i * 4) + 0].Set( 0, 0, 1 );
		normals[(i * 4) + 1].Set( 0, 0, 1 );
		normals[(i * 4) + 2].Set( 0, 0, 1 );
		normals[(i * 4) + 3].Set( 0, 0, 1 );

		textureVertices[(i * 4) + 0].Set(  start_uvX,				start_uvY );
		textureVertices[(i * 4) + 1].Set(  start_uvX + m_UVWidth,	start_uvY );
		textureVertices[(i * 4) + 2].Set(  start_uvX,				start_uvY + m_UVHeight );
		textureVertices[(i * 4) + 3].Set(  start_uvX + m_UVWidth,	start_uvY + m_UVHeight );

		indices[(i * 6) + 0] = (i * 4) + 2;
		indices[(i * 6) + 1] = (i * 4) + 1;
		indices[(i * 6) + 2] = (i * 4);
		indices[(i * 6) + 3] = (i * 4) + 2;
		indices[(i * 6) + 4] = (i * 4) + 3;
		indices[(i * 6) + 5] = (i * 4) + 1;
	}

	m_pMaterial->TypedData<effTexturedData>()->m_Color = this->GetEmissive();
//	m_pMaterial->TypedData<effTexturedData>()->m_ColorEmissive = this->GetEmissive();
//	m_pMaterial->TypedData<effTexturedData>()->m_Transparency = this->GetDiffuse().GetAlpha();

	m_pFragment = g3dFragmentCreate::CreateFragment(vertices,
													normals,
													textureVertices,
													text_len * 4,
													indices,
													text_len * 6,
													m_pMaterial,
													false );
	delete[] vertices;
	delete[] normals;
	delete[] textureVertices;
	delete[] indices;

	m_pNode->SetFragment( m_pFragment );

	//size the model to the size of texture if a size has yet to be set
	int width, height;
	GetSize(width, height);
	if (!width)
	{
		width = m_CharWidth;
		height = m_CharHeight;
	}
	SetSize(width, height);

	if (!m_bScreenBased)
		SetPosition(GetPosition());
}

//--------------------------------------------------------------------
//	remap_geometry_uvs is called when new text length is equal to or
//	less than m_FragmentLengthInChar, handles remapping the UVs of
//	the geometry, which lets us avoid completely remaking the geometry
//--------------------------------------------------------------------
void scrText::remap_geometry_uvs()
{
	//unsigned char* buffer = m_pFragment->Lock();

	//g3dType::Tex1Vertex *pVertices, *pVertex;
	//pVertices = reinterpret_cast<g3dType::Tex1Vertex*>(buffer);

	//int i, text_len = m_Text.GetLength();
	//float start_uvX, start_uvY;
	//char ch;
	//pVertex = pVertices;
	//for (i = 0; i < text_len; ++i)
	//{
	//	ch = m_Text[i];
	//	start_uvX = ch % lc_NumHorizontalChars * m_UVWidth;
	//	start_uvY = ch / lc_NumHorizontalChars * m_UVHeight;

	//	pVertex->m_TexCoord.Set(  start_uvX,				start_uvY );
	//	++pVertex;
	//	pVertex->m_TexCoord.Set(  start_uvX + m_UVWidth,	start_uvY );
	//	++pVertex;
	//	pVertex->m_TexCoord.Set(  start_uvX,				start_uvY + m_UVHeight );
	//	++pVertex;
	//	pVertex->m_TexCoord.Set(  start_uvX + m_UVWidth,	start_uvY + m_UVHeight );
	//	++pVertex;
	//}

	//for (i; i < m_FragmentLengthInChar; ++i)
	//{
	//	pVertex->m_TexCoord.Set(0, 0);
	//	++pVertex;
	//	pVertex->m_TexCoord.Set(0, 0);
	//	++pVertex;
	//	pVertex->m_TexCoord.Set(0, 0);
	//	++pVertex;
	//	pVertex->m_TexCoord.Set(0, 0);
	//	++pVertex;
	//}

	//m_pFragment->Unlock();

	//center_text();
	//update_transform();
}

//--------------------------------------------------------------------
//	update_transform
//--------------------------------------------------------------------
void scrText::update_transform()
{
	if (m_bScreenBased)
		return;
	//	make a matrix to apply the model transformation
	//	transformation order is scale, rotate, translate;
	//but first convert this virtual position into a screen position
	int nX, nY, width, height, char_width = m_CharWidth, char_height = m_CharHeight;
	if (m_pWindow)
		m_pWindow->GetVirtualResolution(nX, nY);
	GetSize(width, height);
	if (width != char_width)
	{
		char_width = width;
		char_height = height;
	}
	maPoint3d pos = GetPosition();

	maMatrix4x4 transform = m_pNode->GetTransform();
	transform.MakeScale((2.0f * char_width + 1.0f) / nX, (2.0f * char_height + 1.0f) / nY, 1);
	transform.TranslateBy((pos.m_X - m_CenterOffsetX), pos.m_Y, pos.m_Z);
	transform = g2dScreenUtil::WindowToScreenMatrix(*m_pWindow,transform);
	if (!m_bInScreenSpace)
	{
		transform.m_Mat[12] += 1.0f;
		transform.m_Mat[13] -= 1.0f;
	}
	m_pNode->SetTransform( transform );
}

//--------------------------------------------------------------------
//	center_text will determine the offset needed for the current text
//	and size combination, will return immediately if not currently
//	centered
//--------------------------------------------------------------------
void scrText::center_text()
{
	if (!m_bCentered || !m_pTexture || !m_Text.GetLength())
	{
		m_CenterOffsetX = 0;
		return;
	}

	int width, height;
	GetSize(width, height);
	m_CenterOffsetX = (static_cast<float>(m_Text.GetLength()) / 2.0f) * width;
}
