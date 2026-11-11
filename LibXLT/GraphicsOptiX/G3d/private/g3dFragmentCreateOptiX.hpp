/*****************************************************************************
**  g3dFragmentCreateOptiX.hpp
**
**      g3dFragmentCreateOptiX makes the OptiX backend's fragments (meshes)
**	from vertex and index arrays (g3dFragmentCreate), installed by
**	GraphicsOptiXLayer while it exists.
**
**	Fragments are g3dFragmentOptiX: CPU meshes, not drawn yet.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_G3DFRAGMENTCREATEOPTIX_HPP
#error g3dFragmentCreateOptiX.hpp multiply included
#endif
#define GOX_G3DFRAGMENTCREATEOPTIX_HPP

#ifndef G3D_FRAGMENTCREATE_HPP
#include "Graphics/G3d/g3dFragmentCreate.hpp"
#endif

namespace gox
{
	class g3dFragmentCreateOptiX : public g3dFragmentCreateImpl
	{
		public:
			virtual g3dFragment* CreateFragment(const maPoint3d* i_Vertices,
												const maPoint3d* i_Normals,
												int i_nVertices,
												const g3dIndexPtr i_Indices,
												int i_nIndices,
												matMaterial* i_pMaterial,
												bool i_bMorphable);
			virtual g3dFragment* CreateFragment(const maPoint3d* i_Vertices,
												const maPoint3d* i_Normals,
												const maPoint2d* i_UVs,
												int i_nVertices,
												const g3dIndexPtr i_Indices,
												int i_nIndices,
												matMaterial* i_pMaterial,
												bool i_bMorphable);
			virtual g3dFragment* CreateLineList(const maPoint3d* i_Vertices,
												const maPoint3d* i_Normals,
												int i_nVertices,
												const g3dIndexPtr i_Indices,
												int i_nIndices,
												matMaterial* i_pMaterial,
												bool i_bMorphable,
												const maPoint2d* i_UVs,
												const maPoint3d* i_Ss,
												const maPoint3d* i_Ts);
			virtual g3dFragment* CloneFragment(const g3dFragment* i_pFragment);
			virtual void SetComputeBasisVectors(bool i_bCompute);
			virtual void StoreGeometryInVideoMemory(bool i_bVideoMem);
	};
}
