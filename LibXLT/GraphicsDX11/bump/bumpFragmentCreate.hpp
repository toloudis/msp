/****************************************************************************\
**	bumpFragmentCreate.hpp
**
**	bumpFragmentCreate creates triangle mesh fragments by creating
**	bumpRegFrag objects.
**
**	StudioGPU
**	Copyright(C) 2003. - All Rights Reserved
\****************************************************************************/

#ifdef BUMP_FRAGMENTCREATE_HPP
#error bumpFragmentCreate.hpp multiply included
#endif
#define BUMP_FRAGMENTCREATE_HPP

#ifndef G3D_FRAGMENTCREATE_HPP
#include "Graphics/g3d/g3dFragmentCreate.hpp"
#endif

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------

class bumpFragmentCreate : public g3dFragmentCreateImpl
{
public:
		//--------------------------------------------------------------------
		//	Creates fragment with no texture coordinates
		//--------------------------------------------------------------------
		virtual g3dFragment* CreateFragment(	const maPoint3d* i_Vertices,
												const maPoint3d* i_Normals,
												int	i_nVertices,
												const g3dIndexPtr i_Indices,
												int	i_nIndices,
												matMaterial* i_pMaterial,
												bool i_bMorphable );

		//--------------------------------------------------------------------
		//	Creates fragment with one set of texture coordinates
		//--------------------------------------------------------------------
		virtual g3dFragment* CreateFragment(	const maPoint3d* i_Vertices,
												const maPoint3d* i_Normals,
												const maPoint2d* i_UVs,
												int	i_nVertices,
												const g3dIndexPtr i_Indices,
												int	i_nIndices,
												matMaterial* i_pMaterial,
												bool i_bMorphable  );

		//--------------------------------------------------------------------
		//	Creates line list fragment
		//--------------------------------------------------------------------
		virtual g3dFragment* CreateLineList(	const maPoint3d* i_Vertices,
										const maPoint3d* i_Normals,
										int	i_nVertices,
										const g3dIndexPtr i_Indices,
										int	i_nIndices,
										matMaterial* i_pMaterial,
										bool i_bMorphable,
										const maPoint2d* i_UVs,
										const maPoint3d* i_Ss,
										const maPoint3d* i_Ts );

		//--------------------------------------------------------------------
		// Creates new fragment with same values as the old fragment
		// sharing vertex and index buffers, if possible.
		//--------------------------------------------------------------------
		virtual g3dFragment* CloneFragment(	const g3dFragment* i_pFragment );

		//--------------------------------------------------------------------
		// Set flag to turn off generation of basis vectors (for faster loads)
		//--------------------------------------------------------------------
		virtual void SetComputeBasisVectors(bool i_bCompute);

		//--------------------------------------------------------------------
		// Set flag for whether to store geometry in video or system memory
		//--------------------------------------------------------------------
		virtual void StoreGeometryInVideoMemory(bool i_bVideoMem);
};
