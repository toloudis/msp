/*****************************************************************************
**  mayFragCreateOptiX.hpp
**
**      mayFragCreateOptiX makes the OptiX backend's fragments from imported
**	models (mdlFragCreate), installed by GraphicsOptiXLayer while it
**	exists, as mayPackage installs GraphicsVulkan's.
**
**	CreateFragment makes a g3dFragmentOptiX from an mdlSplitFragInfo.
**	CreateFragmentGroup makes one g3dVertexBufferOptiX (over one
**	g3dVertexStoreOptiX) and one fragment per material on it, each with
**	its own indices (mdlFragInfo::GetIndexRange), as GraphicsVulkan's
**	mayFragCreate does; UpdateFragmentGroup replaces the store's vertices
**	and each fragment's indices. Shadow-volume weld indices (the ones past
**	mdlSplitFragInfo::m_WeldIndex) are dropped: they only build stencil
**	shadow volumes, which a ray tracer doesn't use. OptimizeFragment does nothing (DX11's is compiled out) and bump
**	fragments aren't used (UseBumpFrag). No hair yet: CreateHairFragment
**	returns NULL.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_MAYFRAGCREATEOPTIX_HPP
#error mayFragCreateOptiX.hpp multiply included
#endif
#define GOX_MAYFRAGCREATEOPTIX_HPP

#ifndef MDL_FRAGCREATE_HPP
#include "Graphics/mdl/mdlFragCreate.hpp"
#endif

#include <vector>

namespace gox
{
	class mayFragCreateOptiX : public mdlFragCreateImpl
	{
		public:
			virtual bool UseBumpFrag(int i_NumTexCoords, bool i_EffectQualified);
			virtual g3dFragment* CreateFragment(const mdlSplitFragInfo& i_Info, bool i_Morphable = false);
			virtual g3dFragment* CreateHairFragment(const mdlHairInfo& i_Info, matMaterial* i_pMaterial);
			virtual void OptimizeFragment(mdlSplitFragInfo& io_Info);
			virtual g3dVertexBuffer* CreateFragmentGroup(const mdlFragInfo& i_FragInfo,
														 std::vector<g3dFragment*>& o_Fragments,
														 bool i_Morphable = false);
			virtual void UpdateFragmentGroup(const mdlFragInfo& i_FragInfo,
											 g3dVertexBuffer& io_SharedVertexBuffer,
											 std::vector<g3dFragment*>& io_Fragments,
											 bool i_Morphable = false);
	};
}
