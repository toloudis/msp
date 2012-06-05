/*****************************************************************************
**  matTexture.cpp
**
**      matTexture is the base class for the different texture types
**	available in the mat package.  It supplies only the functions common to
**	all types of textures.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mat/matTexture.hpp"


//----------------------------------------------------------------------------
//	Default constructor
//----------------------------------------------------------------------------
matTexture::matTexture()
:	m_Width(0),
	m_Height(0),
	m_TransparencyMap(NULL)
{
}

//----------------------------------------------------------------------------
//	pure virtual destructor
//----------------------------------------------------------------------------
matTexture::~matTexture()
{
	if (m_TransparencyMap)
	{
		delete m_TransparencyMap;
		m_TransparencyMap = NULL;
	}
}

//----------------------------------------------------------------------------
//	GetSize returns approximate amount of memory (in kbytes) being
// used by this texture
//----------------------------------------------------------------------------
float matTexture::GetSize() const
{
	return ((float)this->GetHeight() * (float)this->GetWidth()  *
		(this->GetPixelFormat().BitsPerPixel())/(1024.0f*8) ); // convert to kbytes)
}


//--------------------------------------------------------------------
// Return an object pointer to use for rendering to this texture.
//	Only certain texture types can return this object, most will
//	return NULL.  The object pointed to will be owned by this 
//	texture, it should not be deleted by the user.
//--------------------------------------------------------------------
//virtual 
g2dRenderTarget* matTexture::GetRenderTargetAPI()
{
	return NULL;
}

//----------------------------------------------------------------------------
//	SetWidth is used by child classes to set the width of the
//	texture.
//----------------------------------------------------------------------------
void matTexture::SetWidth(int i_Width)
{
	m_Width = i_Width;
}

//----------------------------------------------------------------------------
//	SetHeight is used by child classes to set the width of the
//	texture.
//----------------------------------------------------------------------------
void matTexture::SetHeight(int i_Height)
{
	m_Height = i_Height;
}

//----------------------------------------------------------------------------
//	SetPixelFormat is used by child classes to set the pixel format
//	of the texture.
//----------------------------------------------------------------------------
void matTexture::SetPixelFormat(const g2dPFD& i_PFD)
{
	m_PFD = i_PFD;
}

//--------------------------------------------------------------------
// SetFileName() - Keep a copy of the map path
//--------------------------------------------------------------------
//void matTexture::SetFileName(const itString & i_FileName)
//{
//	m_FileName = i_FileName;
//}

//--------------------------------------------------------------------
// GetFileName() - Return a copy of the map path
//--------------------------------------------------------------------
//itString matTexture::GetFileName()
//{
//	return m_FileName;
//}

//--------------------------------------------------------------------
//	MakeTransparencyMap will build a transparency map for this texture
//	i_Bias specifies the range (0-i_Bias) that will be considered transparent
//--------------------------------------------------------------------
void matTexture::MakeTransparencyMap(float i_Bias)
{
	if (!HasTransparency())
	{
		return;
	}

	if (m_TransparencyMap)
	{
		delete m_TransparencyMap;
		m_TransparencyMap = NULL;
	}

	m_TransparencyMap = this->MakeTransparencyMap(i_Bias, GetPixelFormat(), GetWidth(), GetHeight());
}
