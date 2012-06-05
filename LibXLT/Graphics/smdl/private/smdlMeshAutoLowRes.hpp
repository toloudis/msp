/*****************************************************************************
**	smdlMeshAutoLowRes.hpp
**
**		smdlMeshAutoLowRes creates a low resolution model automatically
**	from skinned surfaces.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_MESHAUTOLOWRES_HPP
#error smdlMeshAutoLowRes.hpp multiply included
#endif
#define SMDL_MESHAUTOLOWRES_HPP

#ifndef SMDL_SURFACE_HPP
#include "Graphics/smdl/private/smdlSurface.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class g3dFragment;
class g3dSceneNode;
class matMaterial;
struct mdlSkinInfo;


//============================================================================
//============================================================================
class smdlMeshAutoLowRes : public smdlSurface
{
	public:
		//--------------------------------------------------------------------
		//	smdlMeshAutoLowRes requires the root scene node of the skeleton.
		//	It will insert new static meshes into the hierarchy based
		//	on the skinned fragments and mark them low-resolution
		//--------------------------------------------------------------------
		smdlMeshAutoLowRes( g3dSceneNode *i_pRootJoint, 
							const std::vector<mdlSkinInfo>& i_SkinnedSurfaces );

		//--------------------------------------------------------------------
		//  Destructor
		//--------------------------------------------------------------------
		~smdlMeshAutoLowRes();

		//--------------------------------------------------------------------
		//	Visible - set/get whether the given surface is renderable
		//--------------------------------------------------------------------
		virtual void SetVisible(bool i_bVisible);
		virtual bool GetVisible() const;

		//--------------------------------------------------------------------
		//	Returns true if some of the fragments in this group are
		//	marked as the low-resolution model.
		//--------------------------------------------------------------------
		bool HasLowResolution() const;

	private:
		matMaterial *m_pLowResMat;
		std::vector<g3dFragment*> m_Fragments;

		std::vector<g3dSceneNode*> m_ParentNodes;
		bool m_bVisible;
};
