/****************************************************************************\
**  bumpVertexDecl.hpp
**
**      bumpVertexDecl contains declaration for bump mapping mesh type
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef BUMP_VERTEXDECL_HPP
#error bumpVertexDecl.hpp multiply included
#endif
#define BUMP_VERTEXDECL_HPP

#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif

namespace bumpVertexDecl
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Initialize();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void DeInitialize();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	ID3D11InputLayout* GetBumpMeshDeclaration(bool i_bSkinned = false, 
											  bool i_bVelocityMap = false);
}
