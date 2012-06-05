/*****************************************************************************
**  matTexture.hpp
**
**      matTexture is the base class for the different texture types
**	available in the mat package.  It supplies only the functions common to
**	all types of textures.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_TEXTURE_HPP
#error matTexture.hpp multiply included
#endif
#define MAT_TEXTURE_HPP

#ifndef G2D_PFD_HPP
#include "Graphics/g2d/g2dPFD.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#ifndef IT_STRING_HPP
#include "Core/It/itString.hpp"
#endif

class g2dRenderTarget;

class matTexture
{
	public:

		//--------------------------------------------------------------------
		//	Default constructor
		//--------------------------------------------------------------------
		matTexture();

		//--------------------------------------------------------------------
		//	pure virtual destructor
		//--------------------------------------------------------------------
		virtual ~matTexture() = 0;

		//--------------------------------------------------------------------
		//	GetWidth returns the width of the texture (if this is a mip-map
		//	texture, this is the width of the top level).
		//--------------------------------------------------------------------
		inline int GetWidth() const;

		//--------------------------------------------------------------------
		//	GetHeight returns the height of the texture (if this is a mip-map
		//	texture, this is the height of the top level).
		//--------------------------------------------------------------------
		inline int GetHeight() const;

		//--------------------------------------------------------------------
		//	GetPixelFormat returns the pixel format descriptor for the
		//	texture.  Even textures that have multiple surfaces like UVAS or
		//	mip maps must have the same pixel format for all surfaces.
		//--------------------------------------------------------------------
		inline const g2dPFD& GetPixelFormat() const;

		//----------------------------------------------------------------------------
		//	GetSize returns approximate amount of memory (in kbytes) being
		// used by this texture
		//----------------------------------------------------------------------------
		virtual float GetSize() const;

		//--------------------------------------------------------------------
		//	HasTransparency returns true if the texture has any transparent
		//	parts (and therefore needs to be sorted differently)
		//--------------------------------------------------------------------
		inline bool HasTransparency() const;

		//--------------------------------------------------------------------
		//	MakeTransparencyMap will build a transparency map for this texture
		//	i_Bias specifies the range (0-i_Bias) that will be considered transparent
		//--------------------------------------------------------------------
		void MakeTransparencyMap(float i_Bias);

		//--------------------------------------------------------------------
		//	RemoveTransparencyMap removes the transparencymap
		//--------------------------------------------------------------------
		inline void RemoveTransparencyMap();

		//--------------------------------------------------------------------
		//	HasTransparencyMap returns whether there is a transparency map for
		//	this texture
		//--------------------------------------------------------------------
		inline bool HasTransparencyMap() const;

		//--------------------------------------------------------------------
		//	IsPointTransparent returns true if the given (x,y) is transparent,
		//	be sure to convert to texel coordinates before querying this function
		//	will always return false if no there is no transparencymap, or if the
		//	point is out of bounds of the texture
		//--------------------------------------------------------------------
		inline bool IsPointTransparent(int i_x, int i_y) const;

		//--------------------------------------------------------------------
		// Return an object pointer to use for rendering to this texture.
		//	Only certain texture types can return this object, most will
		//	return NULL.  The object pointed to will be owned by this 
		//	texture, it should not be deleted by the user.
		//--------------------------------------------------------------------
		virtual g2dRenderTarget* GetRenderTargetAPI();
		
		//--------------------------------------------------------------------
		// SetFileName() - Keep a copy of the map path
		//--------------------------------------------------------------------
		// void SetFileName(const itString & i_FileName);

		//--------------------------------------------------------------------
		// GetFileName() - Return a copy of the map path
		//--------------------------------------------------------------------
		// itString GetFileName();

	protected:

		//--------------------------------------------------------------------
		//	SetWidth is used by child classes to set the width of the
		//	texture.
		//--------------------------------------------------------------------
		void SetWidth(int i_Width);

		//--------------------------------------------------------------------
		//	SetHeight is used by child classes to set the width of the
		//	texture.
		//--------------------------------------------------------------------
		void SetHeight(int i_Height);

		//--------------------------------------------------------------------
		//	SetPixelFormat is used by child classes to set the pixel format
		//	of the texture.
		//--------------------------------------------------------------------
		void SetPixelFormat(const g2dPFD& i_PFD);

		//--------------------------------------------------------------------
		//	MakeTransparencyMap will build a transparency map for this texture
		//	i_Bias specifies the range (0-i_Bias) that will be considered transparent
		//--------------------------------------------------------------------
		virtual char *MakeTransparencyMap(float i_Bias, const g2dPFD& i_PFD, int i_Width, int i_Height) = 0;

	private:

		short m_Width, m_Height;
		g2dPFD m_PFD;
		char *m_TransparencyMap;
};

//----------------------------------------------------------------------------
//	some matTexture implemenation
//----------------------------------------------------------------------------
//--------------------------------------------------------------------
//	GetWidth returns the width of the texture (if this is a mip-map
//	texture, this is the width of the top level).
//--------------------------------------------------------------------
inline int matTexture::GetWidth() const
{
	return m_Width;
}

//--------------------------------------------------------------------
//	GetHeight returns the height of the texture (if this is a mip-map
//	texture, this is the height of the top level).
//--------------------------------------------------------------------
inline int matTexture::GetHeight() const
{
	return m_Height;
}

//--------------------------------------------------------------------
//	GetPixelFormat returns the pixel format descriptor for the
//	texture.  Even textures that have multiple surfaces like UVAS or
//	mip maps must have the same pixel format for all surfaces.
//--------------------------------------------------------------------
inline const g2dPFD& matTexture::GetPixelFormat() const
{
	return m_PFD;
}

//--------------------------------------------------------------------
//	HasTransparency returns true if the texture has any transparent
//	parts (and therefore needs to be sorted differently)
//--------------------------------------------------------------------
inline bool matTexture::HasTransparency() const
{
	return m_PFD.NumAlphaBits() != 0;
}


//--------------------------------------------------------------------
//	RemoveTransparencyMap will build a transparency map for this texture
//	i_Bias specifies the range (0-i_Bias) that will be considered transparent
//--------------------------------------------------------------------
inline void matTexture::RemoveTransparencyMap()
{
	if (m_TransparencyMap)
	{
		delete m_TransparencyMap;
		m_TransparencyMap = NULL;
	}
}

//--------------------------------------------------------------------
//	HasTransparencyMap returns whether there is a transparency map for
//	this texture
//--------------------------------------------------------------------
inline bool matTexture::HasTransparencyMap() const
{
	return (m_TransparencyMap ? true : false);
}

//--------------------------------------------------------------------
//	IsPointTransparent returns true if the given (x,y) is transparent,
//	be sure to convert to texel coordinates before querying this function
//	will always return false if no there is no transparencymap, or if the
//	point is out of bounds of the texture
//--------------------------------------------------------------------
inline bool matTexture::IsPointTransparent(int i_x, int i_y) const
{
	DBG_ASSERT(m_TransparencyMap, "Calling IsPointTransparent on texture w/ no transparencymap");
	if (!m_TransparencyMap)
		return false;
	DBG_ASSERT(0 <= i_x && i_x < GetWidth() && 0 <= i_y && i_y < GetHeight(), "Invalid x #" << i_x << ", y #" << i_y << " values");
	if (!(0 <= i_x && i_x < GetWidth() && 0 <= i_y && i_y < GetHeight()))
		return false;

	return (m_TransparencyMap[(i_y * GetWidth()) + i_x] ? true : false);
}
