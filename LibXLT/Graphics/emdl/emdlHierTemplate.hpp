/*****************************************************************************
**  emdlHierTemplate.hpp
**
**      A emdlHierTemplate contains the geometry information needed to
**	generate scHierarchicalObjects.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef EMDL_HIERTEMPLATE_HPP
#error emdlHierTemplate.hpp multiply included
#endif
#define EMDL_HIERTEMPLATE_HPP

#ifndef ENT_MODELTEMPLATE_HPP
#include "Graphics/ent/entModelTemplate.hpp"
#endif


//============================================================================
//============================================================================
class g3dSceneNode;


//============================================================================
//============================================================================
class emdlHierTemplate : public entModelTemplate
{
	public:
		//--------------------------------------------------------------------
		//	Constructor
		//--------------------------------------------------------------------
		emdlHierTemplate();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~emdlHierTemplate();

		//--------------------------------------------------------------------
		//	GetModel returns the scene node representing the root of the
		//	object.
		//--------------------------------------------------------------------
		inline const g3dSceneNode* GetModel() const;
		inline g3dSceneNode* GetModel();

		//--------------------------------------------------------------------
		//	SetModel sets the scene node representing the root of the
		//	object.
		//--------------------------------------------------------------------
		void SetModel( g3dSceneNode* i_pNode );

	private:
		g3dSceneNode* m_pModel;
};


//--------------------------------------------------------------------
//	GetModel returns the scene node representing the root of the
//	object.
//--------------------------------------------------------------------
inline const g3dSceneNode* emdlHierTemplate::GetModel() const
{
	return m_pModel;
}
inline g3dSceneNode* emdlHierTemplate::GetModel()
{
	return m_pModel;
}
