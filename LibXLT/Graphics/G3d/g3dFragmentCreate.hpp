/****************************************************************************\
**	g3dFragmentCreate.hpp
**
**	g3dFragmentCreate contains functions for creating fragments from
**	vertices and materials.  It must be configured to create fragments
**	of a certain type by implementing its Creator class.
**
**	StudioGPU
**	Copyright(C) 2003. - All Rights Reserved
\****************************************************************************/
#ifdef G3D_FRAGMENTCREATE_HPP
#error g3dFragmentCreate.hpp multiply included
#endif
#define G3D_FRAGMENTCREATE_HPP

#ifndef ENV_ABSTRACTION_HPP
#include "Core/env/envAbstraction.hpp"
#endif
#ifndef G3D_INDEXPTR_HPP
#include "Graphics/g3d/g3dIndexPtr.hpp"
#endif
#ifndef MA_POINT2D_HPP
#include "Core/ma/maPoint2d.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif


//============================================================================
//	Forward References
//============================================================================
class matMaterial;
class g3dFragment;
class g3dFragmentCreateImpl;


//============================================================================
//============================================================================
class g3dFragmentCreate : public envAbstraction<g3dFragmentCreateImpl>
{
public:
	//--------------------------------------------------------------------
	// Static functions for direct access:
	//   g3dFragmentCreate::CreateFragment
	//--------------------------------------------------------------------

		//--------------------------------------------------------------------
		//	Creates fragment with no texture coordinates
		//--------------------------------------------------------------------
		static g3dFragment* CreateFragment(	const maPoint3d* i_Vertices,
										const maPoint3d* i_Normals,
										int	i_nVertices,
										const g3dIndexPtr i_Indices,
										int	i_nIndices,
										matMaterial* i_pMaterial,
										bool i_bMorphable = false );

		//--------------------------------------------------------------------
		//	Creates fragment with one set of texture coordinates
		//--------------------------------------------------------------------
		static g3dFragment* CreateFragment(	const maPoint3d* i_Vertices,
										const maPoint3d* i_Normals,
										const maPoint2d* i_UVs,
										int	i_nVertices,
										const g3dIndexPtr i_Indices,
										int	i_nIndices,
										matMaterial* i_pMaterial,
										bool i_bMorphable = false  );

		//--------------------------------------------------------------------
		//	Creates line list fragment
		//--------------------------------------------------------------------
		static g3dFragment* CreateLineList(	const maPoint3d* i_Vertices,
										const maPoint3d* i_Normals,
										int	i_nVertices,
										const g3dIndexPtr i_Indices,
										int	i_nIndices,
										matMaterial* i_pMaterial,
										bool i_bMorphable = false,
										const maPoint2d* i_UVs = NULL,
										const maPoint3d* i_Ss = NULL,
										const maPoint3d* i_Ts = NULL );
		
		//--------------------------------------------------------------------
		// Creates new fragment with same values as the old fragment
		// sharing vertex and index buffers, if possible.
		//--------------------------------------------------------------------
		static g3dFragment* CloneFragment(	const g3dFragment* i_pFragment );

		//--------------------------------------------------------------------
		// Set flag to turn off generation of basis vectors (for faster loads)
		//--------------------------------------------------------------------
		static void SetComputeBasisVectors(bool i_bCompute);

		//--------------------------------------------------------------------
		// Set flag for whether to store geometry in video or system memory
		//--------------------------------------------------------------------
		static void StoreGeometryInVideoMemory(bool i_bVideoMem);
};


//============================================================================
//============================================================================
class g3dFragmentCreateImpl
{
public:
	//--------------------------------------------------------------------
	// Virtual functions to be overriden in implementation
	//--------------------------------------------------------------------

		//--------------------------------------------------------------------
		//	Creates fragment with no texture coordinates
		//--------------------------------------------------------------------
		virtual g3dFragment* CreateFragment(	const maPoint3d* i_Vertices,
										const maPoint3d* i_Normals,
										int	i_nVertices,
										const g3dIndexPtr i_Indices,
										int	i_nIndices,
										matMaterial* i_pMaterial,
										bool i_bMorphable ) = 0;

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
										bool i_bMorphable  ) = 0;


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
										const maPoint3d* i_Ts ) = 0;

		//--------------------------------------------------------------------
		// Creates new fragment with same values as the old fragment
		// sharing vertex and index buffers, if possible.
		//--------------------------------------------------------------------
		virtual g3dFragment* CloneFragment(	const g3dFragment* i_pFragment ) = 0;

		//--------------------------------------------------------------------
		// Set flag to turn off generation of basis vectors (for faster loads)
		//--------------------------------------------------------------------
		virtual void SetComputeBasisVectors(bool i_bCompute) = 0;

		//--------------------------------------------------------------------
		// Set flag for whether to store geometry in video or system memory
		//--------------------------------------------------------------------
		virtual void StoreGeometryInVideoMemory(bool i_bVideoMem) = 0;
};
