/****************************************************************************\
**	mdlFragInfo.hpp
**
**		Contains structures for passing around fragment data.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_FRAGINFO_HPP
#error mdlFragInfo.hpp multiply included
#endif
#define MDL_FRAGINFO_HPP

#ifndef ENT_FRAGINFOSINK_HPP
#include "Graphics/ent/entFragInfoSink.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef MDL_MATERIALINFO_HPP
#include "Graphics/mdl/mdlMaterialInfo.hpp"
#endif 
#ifndef MDL_MATINFO_HPP
#include "Graphics/mdl/mdlMatInfo.hpp"
#endif
#ifndef MA_POINT2D_HPP
#include "Core/ma/maPoint2d.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#include <map>
#include <vector>


//============================================================================
//============================================================================
class  mdlFragInfo : public entFragInfo
{
public:
	//forward reference to an embedded functor
	//used by the std::transform operation
	struct TransformVec;
	// see the dscription at m_Parts
	struct PartComponent
	{
		//name of the source fragment that contributed to this part
		std::string m_Name;
		//beginning index of triangles of this part
		envType::UInt32 m_PartBeginIndex;
	};

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	mdlFragInfo();

	//------------------------------------------------------------------------
	//	AddGeometry causes the geometry information from i_ToAdd to be added
	//	to this.  All indices will be modified properly, etc.
	//------------------------------------------------------------------------
	void AddGeometry(const mdlFragInfo& i_ToAdd);

	//------------------------------------------------------------------------
	//	AddGeometry causes the partial but contiguous geometry information from 
	//	i_ToAdd to be merged to this.
	//	Only the triangles captured by the interval [i_BeginIdx, i_EndIdx)
	//	will be merged.
	//	i_BeginIdx is the first triangle in i_ToAdd to be merged.
	//	i_EndIdx is the triangle that comes after the last triangle 
	//	that need be merged. Note that these are indices to triangles,
	//  To convert them o vertex indices, one has to multiple by 3.
	//	i_pTransformToBeAppliedBeforeMerging is the transformation that will be applied
	//	to the positions of the vertices before merging.
	//	Normals of the vertices will be appropriately modified by the transpose of the
	//	inverse of this transformation.
	//	The input mesh i_ToAdd should have its basis vectprs already calculated
	//  and present. The input mesh's basis vectors are appropriately modified
	//  by the transform and added to the merged mesh.
	//------------------------------------------------------------------------
	void AddGeometry(const mdlFragInfo& i_ToAdd, int i_MtlChangeBeginIdx, int i_MtlChangeEndIdx , const maMatrix4x4 * i_pTransformToBeAppliedBeforeMerging );

	//------------------------------------------------------------------------
	//	AddGeometry causes the complte geometry information from 
	//	i_ToAdd to be merged to this. So use this if the i_ToAdd has
	//  a single material and is equal to the single  material of this Frag 
	//	i_pTransformToBeAppliedBeforeMerging is the transformation that will be applied
	//	to the positions of the vertices before merging.
	//	Normals of the vertices will be appropriately modified by the transpose of the
	//	inverse of this transformation.
	//	The input mesh i_ToAdd should have its basis vectprs already calculated
	//  and present. The input mesh's basis vectors are appropriately modified
	//  by the transform and added to the merged mesh.
	//------------------------------------------------------------------------
	void AddGeometry(const mdlFragInfo& i_ToAdd, 
					 const maMatrix4x4 * i_pTransformToBeAppliedBeforeMerging );

	//------------------------------------------------------------------------
	//	Transform the positions of this geometry by  i_PosTransform,
	//	Transform the nornals of this geometry by i_NormalTransform
	//------------------------------------------------------------------------
	void ApplyTransformation( const maMatrix4x4 &i_PosTransform,  const maMatrix4x4 &i_NormalTransform );

	//------------------------------------------------------------------------
	//	GetIndexRange - Get start index and num indices for given
	//	material index
	//------------------------------------------------------------------------
	void GetIndexRange( int i_MatNum,
						int& o_FirstIndex,
						int& o_NumIndices) const;
	
	//------------------------------------------------------------------------
	// Returns true if the mesh is non-empty and has valid basis vectors calculated
	//------------------------------------------------------------------------
	bool HasValidBasisVectors() const;

public:
	std::string m_Name;
	std::vector<maPoint3d> m_Vertices;
	std::vector<maVector3d> m_Normals;
	std::vector<maPoint2d> m_UVs;
	//std::vector<maFloatRGBA> m_Colors;
	
	// for basis vectors
	std::vector<maVector3d> m_Ss;	
	std::vector<maVector3d> m_Ts;	

	int m_NumOrigVertices;
	int m_NumOrigNormals;
	std::multimap<int, int> m_VertexRemap;
	std::multimap<int, int> m_NormalRemap;
	std::vector<envType::UInt32> m_Indices;

	std::vector< shared_ptr<mdlMatInfo> > m_Materials;
	std::vector<int> m_MaterialChanges;	
	//Sometimes fragments can carry multiple parts of
	//geometries which might have been the result of a merge
	//operation from from different source fragments.
	//The sequence of part components, if present will contain,
	//the name of the source fragment and the beginning index 
	//of the triangles from that source fragment.
	std::vector< PartComponent > m_Parts;

	struct
	{
		bool m_bCastsShadow : 1;	// casts shadows
		bool m_bReceivesShadow : 1;	// receives shadows
		bool m_bBumpMap : 1;		// needs texture space info for bump mapping
		bool m_bShadowHull : 1;		// fragment is invisible, but casts shadow
		bool m_bDoubleSided : 1;	// render both sides of mesh
		bool m_bTriangleSort : 1;	// triangles needs to be sorted for transparency
		bool m_bVertexAnimation : 1;// this mesh will undergo vertex animation ("cloth")
	} m_Flags;

	int m_ResolutionLevel;		// 0 - all res, 1 - low res, 2 - high res
	std::vector<envType::UInt32> m_WeldIndices;
};

