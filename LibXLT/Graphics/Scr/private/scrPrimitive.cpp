/*****************************************************************************
**	scrPrimitive.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/scr/scrPrimitive.hpp"


//--------------------------------------------------------------------
// Construction
//--------------------------------------------------------------------
scrPrimitive::scrPrimitive()
:	m_bRenderable(true),
	m_Alpha(1.0f),
	m_Position(0.0f, 0.0f, 0.0f),
	m_Width(0),
	m_Height(0),
	m_EmissiveColor(1.0f,0,0,1.0f),
	m_DiffuseColor(0.0f,0.0f,0.0f,1.0f)
{
}

//--------------------------------------------------------------------
// Destruction
//--------------------------------------------------------------------
scrPrimitive::~scrPrimitive()
{
}

//--------------------------------------------------------------------
//	GetRenderable returns whether this is currently rendered
//--------------------------------------------------------------------
bool scrPrimitive::GetRenderable() const
{
	return m_bRenderable;
}

//--------------------------------------------------------------------
//	SetRenderable sets whether this is currently rendered
//--------------------------------------------------------------------
void scrPrimitive::SetRenderable(bool i_bRenderable)
{
	m_bRenderable = i_bRenderable;
}

//--------------------------------------------------------------------
//	GetAlpha returns the current alpha value
//--------------------------------------------------------------------
float scrPrimitive::GetAlpha() const
{
	return m_Alpha;
}

//--------------------------------------------------------------------
//	SetAlpha sets the current alpha value
//--------------------------------------------------------------------
void scrPrimitive::SetAlpha(float i_Alpha)
{
	m_Alpha = i_Alpha;
}

//--------------------------------------------------------------------
//	GetPosition returns the current screen coordinates
//--------------------------------------------------------------------
const maPoint3d& scrPrimitive::GetPosition() const
{
	return m_Position;
}

//--------------------------------------------------------------------
//	SetPosition sets the current screen coordinates
//--------------------------------------------------------------------
void scrPrimitive::SetPosition(const maPoint3d& i_Position)
{
	m_Position = i_Position;
}

//--------------------------------------------------------------------
//	GetSize returns the current height and width
//--------------------------------------------------------------------
void scrPrimitive::GetSize(int& o_Width, int& o_Height) const
{
	o_Width = m_Width;
	o_Height = m_Height;
}

//--------------------------------------------------------------------
//	SetSize sets the current height and width
//--------------------------------------------------------------------
void scrPrimitive::SetSize(int i_Width, int i_Height)
{
	m_Width = i_Width;
	m_Height = i_Height;
}

//--------------------------------------------------------------------
//	SetEmissive sets the current emissive color value
//--------------------------------------------------------------------
void scrPrimitive::SetEmissive(const maFloatRGBA& i_Color)
{
	m_EmissiveColor = i_Color;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
maFloatRGBA& scrPrimitive::GetEmissive()
{
	return m_EmissiveColor;
}

//--------------------------------------------------------------------
//	SetDiffuse sets the current Diffuse color value
//--------------------------------------------------------------------
void scrPrimitive::SetDiffuse(const maFloatRGBA& i_Color)
{
	m_DiffuseColor = i_Color;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
maFloatRGBA& scrPrimitive::GetDiffuse()
{
	return m_DiffuseColor;
}

//--------------------------------------------------------------------
//	SetImage passes an fsLocator to the Image to use
//--------------------------------------------------------------------
//virtual 
void scrPrimitive::SetImage(const fsLocator& i_Locator)
{
	//m_pImpl->SetImage( i_Locator );
}

