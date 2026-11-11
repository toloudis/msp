/*****************************************************************************
**  mayFragCreateOptiX.cpp
**
**      See mayFragCreateOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_MAYFRAGCREATEOPTIX_HPP
#include "GraphicsOptiX/may/private/mayFragCreateOptiX.hpp"
#endif

#include <cstddef>

namespace gox
{
	bool mayFragCreateOptiX::UseBumpFrag(int /*i_NumTexCoords*/, bool /*i_EffectQualified*/)
	{
		return false;
	}

	g3dFragment* mayFragCreateOptiX::CreateFragment(const mdlSplitFragInfo& /*i_Info*/, bool /*i_Morphable*/)
	{
		return NULL;
	}

	g3dFragment* mayFragCreateOptiX::CreateHairFragment(const mdlHairInfo& /*i_Info*/, matMaterial* /*i_pMaterial*/)
	{
		return NULL;
	}

	void mayFragCreateOptiX::OptimizeFragment(mdlSplitFragInfo& /*io_Info*/)
	{
	}

	//	No fragments, so no shared vertex buffer
	g3dVertexBuffer* mayFragCreateOptiX::CreateFragmentGroup(const mdlFragInfo& /*i_FragInfo*/,
															 std::vector<g3dFragment*>& o_Fragments,
															 bool /*i_Morphable*/)
	{
		o_Fragments.clear();
		return NULL;
	}

	void mayFragCreateOptiX::UpdateFragmentGroup(const mdlFragInfo& /*i_FragInfo*/,
												 g3dVertexBuffer& /*io_SharedVertexBuffer*/,
												 std::vector<g3dFragment*>& /*io_Fragments*/,
												 bool /*i_Morphable*/)
	{
	}
}
