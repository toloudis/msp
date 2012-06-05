/*****************************************************************************
**	api3dNodeReference.hpp
**
**	Reference that represents an attachment to a node within the scene graph
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef API3D_NODEREFERENCE_HPP
#error api3dNodeReference.hpp multiply included
#endif
#define API3D_NODEREFERENCE_HPP

#ifndef API3D_REFERENCE_HPP
#include "Tool/api3d/api3dReference.hpp"
#endif


//============================================================================
//============================================================================
class g3dSceneNode;


//============================================================================
//============================================================================
class api3dNodeReference : public api3dReference
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	api3dNodeReference(g3dSceneNode &i_Node);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~api3dNodeReference();

	//--------------------------------------------------------------------
	// GetMatrix for this named refence
	//--------------------------------------------------------------------
	virtual maMatrix4x4 GetMatrix() const;

private:
	g3dSceneNode &m_Node;
};
