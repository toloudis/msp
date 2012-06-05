/*****************************************************************************
**	smdlHierarchyObject.hpp
**
**		smdlHierarchyObject represents an object which can be animated using
**	a hierarchical animation.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_HIERARCHYOBJECT_HPP
#error smdlHierarchyObject.hpp multiply included
#endif
#define SMDL_HIERARCHYOBJECT_HPP

#ifndef SMDL_GEOANIMATABLEOBJECT_HPP
#include "Graphics/smdl/smdlGeoAnimatableObject.hpp"
#endif


//============================================================================
//============================================================================
class g3dSceneNode;
class smdlMeshStaticGroup;


//============================================================================
//============================================================================
class smdlHierarchyObject : public smdlGeoAnimatableObject
{
	public:
		//--------------------------------------------------------------------
		//	The object will initially
		//	be placed at the origin with unit scale and no rotation.
		//  Assumes ownership of the i_pHierarchyRoot.
		//--------------------------------------------------------------------
		smdlHierarchyObject( g3dSceneNode* i_pHierarchyRoot );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~smdlHierarchyObject();

		//--------------------------------------------------------------------
		// Overrides scObject's function. Returns true if we have a 
		//	low-resolution variation of the model.
		//--------------------------------------------------------------------
		virtual bool HasLowResolutionModel() const;

		//--------------------------------------------------------------------
		//	Animate
		//--------------------------------------------------------------------
		virtual void Animate(float i_SimulationTime);

		//--------------------------------------------------------------------
		//  GetHierarchyRoot
		//--------------------------------------------------------------------
		inline g3dSceneNode* GetHierarchyRoot();
		inline const g3dSceneNode* GetHierarchyRoot() const;

		//--------------------------------------------------------------------
		// In joint skeletons, scale values should only affect the
		//	current joint and its influences. Mark this true to
		//	keep scaling from propagating to the child nodes.
		//--------------------------------------------------------------------
		//void SetLocalScaling(bool i_bLocal);

	protected:
		//--------------------------------------------------------------------
		//  GetStaticMeshes - group of static meshes within hierarchy
		//--------------------------------------------------------------------
		inline smdlMeshStaticGroup* StaticMeshes();
		inline const smdlMeshStaticGroup* GetStaticMeshes() const;

	private:
		g3dSceneNode* m_pHierarchy;
		//bool m_bLocalScaling;
		smdlMeshStaticGroup* m_pStaticMeshes;
};


//--------------------------------------------------------------------
//  GetHierarchyRoot
//--------------------------------------------------------------------
inline g3dSceneNode* smdlHierarchyObject::GetHierarchyRoot()
{
	return m_pHierarchy;
}

inline const g3dSceneNode* smdlHierarchyObject::GetHierarchyRoot() const
{
	return m_pHierarchy;
}

//--------------------------------------------------------------------
//  GetStaticMeshes - group of static meshes within hierarchy
//--------------------------------------------------------------------
inline smdlMeshStaticGroup* smdlHierarchyObject::StaticMeshes()
{
	return m_pStaticMeshes;
}
inline const smdlMeshStaticGroup* smdlHierarchyObject::GetStaticMeshes() const
{
	return m_pStaticMeshes;
}
