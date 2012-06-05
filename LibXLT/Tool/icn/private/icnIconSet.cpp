/****************************************************************************\
**	icnIconSet.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/icn/icnIconSet.hpp"

#include "Core/Env/envSTLHelpers.hpp"
#include "Graphics/G3d/g3dFragment.hpp"
#include "Graphics/G3d/g3dSceneNode.hpp"
#include "Graphics/Sc/scObject.hpp"
#include "Tool/api3d/api3dObjectSingle.hpp"


//--------------------------------------------------------------------
// This icon set takes ownership of the base object.
//--------------------------------------------------------------------
icnIconSet::icnIconSet(api3dObjectSingle* i_pBaseObject)
: m_pBaseObject(i_pBaseObject)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
icnIconSet::~icnIconSet()
{
	// Disconnect the icon set...
	g3dSceneNode *pRootNode = m_pBaseObject->Object()->GetBase();
	if (pRootNode->GetParent())
		pRootNode->GetParent()->RemoveChild(pRootNode);
	for (int i=0; i<m_Clones.size(); ++i)
	{
		g3dSceneNode *pCloneNode = m_pBaseObject->Object()->GetBase();
		if (pCloneNode->GetParent())
			pCloneNode->GetParent()->RemoveChild(pCloneNode);
	}

	// Delete the nodes
	delete m_pBaseObject;
	envSTLHelpers::DeleteContainer(m_Clones);
	envSTLHelpers::DeleteContainer(m_Fragments);
}

//--------------------------------------------------------------------
// Add a clone of the base object`s scene graph to the given node.
// This maintains management for the clone so that all scene graphs
// update together.
//--------------------------------------------------------------------
void icnIconSet::AddClone(g3dSceneNode *i_pParent)
{
	// Note: The current implementation assumes that the clones are
	// created before any values are set into the base object.
	// If this assumption changes, then you have to set all values from
	// the base object into the clone's scObject.

	g3dSceneNode *pRootNode = m_pBaseObject->Object()->GetBase();
	g3dSceneNode *pCloneNode = pRootNode->Clone(m_Fragments);
	scObject *pClone = new scObject(pCloneNode);
	m_Clones.push_back(pClone);
	i_pParent->AddChild(pCloneNode);
}

//============================================================================
// layer specific control for icons that adjust scale to camera view
//============================================================================

//--------------------------------------------------------------------
//	SetLayerScale changes the scale of the compass for only a 
// given icon layer (a render panel)
//--------------------------------------------------------------------
void  icnIconSet::SetLayerScale(int i_IconLayerIndex, const maPoint3d& i_Scale)
{
	if (i_IconLayerIndex == 0)
		m_pBaseObject->SetScale(i_Scale);
	else
	{
		m_Clones[i_IconLayerIndex-1]->SetScale(i_Scale);
	}
}
maPoint3d icnIconSet::GetLayerScale(int i_IconLayerIndex) const
{
	if (i_IconLayerIndex == 0)
		return m_pBaseObject->GetScale();
	else
	{
		return m_Clones[i_IconLayerIndex-1]->GetScale();
	}
}

//--------------------------------------------------------------------
//	SetLayerRenderable sets the compass visibility for only a given icon 
// layer (a render panel)
//--------------------------------------------------------------------
void  icnIconSet::SetLayerRenderable(int i_IconLayerIndex, bool i_bRender)
{
	if (i_IconLayerIndex == 0)
		m_pBaseObject->SetRenderable(i_bRender);
	else
	{
		m_Clones[i_IconLayerIndex-1]->SetRenderable(i_bRender);
	}
}
bool icnIconSet::GetLayerRenderable(int i_IconLayerIndex) const
{
	if (i_IconLayerIndex == 0)
		return m_pBaseObject->GetRenderable();
	else
	{
		return m_Clones[i_IconLayerIndex-1]->GetRenderable();
	}
}

//============================================================================
// api3dObject interface
//============================================================================

//--------------------------------------------------------------------
// Position
//--------------------------------------------------------------------
maPoint3d icnIconSet::GetPosition() const
{
	return m_pBaseObject->GetPosition();
}
void  icnIconSet::SetPosition(const maPoint3d &i_Position)
{
	m_pBaseObject->SetPosition(i_Position);
	for (int i=0; i<m_Clones.size(); ++i)
		m_Clones[i]->SetPosition(i_Position);
}

//--------------------------------------------------------------------
// Orientation
//--------------------------------------------------------------------
maRotation icnIconSet::GetOrientation() const
{
	return m_pBaseObject->GetOrientation();
}
void  icnIconSet::SetOrientation(const maRotation &i_Rotation)
{
	m_pBaseObject->SetOrientation(i_Rotation);
	for (int i=0; i<m_Clones.size(); ++i)
		m_Clones[i]->SetOrientation(i_Rotation);
}

//--------------------------------------------------------------------
// Scale
//--------------------------------------------------------------------
maVector3d icnIconSet::GetScale() const
{
	return m_pBaseObject->GetScale();
}
void  icnIconSet::SetScale(const maVector3d& i_Scale)
{
	m_pBaseObject->SetScale(i_Scale);
	for (int i=0; i<m_Clones.size(); ++i)
		m_Clones[i]->SetScale(i_Scale);
}
void  icnIconSet::SetUniformScale(const float i_fScale)
{
	this->SetScale(maVector3d(i_fScale, i_fScale, i_fScale));
}

//--------------------------------------------------------------------
//	Renderable sets whether the object is visible
//--------------------------------------------------------------------
void icnIconSet::SetRenderable(bool i_Renderable)
{
	m_pBaseObject->SetRenderable(i_Renderable);
	for (int i=0; i<m_Clones.size(); ++i)
		m_Clones[i]->SetRenderable(i_Renderable);
}
bool icnIconSet::GetRenderable() const
{
	return m_pBaseObject->GetRenderable();
}

//--------------------------------------------------------------------
//	Set active sets whether the object is visible in render layer
//--------------------------------------------------------------------
void icnIconSet::SetActiveInRenderLayer(bool i_Renderable)
{
	m_pBaseObject->SetActiveInRenderLayer(i_Renderable);
	for (int i=0; i<m_Clones.size(); ++i)
		m_Clones[i]->SetActiveInRenderLayer(i_Renderable);
}
bool icnIconSet::GetActiveInRenderLayer() const
{
	return m_pBaseObject->GetActiveInRenderLayer();
}

//--------------------------------------------------------------------
//	Set active sets whether the object is visible in scene manager
//--------------------------------------------------------------------
void icnIconSet::SetActiveInSceneMgr(bool i_Renderable)
{
	m_pBaseObject->SetActiveInSceneMgr(i_Renderable);
	for (int i=0; i<m_Clones.size(); ++i)
		m_Clones[i]->SetActiveInSceneMgr(i_Renderable);
}
bool icnIconSet::GetActiveInSceneMgr() const
{
	return m_pBaseObject->GetActiveInSceneMgr();
}

//--------------------------------------------------------------------
//	Wireframe sets whether the object drawn in wireframe
//--------------------------------------------------------------------
void icnIconSet::SetWireframe(bool i_Wireframe)
{
	m_pBaseObject->SetWireframe(i_Wireframe);

	g3dSceneNode::DrawStyle dstyle = (i_Wireframe) ? g3dSceneNode::e_LitWireframe : g3dSceneNode::e_Inherit;
	for (int i=0; i<m_Clones.size(); ++i)
		m_Clones[i]->GetBase()->SetDrawStyle(dstyle);
}
bool icnIconSet::GetWireframe() const
{
	return m_pBaseObject->GetWireframe();
}

//--------------------------------------------------------------------
//	LowResolution sets whether the object should render its
//		low resolution model.  Calls to SetLowResolution may
//		not change anything if there is no low-res model to choose.
//--------------------------------------------------------------------
void icnIconSet::SetLowResolution(bool i_LowResolution)
{
	m_pBaseObject->SetLowResolution(i_LowResolution);
	for (int i=0; i<m_Clones.size(); ++i)
		m_Clones[i]->GetBase()->SetForceLowResolution(i_LowResolution);
}
bool icnIconSet::GetLowResolution() const
{
	return m_pBaseObject->GetLowResolution();
}

//--------------------------------------------------------------------
//	GPUPickable sets whether the object should be rendered
//	in pick renders when doing GPU picking
//--------------------------------------------------------------------
void icnIconSet::SetGPUPickable(bool i_Pickable)
{
	m_pBaseObject->SetGPUPickable(i_Pickable);
	for (int i=0; i<m_Clones.size(); ++i)
		m_Clones[i]->GetBase()->SetGPUPickable(i_Pickable);
}
bool icnIconSet::GetGPUPickable() const
{
	return m_pBaseObject->GetGPUPickable();
}

//--------------------------------------------------------------------
//	PickHull sets whether the object should be rendered in 
//	pick renders even if the Renderable flag is false.
//--------------------------------------------------------------------
void icnIconSet::SetPickHull(bool i_PickHull)
{
	m_pBaseObject->SetPickHull(i_PickHull);
	for (int i=0; i<m_Clones.size(); ++i)
		m_Clones[i]->GetBase()->SetPickHull(i_PickHull);
}
bool icnIconSet::GetPickHull() const
{
	return m_pBaseObject->GetPickHull();
}

//----------------------------------------------------------------------------
// PickMask is a user defined bit mask that can be used to filter
// the pickable objects. The default value of "0" means "do not filter".
//----------------------------------------------------------------------------
void icnIconSet::SetPickMask(envType::UInt32 i_PickMask)
{
	m_pBaseObject->SetPickMask(i_PickMask);
	for (int i=0; i<m_Clones.size(); ++i)
		m_Clones[i]->GetBase()->SetPickMask(i_PickMask);
}

//--------------------------------------------------------------------
// Inherits Transform controls if the given object uses the 
// transformations of its parent nodes.
//--------------------------------------------------------------------
void icnIconSet::SetInheritsTransform(bool i_bInherits)
{
	m_pBaseObject->SetInheritsTransform(i_bInherits);
	for (int i=0; i<m_Clones.size(); ++i)
	{
		// Note have to invert the flag between "inherits" and "ignore"
		m_Clones[i]->GetBase()->SetIgnoreParentTransform( !i_bInherits);
	}
}
bool icnIconSet::GetInheritsTransform() const
{
	return m_pBaseObject->GetInheritsTransform();
}

//--------------------------------------------------------------------
// Get transformation for the root node of the given object.
// This is the matrix for the position, scale, rotation,
// along with pivots stored in this object.
//--------------------------------------------------------------------
void icnIconSet::GetTransformation(maMatrix4x4& o_Transformation)
{
	return m_pBaseObject->GetTransformation(o_Transformation);
}

//--------------------------------------------------------------------
//	GetWorldBox returns the bounding box
//--------------------------------------------------------------------
const maAxisBox& icnIconSet::GetWorldBox() const
{
	return m_pBaseObject->GetWorldBox();
}

//--------------------------------------------------------------------
//	Get reference for given name.  The returned pointer is owned
//	by this object.  The object should be retained by the caller
//	to avoid repeated string searches.
//--------------------------------------------------------------------
api3dReference* icnIconSet::GetReference(const char* i_Name)
{
	return NULL;
}

//--------------------------------------------------------------------
// Get list of references
//--------------------------------------------------------------------
void icnIconSet::GetReferenceList(std::vector<std::string> &o_List)
{
}

//--------------------------------------------------------------------
// Set color (emissive, not diffuse) for fragments
//--------------------------------------------------------------------
void icnIconSet::SetColor(const maFloatRGBA &i_Color)
{
	m_pBaseObject->SetColor(i_Color);

	//bga - I am assuming that the clones share the same material
	// assignments, so that if the material changes in the base object,
	// it will affect the clones also
}

//----------------------------------------------------------------------------
// Is the pick code given within the high and low pick codes assigned
// to this node?
//----------------------------------------------------------------------------
bool icnIconSet::ContainsPickCode(envType::UInt32 i_PickCode) const
{
	if (m_pBaseObject->ContainsPickCode(i_PickCode))
		return true;

	for (int i=0; i<m_Clones.size(); ++i)
	{
		if (m_Clones[i]->GetBase()->ContainsPickCode(i_PickCode))
			return true;
	}
	return false;
}

//--------------------------------------------------------------------
//	Returns the node with a fragment with the given pick code
//--------------------------------------------------------------------
const g3dSceneNode* icnIconSet::GetPickedNode(envType::UInt32 i_PickCode) const
{
	const g3dSceneNode* pPicked = NULL;

	if (pPicked = m_pBaseObject->GetPickedNode(i_PickCode))
		return pPicked;

	for (int i=0; i<m_Clones.size(); ++i)
	{
		if (pPicked = m_Clones[i]->GetBase()->GetPickedNode(i_PickCode))
			return pPicked;
	}

	return NULL;
}