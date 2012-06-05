/*****************************************************************************
**	api3dObjectNode.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Tool/api3d/api3dObjectNode.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/sc/scObject.hpp"


//--------------------------------------------------------------------
// constructor - will create a single node and an 
//	scObject to control it
//--------------------------------------------------------------------
api3dObjectNode::api3dObjectNode()
{
	m_pObject = new scObject();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
api3dObjectNode::~api3dObjectNode()
{
	delete m_pObject;
}

//--------------------------------------------------------------------
// Position
//--------------------------------------------------------------------
maPoint3d api3dObjectNode::GetPosition() const
{
	return m_pObject->GetPosition();
}
void  api3dObjectNode::SetPosition(const maPoint3d &i_Position)
{
	m_pObject->SetPosition(i_Position);
}

//--------------------------------------------------------------------
// Orientation
//--------------------------------------------------------------------
maRotation  api3dObjectNode::GetOrientation() const
{
	return m_pObject->GetOrientation();
}
void  api3dObjectNode::SetOrientation(const maRotation &i_Rotation)
{
	m_pObject->SetOrientation(i_Rotation);
}

//--------------------------------------------------------------------
// Scale
//--------------------------------------------------------------------
//virtual
maVector3d api3dObjectNode::GetScale() const
{
	return m_pObject->GetScale();
}

//virtual
void  api3dObjectNode::SetScale(const maVector3d& i_Scale)
{
	m_pObject->SetScale( i_Scale );
}

//virtual
void  api3dObjectNode::SetUniformScale(const float i_fScale)
{
	maVector3d scale( i_fScale, i_fScale, i_fScale );
	m_pObject->SetScale( scale );
}


//----------------------------------------------------------------------------
//	Renderable sets whether the object is visible
//----------------------------------------------------------------------------
void api3dObjectNode::SetRenderable(bool i_Renderable)
{
	m_pObject->SetRenderable(i_Renderable);
}
bool api3dObjectNode::GetRenderable() const
{
	return m_pObject->GetRenderable();
}

//--------------------------------------------------------------------
//	ActiveInRenderLayer sets whether the object is visible
//	in render layer
//--------------------------------------------------------------------
void api3dObjectNode::SetActiveInRenderLayer(bool i_bRenderable)
{
	m_pObject->SetActiveInRenderLayer(i_bRenderable);
}
bool api3dObjectNode::GetActiveInRenderLayer() const
{
	return m_pObject->GetActiveInRenderLayer();
}

//--------------------------------------------------------------------
//	ActiveInSceneMgr sets whether the object is visible
//	in scene manager
//--------------------------------------------------------------------
void api3dObjectNode::SetActiveInSceneMgr(bool i_bRenderable)
{
	m_pObject->SetActiveInSceneMgr(i_bRenderable);
}
bool api3dObjectNode::GetActiveInSceneMgr() const
{
	return m_pObject->GetActiveInSceneMgr();
}

//--------------------------------------------------------------------
//	GetWorldBox returns the bounding box
//--------------------------------------------------------------------
const maAxisBox& api3dObjectNode::GetWorldBox() const
{
	m_pObject->GetBase()->UpdateTotalTransform();
	return m_pObject->GetWorldBox();
}

//--------------------------------------------------------------------
// Set color (emissive, not diffuse) for fragments
//--------------------------------------------------------------------
void api3dObjectNode::SetColor(const maFloatRGBA &i_Color)
{
	// nothing to do, no geometry
}

//--------------------------------------------------------------------
//	Get reference for given name.  The returned pointer is owned
//	by this object.  The object should be retained by the caller
//	to avoid repeated string searches.
//--------------------------------------------------------------------
api3dReference* api3dObjectNode::GetReference(const char* i_Name)
{
	return NULL;
}

//--------------------------------------------------------------------
// Get list of references
//--------------------------------------------------------------------
void api3dObjectNode::GetReferenceList(std::vector<std::string> &o_List)
{
	// nothing to do, no named nodes
}



