/****************************************************************************\
**  bumpVertexDecl.hpp
**
**	GraphicsVulkan port of GraphicsDX11/bump/private/bumpVertexDecl.hpp:
**	the vertex layouts (vkdVertexLayout) where DX11 has input layouts.
**	Vulkan matches vertex inputs to shaders by semantic when the
**	pipeline is made, so no shader is needed to create them.
**
**      bumpVertexDecl contains declaration for bump mapping mesh type
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_BUMP_VERTEXDECL_HPP
#error bumpVertexDecl.hpp multiply included
#endif
#define GVK_BUMP_VERTEXDECL_HPP

namespace gvk
{
class vkdVertexLayout;

namespace bumpVertexDecl
{
	void Initialize();

	void DeInitialize();

	//	NULL for the skinned layout, as in DX11 (it is never created)
	const vkdVertexLayout* GetBumpMeshDeclaration(bool i_bSkinned = false,
												  bool i_bVelocityMap = false);
}
}
