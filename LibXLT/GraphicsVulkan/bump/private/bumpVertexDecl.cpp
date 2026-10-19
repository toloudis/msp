/****************************************************************************\
**  bumpVertexDecl.cpp
**
**	See bumpVertexDecl.hpp.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_BUMP_VERTEXDECL_HPP
#include "GraphicsVulkan/bump/private/bumpVertexDecl.hpp"
#endif

#ifndef G3D_TYPE_HPP
#include "Graphics/G3d/g3dType.hpp"
#endif
#ifndef GVK_VKDSTATE_HPP
#include "GraphicsVulkan/vkd/vkdState.hpp"
#endif

#include <memory>

namespace gvk
{
namespace bumpVertexDecl
{

namespace
{

	bool l_bDoneInit = false;
	std::unique_ptr<vkdVertexLayout> l_BumpDecl;
	std::unique_ptr<vkdVertexLayout> l_BumpDeclSkinned;
	std::unique_ptr<vkdVertexLayout> l_BumpDeclVelocityMap;

}	// end of namespace

void Initialize()
{
	if (l_bDoneInit) return;
	l_bDoneInit = true;

//	float3 Position	: POSITION;
//	float3 Normal	: NORMAL;
//	float4 UV		: TEXCOORD0;
//	float3 T		: TANGENT;
//	float3 B		: BINORMAL;

	// optional skinning info.
//	float4 Bones	: TEXCOORD1;
//	float4 Weights	: TEXCOORD2;

	const uint32_t stride = sizeof(g3dType::BumpTex1Vertex);
	l_BumpDecl.reset(new vkdVertexLayout(
	{
		{"SV_POSITION", 0, VK_FORMAT_R32G32B32_SFLOAT, 0, 0},
		{"NORMAL", 0, VK_FORMAT_R32G32B32_SFLOAT, 0, 12},
		{"TEXCOORD", 0, VK_FORMAT_R32G32_SFLOAT, 0, 24},
		{"TANGENT", 0, VK_FORMAT_R32G32B32_SFLOAT, 0, 32},
		{"BINORMAL", 0, VK_FORMAT_R32G32B32_SFLOAT, 0, 44},
	}, { stride }));

	//don't create special layout for skinning until ready to implement!

	l_BumpDeclVelocityMap.reset(new vkdVertexLayout(
	{
		{"SV_POSITION", 0, VK_FORMAT_R32G32B32_SFLOAT, 0, 0},
		{"NORMAL", 0, VK_FORMAT_R32G32B32_SFLOAT, 0, 12},
		{"TEXCOORD", 0, VK_FORMAT_R32G32_SFLOAT, 0, 24},
		{"TANGENT", 0, VK_FORMAT_R32G32B32_SFLOAT, 0, 32},
		{"BINORMAL", 0, VK_FORMAT_R32G32B32_SFLOAT, 0, 44},

		{"POSITION", 1, VK_FORMAT_R32G32B32_SFLOAT, 1, 0},
		{"NORMAL", 1, VK_FORMAT_R32G32B32_SFLOAT, 1, 12},
	}, { stride, (uint32_t)sizeof(g3dType::NonTexVertex) }));
}

void DeInitialize()
{
	l_bDoneInit = false;

	l_BumpDecl.reset();
	l_BumpDeclSkinned.reset();
	l_BumpDeclVelocityMap.reset();
}

const vkdVertexLayout* GetBumpMeshDeclaration(bool i_bSkinned /* = false */, bool i_bVelocityMap)
{
	return i_bVelocityMap ? l_BumpDeclVelocityMap.get() : (i_bSkinned ? l_BumpDeclSkinned.get() : l_BumpDecl.get());
}

}	// end of namespace
}
