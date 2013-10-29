/****************************************************************************\
**  matTextureGL.hpp
**
**      matTextureGL.hpp is the windows implementation of the Terawatt
**	matTexture.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#pragma once

#ifdef MAT_TEXTUREGL_HPP
#error matTextureGL.hpp multiply included
#endif
#define MAT_TEXTUREGL_HPP

#ifndef MAT_TEXTURE_HPP
#include "Graphics/mat/matTexture.hpp"
#endif

#include "Area18/ogl/oglTypes.hpp"
class oglTexture2d;

class matTextureGL : public matTexture
{
public:

	//--------------------------------------------------------------------
	//	This constructor makes an "empty" surface.  The surface pointer
	//	will be NULL, reflecting this.
	//--------------------------------------------------------------------
	matTextureGL();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~matTextureGL();

	//--------------------------------------------------------------------
	//	GetSurface returns a pointer usable for shaders.
	//--------------------------------------------------------------------
	virtual GLint GetResource() const = 0;
	virtual oglTexture2d* GetSurface() const  {return NULL;}

	//--------------------------------------------------------------------
	//	UnloadSurface releases the DirectDraw Surface
	//--------------------------------------------------------------------
	// void UnloadSurface();

	//--------------------------------------------------------------------
	//	MakeTransparencyMap will build a transparency map for this texture
	//	i_Bias specifies the range (0-i_Bias) that will be considered transparent
	//--------------------------------------------------------------------
	char *MakeTransparencyMap(float i_Bias, const g2dPFD& i_PFD, int i_Width, int i_Height);

private:
	char *m_TransparencyMap;
};



