/********************************************************************************************\
**  xfrmTransformGroup.hpp
**
**		xfrmTransformGroup keeps track of a grouping transform node in the scene graph
**	that can apply its transformation to child objects and child transform groups.
**
**  StudioGPU
**  Copyright(C) 2010 - All Rights Reserved
\********************************************************************************************/

#ifdef XFRM_TRANSFORMGROUP_HPP
#error xfrmTransformGroup.hpp multiply included
#endif
#define XFRM_TRANSFORMGROUP_HPP

#ifndef XFRM_TRANSFORMNODE_HPP
#include "Support/xfrm/xfrmTransformNode.hpp"
#endif 
#ifndef MA_MATRIX4X4_HPP
#include "Core/Ma/maMatrix4x4.hpp"
#endif 

#include <vector>


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class xfrmTransformGroup : public xfrmTransformNode
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	xfrmTransformGroup(nameObject* i_pNameObj, 
					   g3dSceneNode* i_pSceneNode, 
					   xfrmTransformNodeCallback* i_pCallback);

	//--------------------------------------------------------------------
	// Set whether this node inherits transformation from parent node
	//--------------------------------------------------------------------
	void SetInheritsTransform(bool i_bInherit);
	bool GetInheritsTransform() const	{ return m_bInheritsTransform; }

	//--------------------------------------------------------------------
	// Current transformation for this node only
	//--------------------------------------------------------------------
	void SetTransformation(const maMatrix4x4& i_Matrix);
	const maMatrix4x4& GetTransformation() const	{ return m_Matrix; }

	//--------------------------------------------------------------------
	// Current transformation for this node plus parent matrices
	//--------------------------------------------------------------------
	const maMatrix4x4& GetTotalTransformation() const;

	//--------------------------------------------------------------------
	// Get pickable state for this node considering the pickable
	// state of all parent nodes.
	//--------------------------------------------------------------------
	bool GetPickable() const;

	//--------------------------------------------------------------------
	// Set the pickable state for this parent node, this will 
	//	affect all children
	//--------------------------------------------------------------------
	void SetPickable(bool i_bPickable);

	//--------------------------------------------------------------------
	// Child objects, can be other groups or leaf objects
	//--------------------------------------------------------------------
	std::vector<xfrmTransformNode*>  m_ChildNodes;

protected:
	//--------------------------------------------------------------------
	// Notification that a parent transform node has changed its transformation
	//--------------------------------------------------------------------
	virtual void ParentTransformChanged();

	//--------------------------------------------------------------------
	// Notification that a parent transform node has changed its pickable state
	//--------------------------------------------------------------------
	virtual void ParentPickableChanged();

private:
	bool m_bInheritsTransform;
	maMatrix4x4 m_Matrix;		// Local Matrix
	mutable maMatrix4x4 m_TotalMatrix;	// Total Matrix combining local matrix plus parent matrices
	mutable bool m_bTotalMatrixDirty;

	bool m_bPickable;
	mutable bool m_bTotalPickable;
	mutable bool m_bPickableDirty;
};
