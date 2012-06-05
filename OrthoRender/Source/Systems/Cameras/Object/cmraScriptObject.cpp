/*****************************************************************************
**  cmraScriptObject.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Object/cmraScriptObject.hpp"

#include "Systems/Cameras/Object/cmraCameraObject.hpp"
#include "Systems/Cameras/Timeline/cmraChannelCapture.hpp"
#include "Systems/Cameras/Object/cmraIconObject.hpp"
#include "Systems/Cameras/Undo/cmraOperations.hpp"
#include "Systems/Cameras/Data/cmraScriptData.hpp"

#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/geo/geoRayIntersection.hpp"
#include "Support/grps/grpsGroupMgr.hpp"
#include "Support/lyer/lyerLayerMgr.hpp"
#include "Core/ma/maConstants.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"


//============================================================================
//============================================================================
namespace
{
	const maVector3d l_One(1,1,1);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmraScriptObject::cmraScriptObject(const cmraScriptData &i_Data, const nameString& i_CamName)
:	m_bDebugRenderable(true),
	m_bSelected(false)
{
	m_pIcon = new cmraCameraObject(i_Data.m_BaseData.m_bOrthographic.GetValue());
	m_pIcon->SetParentObject(this);
	//m_pIcon->SetName( i_CamName );

	// Timeline manager registration
	tmlnTimelineMgr::AddObject(this, "Camera" );

	// Layer manager registration
	lyerLayerMgr::AddObject(m_pIcon, this);
	grpsGroupMgr::AddObject(m_pIcon, m_pIcon);

	// Create and add channels
	//
	m_pPosChannel = new tmlnChannelPositionProperty("Position", m_pIcon->PropertyPosition() );
	this->AddChannel(m_pPosChannel);
	m_pTargetChannel = new tmlnChannelPositionProperty("Target", m_pIcon->PropertyTarget() );
	this->AddChannel(m_pTargetChannel);
	m_pFOVChannel = new tmlnChannelFloatProperty("FOV", m_pIcon->PropertyFieldOfView() );
	this->AddChannel(m_pFOVChannel);
	m_pOrthoWidthChannel = new tmlnChannelFloatProperty("OrthoWidth", m_pIcon->PropertyOrthoWidth() );
	this->AddChannel(m_pOrthoWidthChannel);
	m_pTiltChannel = new tmlnChannelFloatProperty("Tilt", m_pIcon->PropertyTilt() );
	this->AddChannel(m_pTiltChannel);
	m_pCaptureChannel = new cmraChannelCapture("Capture");
	this->AddChannel(m_pCaptureChannel);
	m_pNearFocusDistanceChannel = new tmlnChannelFloatProperty("NearFocusDist", m_pIcon->PropertyNearFocusDistance() );
	this->AddChannel(m_pNearFocusDistanceChannel);
	m_pFarFocusDistanceChannel = new tmlnChannelFloatProperty("FarFocusDist", m_pIcon->PropertyFarFocusDistance() );
	this->AddChannel(m_pFarFocusDistanceChannel);
	m_pNearBlurDistanceChannel = new tmlnChannelFloatProperty("NearBlurDist", m_pIcon->PropertyNearBlurDistance() );
	this->AddChannel(m_pNearBlurDistanceChannel);
	m_pFarBlurDistanceChannel = new tmlnChannelFloatProperty("FarBlurDist", m_pIcon->PropertyFarBlurDistance() );
	this->AddChannel(m_pFarBlurDistanceChannel);
	m_pNearClipChannel = new tmlnChannelFloatProperty("NearClip", m_pIcon->PropertyNearClip() );
	this->AddChannel(m_pNearClipChannel);
	m_pFarClipChannel = new tmlnChannelFloatProperty("FarClip", m_pIcon->PropertyFarClip() );
	this->AddChannel(m_pFarClipChannel);
	m_pDOFMaxFarBlurChannel = new tmlnChannelFloatProperty("DOFMaxFarBlur", m_pIcon->PropertyDOFMaxFarBlur() );
	this->AddChannel(m_pDOFMaxFarBlurChannel);
	m_pHDRMiddleGrayChannel = new tmlnChannelFloatProperty("HDRMiddleGray", m_pIcon->PropertyHDRMiddleGray() );
	this->AddChannel(m_pHDRMiddleGrayChannel);
	m_pHDRBloomScaleChannel = new tmlnChannelFloatProperty("HDRBloomScale", m_pIcon->PropertyHDRBloomScale() );
	this->AddChannel(m_pHDRBloomScaleChannel);
	m_pHDRStarScaleChannel = new tmlnChannelFloatProperty("HDRStarScale", m_pIcon->PropertyHDRStarScale() );
	this->AddChannel(m_pHDRStarScaleChannel);
	m_pHDRBrightPassThreshChannel = new tmlnChannelFloatProperty("HDRBrightPassThresh", m_pIcon->PropertyHDRBrightPassThresh() );
	this->AddChannel(m_pHDRBrightPassThreshChannel);
	m_pHDRBrightPassOffsetChannel = new tmlnChannelFloatProperty("HDRBrightPassOffset", m_pIcon->PropertyHDRBrightPassOffset() );
	this->AddChannel(m_pHDRBrightPassOffsetChannel);
	m_pHDRWhiteCutoffChannel = new tmlnChannelFloatProperty("HDRWhiteCutoff", m_pIcon->PropertyHDRWhiteCutoff() );
	this->AddChannel(m_pHDRWhiteCutoffChannel);
	m_pHDRSceneLuminanceChannel = new tmlnChannelFloatProperty("HDRSceneLuminance", m_pIcon->PropertyHDRSceneLuminance() );
	this->AddChannel(m_pHDRSceneLuminanceChannel);

	// Set starting values from passed in data
	this->SetScriptData(i_Data);

	// Add name callback in order to notify the layer manager
	m_pIcon->PropertyName().AddCallback(new prtyCallbackWrapper<cmraScriptObject>(this, &cmraScriptObject::NameChanged));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmraScriptObject::~cmraScriptObject()
{
	// Unregister connections
	//
	tmlnTimelineMgr::RemoveObject(this);
	grpsGroupMgr::RemoveObject(m_pIcon, m_pIcon);
	lyerLayerMgr::RemoveObject(m_pIcon, this);

	//	delete the cmraObject
	//
	delete m_pIcon;
}


//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string cmraScriptObject::GetTmlnName() const
{
	return m_pIcon->GetName().GetString();
}


//--------------------------------------------------------------------
// Get values as a camera data structure
//--------------------------------------------------------------------
cmraScriptData cmraScriptObject::GetScriptData() const
{
	cmraScriptData data(this->GetBaseData());

	// get driver info
	tmlnCreator::GetDriverInfo(this, (data.m_Drivers));

	// get the channel info
	this->GetChannelSet().GetChannelInfo( data.m_ChannelInfo );

	return data;
}

//--------------------------------------------------------------------
// Set from camera data structure
//--------------------------------------------------------------------
void cmraScriptObject::SetScriptData(const cmraScriptData &i_Data)
{
	this->SetBaseData(i_Data.m_BaseData);

	// create drivers from info
	//DBG_LOG2("%s about to create %d drivers", this->GetName().GetString().c_str(), i_Data.m_Drivers.size() );
	tmlnCreator::SetDriverInfo( this, (i_Data.m_Drivers) );

	// set the channels 
	this->ChannelSet().SetChannelInfo( i_Data.m_ChannelInfo );

}


//--------------------------------------------------------------------
// Get values as a base data structure
//--------------------------------------------------------------------
cmraCameraData cmraScriptObject::GetBaseData() const
{
	cmraCameraData data( m_pIcon->GetData());

	data.m_Position		= m_pPosChannel->GetOriginalPosition();
	data.m_Target		= m_pTargetChannel->GetOriginalPosition();
	data.m_FOV			= m_pFOVChannel->GetOriginalValue();
	data.m_OrthoWidth	= m_pOrthoWidthChannel->GetOriginalValue();
	data.m_Tilt			= m_pTiltChannel->GetOriginalValue();
//	data.m_bCapture		= m_pCaptureChannel->GetOriginalValue();
	data.m_NearFocalDistance= m_pNearFocusDistanceChannel->GetOriginalValue();
	data.m_FarFocalDistance = m_pFarFocusDistanceChannel->GetOriginalValue();
	data.m_NearBlurDistance = m_pNearBlurDistanceChannel->GetOriginalValue();
	data.m_FarBlurDistance	= m_pFarBlurDistanceChannel->GetOriginalValue();
	data.m_Near		= m_pNearClipChannel->GetOriginalValue();
	data.m_Far		= m_pFarClipChannel->GetOriginalValue();
	data.m_MaxFarBlur	= m_pDOFMaxFarBlurChannel->GetOriginalValue();
	data.m_HDRMiddleGray	= m_pHDRMiddleGrayChannel->GetOriginalValue();
	data.m_HDRBloomScale	= m_pHDRBloomScaleChannel->GetOriginalValue();
	data.m_HDRStarScale	= m_pHDRStarScaleChannel->GetOriginalValue();
	data.m_HDRBrightPassThresh	= m_pHDRBrightPassThreshChannel->GetOriginalValue();
	data.m_HDRBrightPassOffset	= m_pHDRBrightPassOffsetChannel->GetOriginalValue();
	data.m_HDRWhiteCutoff	= m_pHDRWhiteCutoffChannel->GetOriginalValue();
	data.m_HDRSceneLuminance	= m_pHDRSceneLuminanceChannel->GetOriginalValue();

	return data;
}

//--------------------------------------------------------------------
// Set from base data structure
//--------------------------------------------------------------------
void cmraScriptObject::SetBaseData(const cmraCameraData &i_Data)
{
	// Set data into Icon/Pick object
	m_pIcon->SetData(i_Data);

	//	call functions for all data that sets the object + data
	//
	m_pPosChannel->SetOriginalPosition(i_Data.m_Position.GetValue());
	m_pTargetChannel->SetOriginalPosition(i_Data.m_Target.GetValue());
	m_pFOVChannel->SetOriginalValue(i_Data.m_FOV.GetValue());
	m_pOrthoWidthChannel->SetOriginalValue(i_Data.m_OrthoWidth.GetValue());
	m_pTiltChannel->SetOriginalValue(i_Data.m_Tilt.GetValue());
	//m_pCaptureChannel->SetOriginalValue(i_Data.m_bCapture.GetValue());
	m_pNearFocusDistanceChannel->SetOriginalValue(i_Data.m_NearFocalDistance.GetValue());
	m_pFarFocusDistanceChannel->SetOriginalValue(i_Data.m_FarFocalDistance.GetValue());
	m_pNearBlurDistanceChannel->SetOriginalValue(i_Data.m_NearBlurDistance.GetValue());
	m_pFarBlurDistanceChannel->SetOriginalValue(i_Data.m_FarBlurDistance.GetValue());
	m_pNearClipChannel->SetOriginalValue(i_Data.m_Near.GetValue());
	m_pFarClipChannel->SetOriginalValue(i_Data.m_Far.GetValue());
	m_pDOFMaxFarBlurChannel->SetOriginalValue(i_Data.m_MaxFarBlur.GetValue());
	m_pHDRMiddleGrayChannel->SetOriginalValue(i_Data.m_HDRMiddleGray.GetValue());
	m_pHDRBloomScaleChannel->SetOriginalValue(i_Data.m_HDRBloomScale.GetValue());
	m_pHDRStarScaleChannel->SetOriginalValue(i_Data.m_HDRStarScale.GetValue());
	m_pHDRBrightPassThreshChannel->SetOriginalValue(i_Data.m_HDRBrightPassThresh.GetValue());
	m_pHDRBrightPassOffsetChannel->SetOriginalValue(i_Data.m_HDRBrightPassOffset.GetValue());
	m_pHDRWhiteCutoffChannel->SetOriginalValue(i_Data.m_HDRWhiteCutoff.GetValue());
	m_pHDRSceneLuminanceChannel->SetOriginalValue(i_Data.m_HDRSceneLuminance.GetValue());

}

//--------------------------------------------------------------------
// Accessors to channels
//--------------------------------------------------------------------
tmlnChannelPosition& cmraScriptObject::PositionChannel()
{
	return (*m_pPosChannel);
}
tmlnChannelPosition& cmraScriptObject::TargetChannel()
{
	return (*m_pTargetChannel);
}
tmlnChannelFloat& cmraScriptObject::FOVChannel()
{
	return (*m_pFOVChannel);
}
tmlnChannelFloat& cmraScriptObject::OrthoWidthChannel()
{
	return (*m_pOrthoWidthChannel);
}
tmlnChannelFloat& cmraScriptObject::TiltChannel()
{
	return (*m_pTiltChannel);
}
cmraChannelCapture& cmraScriptObject::CaptureChannel()
{
	return (*m_pCaptureChannel);
}
tmlnChannelFloatProperty& cmraScriptObject::NearFocusDistanceChannel()
{
	return (*m_pNearFocusDistanceChannel);
}
tmlnChannelFloatProperty& cmraScriptObject::FarFocusDistanceChannel()
{
	return (*m_pFarFocusDistanceChannel);
}
tmlnChannelFloatProperty& cmraScriptObject::NearBlurDistanceChannel()
{
	return (*m_pNearBlurDistanceChannel);
}
tmlnChannelFloatProperty& cmraScriptObject::FarBlurDistanceChannel()
{
	return (*m_pFarBlurDistanceChannel);
}
tmlnChannelFloat& cmraScriptObject::NearClipChannel()
{
	return (*m_pNearClipChannel);
}
tmlnChannelFloat& cmraScriptObject::FarClipChannel()
{
	return (*m_pFarClipChannel);
}
tmlnChannelFloat& cmraScriptObject::DOFMaxFarBlurChannel()
{
	return (*m_pDOFMaxFarBlurChannel);
}
tmlnChannelFloat& cmraScriptObject::HDRMiddleGrayChannel()
{
	return (*m_pHDRMiddleGrayChannel);
}
tmlnChannelFloat& cmraScriptObject::HDRBloomScaleChannel()
{
	return (*m_pHDRBloomScaleChannel);
}
tmlnChannelFloat& cmraScriptObject::HDRStarScaleChannel()
{
	return (*m_pHDRStarScaleChannel);
}
tmlnChannelFloat& cmraScriptObject::HDRBrightPassThreshChannel()
{
	return (*m_pHDRBrightPassThreshChannel);
}
tmlnChannelFloat& cmraScriptObject::HDRBrightPassOffsetChannel()
{
	return (*m_pHDRBrightPassOffsetChannel);
}
tmlnChannelFloat& cmraScriptObject::HDRWhiteCutoffChannel()
{
	return (*m_pHDRWhiteCutoffChannel);
}
tmlnChannelFloat& cmraScriptObject::HDRSceneLuminanceChannel()
{
	return (*m_pHDRSceneLuminanceChannel);
}

//--------------------------------------------------------------------
//	Name - passes functions on to pick object
//--------------------------------------------------------------------
void cmraScriptObject::SetName(const nameString& i_Name)
{
    m_pIcon->SetName( i_Name );
}
const nameString& cmraScriptObject::GetName() const
{
	return m_pIcon->GetName();
}

//----------------------------------------------------------------------------
//	RayPick returns true if the given ray intersects the cmraScriptObject.
//	If it does, the t value is also returned in o_T.
//----------------------------------------------------------------------------
bool cmraScriptObject::RayPick(	const maPoint3d& i_RayStart,
								const maPoint3d& i_RayEnd,
								float& o_T)
{
	if (!this->GetLayerPickable())
		return false;

	return m_pIcon->RayPick(i_RayStart, i_RayEnd, o_T);
}

//--------------------------------------------------------------------
//	ShowIcons - show icons for the light and drivers
//--------------------------------------------------------------------
void cmraScriptObject::ShowIcons(bool i_bVisible)
{
	m_pIcon->SetRenderable(i_bVisible);

	m_bDebugRenderable = i_bVisible;
	this->ShowDriverIcons(m_bDebugRenderable 
		&& m_pIcon->GetEditorVisible() 
		&& this->GetLayerPickable()
		 && m_bSelected);
}

//--------------------------------------------------------------------
//	Set whether this object is selected in order to control
//	display of icons or render style, etc.
//--------------------------------------------------------------------
void cmraScriptObject::SetSelected(bool i_bSelected)
{
	m_bSelected = i_bSelected;
	bool editor_visible = m_pIcon->GetEditorVisible();
	this->ShowDriverIcons(m_bDebugRenderable && editor_visible 
		&& this->GetLayerPickable() && m_bSelected);
}

//--------------------------------------------------------------------
//  Changes visible state of light 
//--------------------------------------------------------------------
void  cmraScriptObject::SetEditorVisible(bool i_bVisible)
{
	m_pIcon->SetEditorVisible(i_bVisible);
	this->ShowDriverIcons(i_bVisible && m_bDebugRenderable 
		&& this->GetLayerPickable() && m_bSelected);
}

//--------------------------------------------------------------------
//	LayerVisible represents if objects in the layer are visible
//--------------------------------------------------------------------
//virtual 
void cmraScriptObject::SetLayerVisible(bool i_bVisible)
{
	// base function sets private flag
	lyerObject::SetLayerVisible(i_bVisible);

	// object is visible only if channel and gui and layer are all
	// are set visible == true;
	m_pIcon->SetLayerVisible(i_bVisible);

}

//--------------------------------------------------------------------
//	LayerPickable represents if objects in the layer can be picked.
//--------------------------------------------------------------------
//virtual 
void cmraScriptObject::SetLayerPickable(bool i_bPickable)
{
	// base function sets private flag
	lyerObject::SetLayerPickable(i_bPickable);

	// Set the pickable state for the camera's icons
	m_pIcon->SetGPUPickable(i_bPickable);

	// driver icons should only be shown when the object is pickable
	this->ShowDriverIcons( m_bDebugRenderable 
						&& m_pIcon->GetEditorVisible() 
						&& this->GetLayerPickable()
						 && m_bSelected);
}

//--------------------------------------------------------------------
//	LayerWireframe represents if the objects are rendered
//	in a wireframe style.
//--------------------------------------------------------------------
//virtual 
void cmraScriptObject::SetLayerWireframe(bool i_bWireframe)
{
	// No wireframe mode for cameras needed.
//	m_pIcon->SetWireframe(i_bWireframe);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraCameraObject* cmraScriptObject::GetPickObject() const
{
	return m_pIcon;
}

//--------------------------------------------------------------------
// This is called when a driver in this script object is changed,
//	or if a driver is added or deleted from this object.
//--------------------------------------------------------------------
void cmraScriptObject::NotifyDriverChanged()
{
	cmraOperations::ChangeDriverData(this->GetScriptData());
}

//--------------------------------------------------------------------
// Accessor to camera
//--------------------------------------------------------------------
const camCamera&	cmraScriptObject::GetCamera() const
{
	return m_pIcon->GetCamera();
}
camCamera&	cmraScriptObject::Camera()
{
	return m_pIcon->Camera();
}
shared_ptr<camCamera> cmraScriptObject::GetCameraPtr()
{
	return m_pIcon->GetCameraPtr();
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void cmraScriptObject::NameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// Update LayerMgr
	lyerLayerMgr::ObjectRenamed(m_pIcon);
	grpsGroupMgr::ObjectRenamed(m_pIcon);
}