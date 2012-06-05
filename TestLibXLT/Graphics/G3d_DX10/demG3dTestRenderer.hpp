/****************************************************************************\
**	demG3dTestRenderer.hpp
**
**	The demG3dTestRenderer is a base class that is a renderer that can 
**	respond to kbd input from the demG3d test framework.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTRENDERER_HPP
#error demG3dTestRenderer.hpp multiply included
#endif
#define DEM_G3DTESTRENDERER_HPP

#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#endif

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif

class demG3dTestRenderer : public g3dSceneRenderer
{
public:
	demG3dTestRenderer():g3dSceneRenderer() {}
	virtual ~demG3dTestRenderer() {}

	virtual void HandleChar(itString::CharType ch) {}
};


