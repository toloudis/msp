/*****************************************************************************
**  mayFragCreateOptiX.hpp
**
**      mayFragCreateOptiX makes the OptiX backend's fragments from imported
**	models (mdlFragCreate), installed by GraphicsOptiXLayer while it
**	exists, as mayPackage installs GraphicsVulkan's.
**
**	Stub: makes no fragments (NULL) and leaves the import data unchanged.
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
