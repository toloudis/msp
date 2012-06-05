/*****************************************************************************\
**	icnIconSet.hpp
**
**		icnIconSet represents multiple instances of the same icon in order to 
**	have one icon per render panel and let it set its size and visibility
**	separately.
**
**	It implements the api3dObject interface so that it can be treated like a 
**	single object from the application point of view.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef ICN_ICONSET_HPP
#error icnIconSet.hpp multiply included
#endif
#define ICN_ICONSET_HPP

#ifndef API3D_OBJECT_HPP
#include "Tool/api3d/api3dObject.hpp"
#endif


//============================================================================
// forward declarations
//============================================================================
class scObject;
class api3dObjectSingle;
class g3dFragment;


//============================================================================
//============================================================================
class icnIconSet : public api3dObject
{
public:
	//--------------------------------------------------------------------
	// This icon set takes ownership of the base object.
	//--------------------------------------------------------------------
	icnIconSet(api3dObjectSingle* i_pBaseObject);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~icnIconSet();

	//--------------------------------------------------------------------
	// Add a clone of the base object`s scene graph to the given node.
	// This maintains management for the clone so that all scene graphs
	// update together.
	//
	// Note: The current implementation assumes that the clones are
	// created before any values are set into the base object.
	//--------------------------------------------------------------------
	void AddClone(g3dSceneNode *i_pParent);

//============================================================================
// layer specific control for icons that adjust scale to camera view
//============================================================================

	//--------------------------------------------------------------------
	//	SetLayerScale changes the scale of the compass for only a 
	// given icon layer (a render panel)
	//--------------------------------------------------------------------
	void SetLayerScale(int i_IconLayerIndex, const maPoint3d& i_Scale);
	maPoint3d GetLayerScale(int i_IconLayerIndex) const;

	//--------------------------------------------------------------------
	//	SetLayerRenderable sets the compass visibility for only a given icon 
	// layer (a render panel)
	//--------------------------------------------------------------------
	void SetLayerRenderable(int i_IconLayerIndex, bool i_bRender);
	bool GetLayerRenderable(int i_IconLayerIndex) const;

//============================================================================
// api3dObject interface
//============================================================================

	//--------------------------------------------------------------------
	// Position
	//--------------------------------------------------------------------
	virtual maPoint3d GetPosition() const;
	virtual void  SetPosition(const maPoint3d &i_Position);

	//--------------------------------------------------------------------
	// Orientation
	//--------------------------------------------------------------------
	virtual maRotation GetOrientation() const;
	virtual void  SetOrientation(const maRotation &i_Rotation);

	//--------------------------------------------------------------------
	// Scale
	//--------------------------------------------------------------------
	virtual maVector3d GetScale() const;
	virtual void  SetScale(const maVector3d& i_Scale);
	virtual void  SetUniformScale(const float i_fScale);

	//--------------------------------------------------------------------
	//	Renderable sets whether the object is visible
	//--------------------------------------------------------------------
	virtual void SetRenderable(bool i_Renderable);
	virtual bool GetRenderable() const;

	//--------------------------------------------------------------------
	//	Set active sets whether the object is visible in render layer
	//--------------------------------------------------------------------
	virtual void SetActiveInRenderLayer(bool i_Renderable);
	virtual bool GetActiveInRenderLayer() const;

	//--------------------------------------------------------------------
	//	Set active sets whether the object is visible in scene manager
	//--------------------------------------------------------------------
	virtual void SetActiveInSceneMgr(bool i_Renderable);
	virtual bool GetActiveInSceneMgr() const;

	//--------------------------------------------------------------------
	//	Wireframe sets whether the object drawn in wireframe
	//--------------------------------------------------------------------
	virtual void SetWireframe(bool i_Wireframe);
	virtual bool GetWireframe() const;

	//--------------------------------------------------------------------
	//	LowResolution sets whether the object should render its
	//		low resolution model.  Calls to SetLowResolution may
	//		not change anything if there is no low-res model to choose.
	//--------------------------------------------------------------------
	virtual void SetLowResolution(bool i_LowResolution);
	virtual bool GetLowResolution() const;

	//--------------------------------------------------------------------
	//	GPUPickable sets whether the object should be rendered
	//	in pick renders when doing GPU picking
	//--------------------------------------------------------------------
	virtual void SetGPUPickable(bool i_Pickable);
	virtual bool GetGPUPickable() const;

	//--------------------------------------------------------------------
	//	PickHull sets whether the object should be rendered in 
	//	pick renders even if the Renderable flag is false.
	//--------------------------------------------------------------------
	virtual void SetPickHull(bool i_PickHull);
	virtual bool GetPickHull() const;
	
	//----------------------------------------------------------------------------
	// PickMask is a user defined bit mask that can be used to filter
	// the pickable objects. The default value of "0" means "do not filter".
	//----------------------------------------------------------------------------
	virtual void SetPickMask(envType::UInt32 i_PickMask);

	//--------------------------------------------------------------------
	// Inherits Transform controls if the given object uses the 
	// transformations of its parent nodes.
	//--------------------------------------------------------------------
	virtual void SetInheritsTransform(bool i_bInherits);
	virtual bool GetInheritsTransform() const;

	//--------------------------------------------------------------------
	// Get transformation for the root node of the given object.
	// This is the matrix for the position, scale, rotation,
	// along with pivots stored in this object.
	//--------------------------------------------------------------------
	virtual void GetTransformation(maMatrix4x4& o_Transformation);

	//--------------------------------------------------------------------
	//	GetWorldBox returns the bounding box
	//--------------------------------------------------------------------
	virtual const maAxisBox& GetWorldBox() const;

	//--------------------------------------------------------------------
	//	Get reference for given name.  The returned pointer is owned
	//	by this object.  The object should be retained by the caller
	//	to avoid repeated string searches.
	//--------------------------------------------------------------------
	virtual api3dReference* GetReference(const char* i_Name);

	//--------------------------------------------------------------------
	// Get list of references
	//--------------------------------------------------------------------
	virtual void GetReferenceList(std::vector<std::string> &o_List);

	//--------------------------------------------------------------------
	// Set color (emissive, not diffuse) for fragments
	//--------------------------------------------------------------------
	virtual void SetColor(const maFloatRGBA &i_Color);

	//----------------------------------------------------------------------------
	// Is the pick code given within the high and low pick codes assigned
	// to this node?
	//----------------------------------------------------------------------------
	virtual bool ContainsPickCode(envType::UInt32 i_PickCode) const;

	//--------------------------------------------------------------------
	//	Returns the node with a fragment with the given pick code
	//--------------------------------------------------------------------
	virtual const g3dSceneNode* GetPickedNode(envType::UInt32 i_PickCode) const;

private:
	api3dObjectSingle *m_pBaseObject;
	std::vector<scObject*> m_Clones;
	std::vector<g3dFragment*> m_Fragments;
};