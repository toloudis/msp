/*****************************************************************************
**  cmraScriptObject.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Object/cmraScriptObject.hpp"

#include "Systems/Cameras/Object/cmraCameraObject.hpp"
#include "Systems/Cameras/Timeline/cmraChannelCapture.hpp"
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"
#include "Systems/Cameras/Object/cmraIconObject.hpp"
#include "Systems/Cameras/Undo/cmraOperations.hpp"

#include "Systems/Common/Object/cmmNamedScriptObjectReference.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/geo/geoRayIntersection.hpp"
#include "Support/grps/grpsGroupMgr.hpp"
#include "Support/lyer/lyerLayerMgr.hpp"
#include "Core/ma/maConstants.hpp"
#include "Support/rlyr/rlyrRenderLayerMgr.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"
#include "Support/tmln/tmlnChannelTextureFileName.hpp"
#include "Support/tmln/tmlnChannelRenderPass.hpp"

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
	this->SetPropertyObject(*m_pIcon);
	//m_pIcon->SetName( i_CamName );

	// Timeline manager registration
	tmlnTimelineMgr::AddObject(this, "Camera" );

	// Layer manager registration
	lyerLayerMgr::AddObject(m_pIcon, this);
	grpsGroupMgr::AddObject(m_pIcon, m_pIcon);
	//rlyrRenderLayerMgr::AddRenderCam(m_pIcon);
	// Create and add channels
	//
	m_pPosChannel = new tmlnChannelPositionProperty(m_pIcon->PropertyPosition() );
	this->AddChannel(m_pPosChannel);
	m_pTargetChannel = new tmlnChannelPositionProperty(m_pIcon->PropertyTarget() );
	this->AddChannel(m_pTargetChannel);
	m_pFOVChannel = new tmlnChannelFloatProperty(m_pIcon->PropertyFieldOfView() );
	this->AddChannel(m_pFOVChannel);
	m_pOrthoWidthChannel = new tmlnChannelFloatProperty(m_pIcon->PropertyOrthoWidth() );
	this->AddChannel(m_pOrthoWidthChannel);
	m_pNearClipChannel = new tmlnChannelFloatProperty( m_pIcon->PropertyNearClip() );
	this->AddChannel(m_pNearClipChannel);
	m_pFarClipChannel = new tmlnChannelFloatProperty( m_pIcon->PropertyFarClip() );
	this->AddChannel(m_pFarClipChannel);
	m_pTiltChannel = new tmlnChannelFloatProperty(m_pIcon->PropertyTilt() );
	this->AddChannel(m_pTiltChannel);
	m_pFocalLengthChannel = new tmlnChannelFloatProperty( m_pIcon->PropertyFocalLength() );
	this->AddChannel(m_pFocalLengthChannel);
	m_pHorizontalApertureChannel = new tmlnChannelFloatProperty( m_pIcon->PropertyHorizontalAperture() );
	this->AddChannel(m_pHorizontalApertureChannel);
	m_pCaptureChannel = new cmraChannelCapture("Capture");
	this->AddChannel(m_pCaptureChannel);

	m_pRenderPassAOChannel = new tmlnChannelRenderPassProperty( m_pIcon->PropertyTextureFileNameAO(),
																m_pIcon->PropertyIntensityAO(), 
																m_pIcon->PropertyBlendOpAO() );
	this->AddChannel(m_pRenderPassAOChannel);

	m_pRenderPassGIChannel = new tmlnChannelRenderPassProperty( m_pIcon->PropertyTextureFileNameGI(),
																m_pIcon->PropertyIntensityGI(), 
																m_pIcon->PropertyBlendOpGI() );
	this->AddChannel(m_pRenderPassGIChannel);

	m_pRenderPassReflChannel = new tmlnChannelRenderPassProperty( m_pIcon->PropertyTextureFileNameRefl(),
																  m_pIcon->PropertyIntensityRefl(), 
																  m_pIcon->PropertyBlendOpRefl() );
	this->AddChannel(m_pRenderPassReflChannel);

	m_pRenderPassShadowMaskChannel = new tmlnChannelRenderPassProperty( m_pIcon->PropertyTextureFileNameShadowMask(),
																		m_pIcon->PropertyIntensityShadowMask(), 
																		m_pIcon->PropertyBlendOpShadowMask() );
	this->AddChannel(m_pRenderPassShadowMaskChannel);

	m_pRenderPassBeautyChannel = new tmlnChannelRenderPassProperty( m_pIcon->PropertyTextureFileNameBeauty(),
																	m_pIcon->PropertyIntensityBeauty(), 
																	m_pIcon->PropertyBlendOpBeauty() );
	this->AddChannel(m_pRenderPassBeautyChannel);

	m_pFStopChannel = new tmlnChannelFloatProperty( m_pIcon->PropertyFStop() );
	this->AddChannel(m_pFStopChannel);
	m_pFocalDistanceChannel = new tmlnChannelFloatProperty( m_pIcon->PropertyFocalDistance() );
	this->AddChannel(m_pFocalDistanceChannel);
	m_pNearBlurDistanceChannel = new tmlnChannelFloatProperty( m_pIcon->PropertyNearBlurDistance() );
	this->AddChannel(m_pNearBlurDistanceChannel);
	m_pNearFocusDistanceChannel = new tmlnChannelFloatProperty( m_pIcon->PropertyNearFocusDistance() );
	this->AddChannel(m_pNearFocusDistanceChannel);
	m_pCoCChannel =  new tmlnChannelFloatProperty( m_pIcon->PropertyCoC() );
	this->AddChannel(m_pCoCChannel);
	m_pFarFocusDistanceChannel = new tmlnChannelFloatProperty( m_pIcon->PropertyFarFocusDistance() );
	this->AddChannel(m_pFarFocusDistanceChannel);
	m_pFarBlurDistanceChannel = new tmlnChannelFloatProperty( m_pIcon->PropertyFarBlurDistance() );
	this->AddChannel(m_pFarBlurDistanceChannel);
	m_pDOFMaxFarBlurChannel = new tmlnChannelFloatProperty( m_pIcon->PropertyDOFMaxFarBlur() );
	this->AddChannel(m_pDOFMaxFarBlurChannel);
	m_pDOFMaxCoCChannel = new tmlnChannelFloatProperty( m_pIcon->PropertyMaxCoC() );
	this->AddChannel(m_pDOFMaxCoCChannel);
	m_pHDRMiddleGrayChannel = new tmlnChannelFloatProperty(m_pIcon->PropertyHDRMiddleGray() );
	this->AddChannel(m_pHDRMiddleGrayChannel);
	m_pHDRWhiteCutoffChannel = new tmlnChannelFloatProperty( m_pIcon->PropertyHDRWhiteCutoff() );
	this->AddChannel(m_pHDRWhiteCutoffChannel);
	m_pHDRSceneLuminanceChannel = new tmlnChannelFloatProperty( m_pIcon->PropertyHDRSceneLuminance() );
	this->AddChannel(m_pHDRSceneLuminanceChannel);
	m_pHDRBloomScaleChannel = new tmlnChannelFloatProperty( m_pIcon->PropertyHDRBloomScale() );
	this->AddChannel(m_pHDRBloomScaleChannel);
	m_pHDRStarScaleChannel = new tmlnChannelFloatProperty( m_pIcon->PropertyHDRStarScale() );
	this->AddChannel(m_pHDRStarScaleChannel);
	m_pHDRBrightPassThreshChannel = new tmlnChannelFloatProperty( m_pIcon->PropertyHDRBrightPassThresh() );
	this->AddChannel(m_pHDRBrightPassThreshChannel);
	m_pHDRBrightPassOffsetChannel = new tmlnChannelFloatProperty( m_pIcon->PropertyHDRBrightPassOffset() );
	this->AddChannel(m_pHDRBrightPassOffsetChannel);
	m_pStereoFDChannel = new tmlnChannelFloatProperty( m_pIcon->PropertyStereoFD() );
	this->AddChannel(m_pStereoFDChannel);
	m_pStereoIODChannel = new tmlnChannelFloatProperty( m_pIcon->PropertyStereoIOD() );
	this->AddChannel(m_pStereoIODChannel);

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
	//rlyrRenderLayerMgr::RemoveRenderCam(m_pIcon);
	//	delete the cmraObject
	//
	delete m_pIcon;
}


//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string cmraScriptObject::GetDisplayName() const
{
	return m_pIcon->GetName().GetString();
}

//--------------------------------------------------------------------
//	CreateReferenceToSelf - create a reference that refers to the
//	given object. This reference should be able to refer to 
//  future instances of this object persistent through deletion
//	and recreation through undo.
//--------------------------------------------------------------------
shared_ptr<relObjectReference> cmraScriptObject::CreateReferenceToSelf()
{
	shared_ptr<relObjectReference> named_reference(new cmmNamedScriptObjectReference<cmraObjectMgr>(this->GetName()));
	return named_reference;
}

//--------------------------------------------------------------------
// Get values as a camera data structure
//--------------------------------------------------------------------
cmraScriptData cmraScriptObject::GetScriptData() const
{
	cmraScriptData data(this->GetBaseData());

	// get custom property data
	this->GetCustomPropertyData(data.m_CustomProperties);

	// get driver info
	tmlnCreator::GetDriverInfo(this, (data.m_Drivers));

	// get the channel info
	this->GetChannelSet().GetChannelInfo( data.m_ChannelInfo );

	// get the connection info
	lyerLayerMgr::GetLayerNameFromObject(data.m_BaseData.m_Name.GetValue(), data.m_ConnectionData.m_LayerName);
	grpsGroupMgr::GetGroupsForObject(data.m_BaseData.m_Name.GetValue(), data.m_ConnectionData.m_GroupNames);

	return data;
}

//--------------------------------------------------------------------
// Set from camera data structure
//--------------------------------------------------------------------
void cmraScriptObject::SetScriptData(const cmraScriptData &i_Data)
{
	this->SetBaseData(i_Data.m_BaseData);

	// set custom property data
	this->SetCustomPropertyData(i_Data.m_CustomProperties);

	// create drivers from info
	//DBG_LOG2("%s about to create %d drivers", this->GetName().GetString().c_str(), i_Data.m_Drivers.size() );
	tmlnCreator::SetDriverInfo( this, (i_Data.m_Drivers) );

	// set the channels 
	this->ChannelSet().SetChannelInfo( i_Data.m_ChannelInfo );

	// set the connection info, if set
	if (!i_Data.m_ConnectionData.m_LayerName.IsEmpty())
		lyerLayerMgr::AddObjectToLayer(i_Data.m_ConnectionData.m_LayerName, i_Data.m_BaseData.m_Name.GetValue());

	const int num_groups = i_Data.m_ConnectionData.m_GroupNames.size();
	for (int g=0; g<num_groups; ++g)
		grpsGroupMgr::AddObjectToGroup(i_Data.m_ConnectionData.m_GroupNames[g], i_Data.m_BaseData.m_Name.GetValue());

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
	data.m_MaxCoC = m_pDOFMaxCoCChannel->GetOriginalValue();

	data.m_StereoFD = m_pStereoFDChannel->GetOriginalValue();
	data.m_StereoIOD = m_pStereoIODChannel->GetOriginalValue();

	data.m_FocalLength = m_pFocalLengthChannel->GetOriginalValue();
	data.m_HorizontalAperture = m_pHorizontalApertureChannel->GetOriginalValue();
	data.m_Fstop = m_pFStopChannel->GetOriginalValue();
	data.m_FocalDistance = m_pFocalDistanceChannel->GetOriginalValue();
	data.m_CoC = m_pCoCChannel->GetOriginalValue();

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
	m_pDOFMaxCoCChannel->SetOriginalValue(i_Data.m_MaxCoC.GetValue());

	m_pStereoFDChannel->SetOriginalValue(i_Data.m_StereoFD.GetValue());
	m_pStereoIODChannel->SetOriginalValue(i_Data.m_StereoIOD.GetValue());
		
	m_pFocalLengthChannel->SetOriginalValue(i_Data.m_FocalLength.GetValue());
	m_pHorizontalApertureChannel->SetOriginalValue(i_Data.m_HorizontalAperture.GetValue());
	m_pFStopChannel->SetOriginalValue(i_Data.m_Fstop.GetValue());
	m_pFocalDistanceChannel->SetOriginalValue(i_Data.m_FocalDistance.GetValue());
	m_pCoCChannel->SetOriginalValue(i_Data.m_CoC.GetValue());
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
tmlnChannelRenderPass& cmraScriptObject::RenderPassAOChannel()
{
	return (*m_pRenderPassAOChannel);
}
tmlnChannelRenderPass& cmraScriptObject::RenderPassGIChannel()
{
	return (*m_pRenderPassGIChannel);
}
tmlnChannelRenderPass& cmraScriptObject::RenderPassReflChannel()
{
	return (*m_pRenderPassReflChannel);
}
tmlnChannelRenderPass& cmraScriptObject::RenderPassShadowMaskChannel()
{
	return (*m_pRenderPassShadowMaskChannel);
}
tmlnChannelRenderPass& cmraScriptObject::RenderPassBeautyChannel()
{
	return (*m_pRenderPassBeautyChannel);
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
tmlnChannelFloat& cmraScriptObject::DOFMaxCoCChannel()
{
	return (*m_pDOFMaxCoCChannel);
}
tmlnChannelFloat& cmraScriptObject::StereoFDChannel()
{
	return (*m_pStereoFDChannel);
}
tmlnChannelFloat& cmraScriptObject::StereoIODChannel()
{
	return (*m_pStereoIODChannel);
}
tmlnChannelFloat& cmraScriptObject::FocalLengthChannel()
{
	return (*m_pFocalLengthChannel);
}
tmlnChannelFloat& cmraScriptObject::HorizontalApertureChannel()
{
	return (*m_pHorizontalApertureChannel);
}
tmlnChannelFloat& cmraScriptObject::FStopChannel()
{
	return (*m_pFStopChannel);
}
tmlnChannelFloat& cmraScriptObject::FocusDistanceChannel()
{
	return (*m_pFocalDistanceChannel);
}
tmlnChannelFloat& cmraScriptObject::CoCChannel()
{
	return (*m_pCoCChannel);
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
// Set object active state in the render layer
//--------------------------------------------------------------------
void cmraScriptObject::SetActiveRenderLayer(bool i_bActive)
{
	m_pIcon->SetActiveRenderLayer(i_bActive);
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
//  Returns the visible state of light 
//--------------------------------------------------------------------
bool  cmraScriptObject::GetEditorVisible()
{
	return m_pIcon->GetEditorVisible();
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
shared_ptr<gpxCamera> cmraScriptObject::GetCameraProxyPtr()
{
	return m_pIcon->GetCameraProxyPtr();
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void cmraScriptObject::NameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	//nameObject* pNO = nameMgr::GetObjectByName( nameStr );

	//if (pNO == NULL)
	//	// undo the name change

	// Update LayerMgr
	lyerLayerMgr::ObjectRenamed(m_pIcon);
	grpsGroupMgr::ObjectRenamed(m_pIcon);
	rlyrRenderLayerMgr::ObjectRenamed(m_pIcon);
}

//--------------------------------------------------------------------
// Get the number of capture drivers
//--------------------------------------------------------------------
int cmraScriptObject::GetNumCaptureDrivers()
{
	return m_pCaptureChannel->GetNumDrivers();
}