/*****************************************************************************
**  emdlCharacterTemplate.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Graphics/emdl/emdlCharacterTemplate.hpp"

#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Core/env/envSTLHelpers.hpp"
//#include "Graphics/smdl/smdlJoint.hpp"


//--------------------------------------------------------------------
//	Constructor
//--------------------------------------------------------------------
emdlCharacterTemplate::emdlCharacterTemplate()
:	m_pModel(NULL) //, m_pJoints(NULL)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
emdlCharacterTemplate::~emdlCharacterTemplate()
{
	// The joint tree does not delete the scene graph 
	// it points to, so we can delete the whole node tree and the
	// joint trees separately.
	//delete m_pJoints;
	delete m_pModel;
}

//--------------------------------------------------------------------
//	SetModel sets the scene node representing the root of the
//	object.
//--------------------------------------------------------------------
void emdlCharacterTemplate::SetModel( g3dSceneNode* i_pNode )
{
	m_pModel = i_pNode;
}

//--------------------------------------------------------------------
//	SetModelType sets the model type that should be used to display
//	the object.
//--------------------------------------------------------------------
//void emdlCharacterTemplate::SetJoints( smdlJoint* i_pHead )
//{
//	m_pJoints = i_pHead;
//}


//--------------------------------------------------------------------
//	SetFilename sets the filename from which this template was loaded.
//bga - This is used to share subdivision networks. It would be better
//	if the subdiv infos and other things in the template were shared
//	also, but this is a change in a hurry for a bug fix.
//--------------------------------------------------------------------
void emdlCharacterTemplate::SetFilename( const fsLocator& i_Locator )
{
	m_Filename = i_Locator;
}
