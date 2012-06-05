/*****************************************************************************
**  mtrPickUtil.cpp
**
**      mtrPickUtil 
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "mtrPickUtil.hpp"

#include "mtrFragmentTraverser.hpp"

#include "g3dFragment.hpp"
#include "maMatrix4x4.hpp"

namespace mtrPickUtil
{
namespace
{

//maPoint3d l_PickPos1, l_PickPos2;
//mtrFaceMesh* l_PickMesh = NULL;
//int l_PickEdge = -1;

class FacePickOp : public mtFragmentOp
{
public:
	FacePickOp(std::map<FragmentMaterial, mtrFaceMesh>& i_FaceMap,
					   const maPoint3d &i_RayStart, 
					   const maVector3d &i_RayDir)
		: m_FaceMap(i_FaceMap),
		  m_RayStart(i_RayStart),
		  m_RayDir(i_RayDir),
		  m_BestTval(99999),
		  m_PickMaterial(NULL)
	{ }

	virtual void Apply( const maMatrix4x4& i_Matrix, g3dFragment* i_pFragment );

	bool HasPick() const { return m_PickMaterial != NULL; }
	matMaterial* GetPickedMaterial() const { return m_PickMaterial; }

private:
	std::map<FragmentMaterial, mtrFaceMesh>& m_FaceMap;
	const maPoint3d &m_RayStart;
	const maVector3d &m_RayDir;

	// Results
	float m_BestTval;
	matMaterial* m_PickMaterial;
};

void FacePickOp::Apply( const maMatrix4x4& i_Matrix, g3dFragment* i_pFragment )	
{
	FragmentMaterial info(i_pFragment, i_pFragment->GetMaterial());

	std::map<FragmentMaterial, mtrFaceMesh>::iterator it = m_FaceMap.find( info );
	if (it == m_FaceMap.end())
	{
		info.m_Fragment = NULL;
		it = m_FaceMap.find(info); // try NULL for single-skin map
	}

	if (it != m_FaceMap.end())
	{
		maMatrix4x4 invert = i_Matrix;
		invert.Invert();
		maPoint3d ray_pt = invert * m_RayStart;
		maPoint3d ray_end = invert * (m_RayStart + m_RayDir);

		float tval;
		int face = it->second.PickFace(ray_pt, (ray_end - ray_pt), tval);
		if ( (face >= 0) && (tval < m_BestTval) )
		{
			m_BestTval = tval;
			m_PickMaterial = i_pFragment->GetMaterial();
		}
	}
}

} // end of namespace


//========================================================================
// PickMaterial()
//========================================================================
matMaterial* PickMaterial( scObject* i_Object,
			   std::map<FragmentMaterial, mtrFaceMesh>& i_FaceMap,
			   const maPoint3d &i_RayStart, 
			   const maVector3d &i_RayDir )
{
	maMatrix4x4 identity_matrix;
	FacePickOp pick_op(i_FaceMap, i_RayStart, i_RayDir);

	mtrFragmentTraverser::TraverseFragments(i_Object, pick_op, identity_matrix);
	
	if (pick_op.HasPick())
	{
		return pick_op.GetPickedMaterial();
	}
	return NULL;
}

}	// end of namespace

