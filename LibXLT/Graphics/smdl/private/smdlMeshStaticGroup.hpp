/*****************************************************************************
**	smdlMeshStaticGroup.hpp
**
**		smdlMeshStaticGroup manages the visibility of a group of fragments
**	within the scene graph hierarchy.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_MESHSTATICGROUP_HPP
#error smdlMeshStaticGroup.hpp multiply included
#endif
#define SMDL_MESHSTATICGROUP_HPP

#ifndef SMDL_SURFACE_HPP
#include "Graphics/smdl/private/smdlSurface.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class g3dSceneNode;


//============================================================================
//============================================================================
class smdlMeshStaticGroup : public smdlSurface
{
	public:
		//--------------------------------------------------------------------
		//	smdlMeshStaticGroup requires the root scene node of the skeleton.
		//	It will traverse the hierarchy and grab pointers to the nodes
		//	that contain static fragments.
		//--------------------------------------------------------------------
		smdlMeshStaticGroup( g3dSceneNode *i_pNode );

		//--------------------------------------------------------------------
		//  Destructor
		//--------------------------------------------------------------------
		~smdlMeshStaticGroup();

		//--------------------------------------------------------------------
		// Adds in node to list of managed nodes of parents of static meshes.
		// Designed for when new fragments are added to hierarchy after 
		// initial load.
		//--------------------------------------------------------------------
		void AddNode(g3dSceneNode* i_pNode);

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
		std::vector<g3dSceneNode*> m_ParentNodes;
		bool m_bVisible;
		bool m_bHasLowRes;
};
