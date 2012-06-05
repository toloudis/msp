/*****************************************************************************
**	api3dObject.hpp
**
**	Base class for all objects that can be placed into the scene
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef API3D_OBJECT_HPP
#error api3dObject.hpp multiply included
#endif
#define API3D_OBJECT_HPP

#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif

#include <string>
#include <vector>


//============================================================================
//============================================================================
class api3dReference;
class g3dSceneNode;


//============================================================================
//============================================================================
class api3dObject
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~api3dObject();

	//--------------------------------------------------------------------
	// Position
	//--------------------------------------------------------------------
	virtual maPoint3d GetPosition() const = 0;
	virtual void  SetPosition(const maPoint3d &i_Position) = 0;

	//--------------------------------------------------------------------
	// Orientation
	//--------------------------------------------------------------------
	virtual maRotation GetOrientation() const = 0;
	virtual void  SetOrientation(const maRotation &i_Rotation) = 0;

	//--------------------------------------------------------------------
	// Scale
	//--------------------------------------------------------------------
	virtual maVector3d GetScale() const = 0;
	virtual void  SetScale(const maVector3d& i_Scale) = 0;
	virtual void  SetUniformScale(const float i_fScale) = 0;

	//--------------------------------------------------------------------
	// Pivot point information
	//--------------------------------------------------------------------
	virtual maPoint3d GetPivotPoint() const;
	virtual void  SetPivotPoint(const maPoint3d &i_Position, 
						   bool i_bPreserveTransformation = false);
	virtual maVector3d GetPivotCompensation() const;
	virtual void  SetPivotCompensation(const maVector3d &i_Compensation);

	//--------------------------------------------------------------------
	//	Renderable sets whether the object is visible
	//--------------------------------------------------------------------
	virtual void SetRenderable(bool i_Renderable) = 0;
	virtual bool GetRenderable() const = 0;

	//--------------------------------------------------------------------
	//	ActiveInRenderLayer sets whether the object is visible
	//	in render layer
	//--------------------------------------------------------------------
	virtual void SetActiveInRenderLayer(bool i_bRenderable) = 0;
	virtual bool GetActiveInRenderLayer() const = 0;

	//--------------------------------------------------------------------
	//	ActiveInRenderLayer sets whether the object is visible
	//	in scene manager
	//--------------------------------------------------------------------
	virtual void SetActiveInSceneMgr(bool i_bRenderable) = 0;
	virtual bool GetActiveInSceneMgr() const = 0;

	//--------------------------------------------------------------------
	//	Wireframe sets whether the object drawn in wireframe
	//--------------------------------------------------------------------
	virtual void SetWireframe(bool i_Wireframe) = 0;
	virtual bool GetWireframe() const = 0;

	//--------------------------------------------------------------------
	//	LowResolution sets whether the object should render its
	//		low resolution model.  Calls to SetLowResolution may
	//		not change anything if there is no low-res model to choose.
	//--------------------------------------------------------------------
	virtual void SetLowResolution(bool i_LowResolution) = 0;
	virtual bool GetLowResolution() const = 0;

	//--------------------------------------------------------------------
	//	GPUPickable sets whether the object should be rendered
	//	in pick renders when doing GPU picking
	//--------------------------------------------------------------------
	virtual void SetGPUPickable(bool i_Pickable) = 0;
	virtual bool GetGPUPickable() const = 0;

	//--------------------------------------------------------------------
	//	PickHull sets whether the object should be rendered in 
	//	pick renders even if the Renderable flag is false.
	//--------------------------------------------------------------------
	virtual void SetPickHull(bool i_PickHull) = 0;
	virtual bool GetPickHull() const = 0;

	//----------------------------------------------------------------------------
	// PickMask is a user defined bit mask that can be used to filter
	// the pickable objects. The default value of "0" means "do not filter".
	//----------------------------------------------------------------------------
	virtual void SetPickMask(envType::UInt32 i_PickMask) = 0;

	//--------------------------------------------------------------------
	// Inherits Transform controls if the given object uses the 
	// transformations of its parent nodes.
	//--------------------------------------------------------------------
	virtual void SetInheritsTransform(bool i_bInherits) = 0;
	virtual bool GetInheritsTransform() const = 0;

	//--------------------------------------------------------------------
	// Get transformation for the root node of the given object.
	// This is the matrix for the position, scale, rotation,
	// along with pivots stored in this object.
	//--------------------------------------------------------------------
	virtual void GetTransformation(maMatrix4x4& o_Transformation) = 0;

	//--------------------------------------------------------------------
	//	GetWorldBox returns the bounding box
	//--------------------------------------------------------------------
	virtual const maAxisBox& GetWorldBox() const = 0;

	//--------------------------------------------------------------------
	//	Get reference for given name.  The returned pointer is owned
	//	by this object.  The object should be retained by the caller
	//	to avoid repeated string searches.
	//--------------------------------------------------------------------
	virtual api3dReference* GetReference(const char* i_Name) = 0;

	//--------------------------------------------------------------------
	// Get list of references
	//--------------------------------------------------------------------
	virtual void GetReferenceList(std::vector<std::string> &o_List) = 0;

	//--------------------------------------------------------------------
	// Set color (emissive, not diffuse) for fragments
	//--------------------------------------------------------------------
	virtual void SetColor(const maFloatRGBA &i_Color) = 0;

	//----------------------------------------------------------------------------
	// Is the pick code given within the high and low pick codes assigned
	// to this node?
	//----------------------------------------------------------------------------
	virtual bool ContainsPickCode(envType::UInt32 i_PickCode) const;

	//--------------------------------------------------------------------
	//	Returns the node with a fragment with the given pick code
	//--------------------------------------------------------------------
	virtual const g3dSceneNode* GetPickedNode(envType::UInt32 i_PickCode) const;
};