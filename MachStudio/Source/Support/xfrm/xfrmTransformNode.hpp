/********************************************************************************************\
**  xfrmTransformNode.hpp
**
**		xfrmTransformNode represents a node in the scene graph that can be 
**	grouped under another node from which it would inherit transforms.
**
**  StudioGPU
**  Copyright(C) 2010 - All Rights Reserved
\********************************************************************************************/

#ifdef XFRM_TRANSFORMNODE_HPP
#error xfrmTransformNode.hpp multiply included
#endif
#define XFRM_TRANSFORMNODE_HPP

#ifndef NAME_OBJECT_HPP
#include "Core/name/nameObject.hpp"
#endif 
#ifndef XFRM_TRANSFORMNODECALLBACK_HPP
#include "Support/xfrm/xfrmTransformNodeCallback.hpp"
#endif 

#include <boost/function.hpp>

//--------------------------------------------------------------------
//--------------------------------------------------------------------
class g3dSceneNode;
class xfrmTransformGroup;
class nameObject;


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class xfrmTransformNode
{
public:
	nameObject* m_pNameObject;
	g3dSceneNode* m_pSceneNode;
	xfrmTransformGroup* m_pParentGroup;
	xfrmTransformNodeCallback* m_pCallback;
	

	xfrmTransformNode(nameObject* i_pNameObj, 
					  g3dSceneNode* i_pSceneNode, 
					  xfrmTransformNodeCallback* i_pCallback)
		:   m_pNameObject(i_pNameObj), 
			m_pSceneNode(i_pSceneNode), 
			m_pParentGroup(NULL), 
			m_pCallback(i_pCallback) {}

	virtual ~xfrmTransformNode() {}

	// Notification that a parent transform node has changed its transformation
	virtual void ParentTransformChanged() {}

	// Notification that a parent transform node has changed its pickable state
	virtual void ParentPickableChanged() {}
};
