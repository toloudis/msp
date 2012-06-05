/*****************************************************************************
**  cmraCameraObject.cpp
**
**      A cmraCameraObject is a derived class for displaying a point
**	camera's position.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Object/cmraCameraObject.hpp"

#include "Systems/Cameras/GUI/cmraDialogDataUtil.hpp"
#include "Systems/Cameras/GUI/cmraDialogUtil.hpp"
#include "Systems/Cameras/Data/cmraDocumentChunk.hpp"
#include "Systems/Cameras/Object/cmraIconObject.hpp"
#include "Systems/Cameras/Undo/cmraOperations.hpp"

//#include "Features/RenderPrefs/rndrPrefsMgr.hpp"
#include "Support/mnm/mnmDebugInfo.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"
#include "Support/cams/camsCameraMgr.hpp"

#include "MainApp/mnmApp.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/geo/geoRayIntersection.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
//#include "Graphics/g3d/g3dSceneRendererCreate.hpp"
#include "GraphicsDX9/g3d/g3dSceneGlobal.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"


namespace
{
	const float l_SphereRadius	= 0.5f;
	const float l_PickRadius	= 1.0f;	// pick larger than icon
	const maAxisBox l_SphereBox(-l_SphereRadius, l_SphereRadius,
								-l_SphereRadius, l_SphereRadius,
								-l_SphereRadius, l_SphereRadius);
	const maVector3d l_Zero(0,0,0);
	const maVector3d l_One(1,1,1);
	const maRotation l_NoRot;

	enum ManipModes
	{
		e_Eye = 0,
		e_Target = 1,
		e_Together = 2
	};

	//--------------------------------------------------------------------
	// Convert from a vector to pitch yaw and distance
	//--------------------------------------------------------------------
	void convert_pitch_yaw(const maVector3d &i_Vector,
							float &o_Pitch,
							float &o_Yaw,
							float &o_Distance)
	{
		o_Distance = i_Vector.Length();

		if (o_Distance > 0.0f)
		{
			o_Yaw = float(::atan2f(i_Vector.m_X, i_Vector.m_Z));
			float zx_len = sqrtf( i_Vector.m_Z * i_Vector.m_Z + i_Vector.m_X * i_Vector.m_X );
			o_Pitch = float(::atan2f(i_Vector.m_Y, zx_len));
		}
		else
		{
			o_Yaw = o_Pitch = 0.0f;
		}
	}

	//--------------------------------------------------------------------
	// Convert from pitch, yaw distance to a vector
	//--------------------------------------------------------------------
	void convert_pitch_yaw(	float i_Pitch,
							float i_Yaw,
							float i_Distance,
							maVector3d &o_Vector)
	{
		float x = float(sin(i_Yaw) * cos(i_Pitch));
		float y = float(sin(i_Pitch));
		float z = float(cos(i_Yaw) * cos(i_Pitch));

		o_Vector.Set( x * i_Distance, y * i_Distance, z * i_Distance);
	}

}


//----------------------------------------------------------------------------
// Constructor - set perspective or orthographic camera here.
//	If i_bUseEditorCamera is true, this object attaches to the
//	cam3dMgr editor camera.
//----------------------------------------------------------------------------
cmraCameraObject::cmraCameraObject(bool i_bOrthographic,
								   bool i_bUseEditorCamera)
:	m_WorldBox(l_SphereBox),
	m_pParent(NULL),
	m_pObject(NULL), 
	m_pTargetObject(NULL),
	m_bRenderable(true),
	m_bLayerVisible(true),
//	m_pAspectRatioControl(NULL),
	m_Pitch("Pitch", 0.0f),
	m_Yaw("Yaw", 0.0f),
	m_ManipMode("Manip Mode", e_Eye)
{
	// Set up our camera pointer. Shared pointer is either ours or
	// the cam3dMgr's editor camera.
	if (i_bUseEditorCamera)
		m_Camera = cam3dMgr::GetEditorCameraPtr();
	else
		m_Camera.reset(new camCamera);

	// Default value for match is true for cameras we control
	m_Camera->SetMatchAspectToWindow(true);

	// Setup camera as orthographic based on parameter
	m_Camera->SetOrthographic(i_bOrthographic);
	m_Data.m_bOrthographic = i_bOrthographic;

	// Set up property ranges
	m_Pitch.SetMaximum(90.0f);
	m_Pitch.SetMinimum(-90.0f);
	m_Yaw.SetMaximum(180.0f);
	m_Yaw.SetMinimum(-180.0f);

	// Set up enumeration for manip mode
	m_ManipMode.SetEnumTag(e_Eye,"Eye");
	m_ManipMode.SetEnumTag(e_Target,"Target");
	m_ManipMode.SetEnumTag(e_Together,"Together");

	// Editor camera doesn't have icon
	if (!i_bUseEditorCamera)
	{
		// Create 3D icon
		//m_pObject = api3dShape::CreateSphere(maFloatRGBA(1,0,0,1), l_SphereRadius, 8, 8);
		m_pObject = new cmraIconObject();
		m_pObject->SetRenderable(true);

		m_pTargetObject = create_line();
		m_pTargetObject->SetGPUPickable(false);
		api3dScene::AddObject(m_pTargetObject, mnmApp::GetIconsLayerIndex());
	}

	// Add callback for when manipulator changes our camera
	m_Camera->AddCameraChangedCallback(this);

	api3dScale::RegisterScaleInterest(this);

	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_Position), "Transform", "Position of the object");
	AddProperty( pPUII );
	pPUII  = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_Target), "Transform", "Target of the object");
	AddProperty( pPUII );
	// cameras don't use orientation, just position and target
	//pPUII  = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_Orientation), "category2", "Orientation of the object");
	//AddProperty( pPUII );

	// Don't allow name or description altering on editor camera
	if (!i_bUseEditorCamera)
	{
		pPUII = new prtyTextBoxUIInfo(&(m_Data.m_Name), "Asset", "Name of the object");
		AddProperty( pPUII );
		pPUII = new prtyTextBoxUIInfo(&(m_Data.m_Description), "Asset", "Description of the object");
		AddProperty( pPUII );
	}

	if (i_bOrthographic)
	{
		pPUII  = new prtyNumericUpDownUIInfo(&(m_Data.m_OrthoWidth), "View", "Field of View");
		AddProperty( pPUII );
	}
	else
	{
		pPUII  = new prtyNumericUpDownUIInfo(&(m_Data.m_FOV), "View", "Field of View");
		AddProperty( pPUII );
	}

	// AspectRatio
	//pPUII  = new prtyCheckBoxUIInfo(&(m_Data.m_bMatchAspectToWindow), "View", "Aspect ratio of camera should match window");
	//AddProperty( pPUII );
	//m_pAspectRatioControl  = new prtyNumericUpDownUIInfo(&(m_Data.m_AspectRatio), "View", "Film Aspect Ratio");
	//m_pAspectRatioControl->SetIncrement(0.01f);
	//m_pAspectRatioControl->SetReadOnly(true);
	//AddProperty( m_pAspectRatioControl );

	pPUII = new prtyComboBoxUIInfo(&(m_ManipMode), "View Angles", "Manipulations center on eye or target");
	AddProperty( pPUII );
	prtyRangedFloatUIInfo* pRFUII  = new prtyRangedFloatUIInfo(&(m_Pitch), "View Angles", "Pitch angle of view direction");
	pRFUII->SetMinimum(-89.9f);
	pRFUII->SetMaximum(89.9f);
	pRFUII->SetDecimalPlaces(1);
	pRFUII->SetNumTicks(180);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Yaw), "View Angles", "Yaw angle of view direction");
	pRFUII->SetMinimum(-180.0f);
	pRFUII->SetMaximum(180.0f);
	pRFUII->SetDecimalPlaces(1);
	pRFUII->SetNumTicks(90);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Tilt), "View Angles", "Tilt angle around view direction");
	pRFUII->SetMinimum(-180.0f);
	pRFUII->SetMaximum(180.0f);
	pRFUII->SetDecimalPlaces(1);
	pRFUII->SetNumTicks(90);
	AddProperty( pRFUII );
	
	pPUII  = new prtyNumericUpDownUIInfo(&(m_Data.m_Near), "View", "Near Clipping plane");
	AddProperty( pPUII );
	pPUII  = new prtyNumericUpDownUIInfo(&(m_Data.m_Far), "View", "Far Clipping plane");
	AddProperty( pPUII );
	pPUII  = new prtyCheckBoxUIInfo(&(m_Data.m_bEnableDOF), "Depth of Field", "Enable Depth of Field?");
	AddProperty( pPUII );
	pPUII  = new prtyNumericUpDownUIInfo(&(m_Data.m_NearBlurDistance), "Depth of Field", "Near Blur Distance");
	AddProperty( pPUII );
	pPUII  = new prtyNumericUpDownUIInfo(&(m_Data.m_NearFocalDistance), "Depth of Field", "Near Focal Distance");
	AddProperty( pPUII );
	pPUII  = new prtyNumericUpDownUIInfo(&(m_Data.m_FarFocalDistance), "Depth of Field", "Far Focal Distance");
	AddProperty( pPUII );
	pPUII  = new prtyNumericUpDownUIInfo(&(m_Data.m_FarBlurDistance), "Depth of Field", "Far Blur Distance");
	AddProperty( pPUII );
	pPUII  = new prtyNumericUpDownUIInfo(&(m_Data.m_MaxFarBlur), "Depth of Field", "Maximum Far Blurriness");
	AddProperty( pPUII );

	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_HDRMiddleGray), "HDR", "HDR Middle Gray");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	pRFUII->SetDecimalPlaces(3);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_HDRWhiteCutoff), "HDR", "Lowest luminance which is mapped to white");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(20.0f);
	AddProperty( pRFUII );

	prtyNumericUpDownUIInfo* pNUII  = new prtyNumericUpDownUIInfo(&(m_Data.m_HDRSceneLuminance), "HDR", "tone map using this value as avg scene luminance");
	pNUII->SetIncrement(0.05f);
	pNUII->SetMinimum(0.0f);
	pNUII->SetMaximum(1000);
	AddProperty( pNUII );

	pPUII = new prtyComboBoxUIInfo(&(m_Data.m_HDRStarType), "HDR", "Type of glare effect");
	AddProperty( pPUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_HDRBloomScale), "HDR", "HDR Bloom Scale");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(2.0f);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_HDRStarScale), "HDR", "HDR Star Scale");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(2.0f);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_HDRBrightPassThresh), "HDR", "HDR Bright Pass Threshold");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(10.0f);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_HDRBrightPassOffset), "HDR", "HDR Bright Pass Offset");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(40.0f);
	AddProperty( pRFUII );

	// Register callbacks to update particle generator and icons
	// when properties change
	m_Data.m_Name.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::NameChanged));
	m_Data.m_Position.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::TransformChanged));
	m_Data.m_Target.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::TransformChanged));
	m_Data.m_FOV.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::FOVChanged));
	m_Data.m_Tilt.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::TransformChanged));
	m_Data.m_Near.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::CameraChanged));
	m_Data.m_Far.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::CameraChanged));
	m_Data.m_OrthoWidth.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::OrthoWidthChanged));
	m_Data.m_bEnableDOF.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::DOFChanged));
	m_Data.m_NearBlurDistance.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::DOFChanged));
	m_Data.m_NearFocalDistance.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::DOFChanged));
	m_Data.m_FarFocalDistance.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::DOFChanged));
	m_Data.m_FarBlurDistance.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::DOFChanged));
	m_Data.m_MaxFarBlur.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::DOFChanged));
	m_Data.m_Description.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::DescriptionChanged));
	m_Data.m_HDRMiddleGray.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::CameraChanged));
	m_Data.m_HDRBloomScale.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::CameraChanged));
	m_Data.m_HDRStarScale.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::CameraChanged));
	m_Data.m_HDRBrightPassThresh.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::CameraChanged));
	m_Data.m_HDRBrightPassOffset.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::CameraChanged));
	m_Data.m_HDRWhiteCutoff.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::CameraChanged));
	m_Data.m_HDRStarType.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::CameraChanged));
	m_Data.m_HDRSceneLuminance.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::CameraChanged));
	//m_Data.m_AspectRatio.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::AspectRatioChanged));
	//m_Data.m_bMatchAspectToWindow.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::MatchAspectChanged));

	// Properties just for the interface
	m_Yaw.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::PitchYawChanged));
	m_Pitch.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::PitchYawChanged));
	m_ManipMode.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::ManipModeChanged));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmraCameraObject::~cmraCameraObject()
{
	api3dScale::UnRegisterScaleInterest(this);

	delete m_pObject;

	if (m_pTargetObject)
	{
		api3dScene::RemoveObject(m_pTargetObject, mnmApp::GetIconsLayerIndex());
		delete m_pTargetObject;
	}

	//api3dCameraMgr::DestroyCamera(m_pCamera);
}


//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string cmraCameraObject::GetPick3dName() const
{
	return GetName().GetString();
}

//--------------------------------------------------------------------
//	GlobalScaleChanged - notification that global scale has changed.
//--------------------------------------------------------------------
//virtual 
void cmraCameraObject::GlobalScaleChanged( float i_Scale )
{
	if (m_pObject)
		m_pObject->SetUniformScale( i_Scale );
	update_world_box();
}


//--------------------------------------------------------------------
// Get values as a camera data structure
//--------------------------------------------------------------------
const cmraCameraData& cmraCameraObject::GetData() const
{
	return m_Data;
}

//--------------------------------------------------------------------
// Set from camera data structure
//--------------------------------------------------------------------
void cmraCameraObject::SetData(const cmraCameraData &i_Data)
{
	m_Data = i_Data;
}

//--------------------------------------------------------------------
// Name property access
//--------------------------------------------------------------------
prtyName&	cmraCameraObject::PropertyName()
{
	return m_Data.m_Name;
}
const prtyName&	cmraCameraObject::GetPropertyName() const
{
	return m_Data.m_Name;
}

//--------------------------------------------------------------------
// Description property access
//--------------------------------------------------------------------
prtyText&	cmraCameraObject::PropertyDescription()
{
	return m_Data.m_Description;
}
const prtyText&	cmraCameraObject::GetPropertyDescription() const
{
	return m_Data.m_Description;
}

//--------------------------------------------------------------------
// Position property access
//--------------------------------------------------------------------
prtyPoint3d&	cmraCameraObject::PropertyPosition()
{
	return m_Data.m_Position;
}
const prtyPoint3d&	cmraCameraObject::GetPropertyPosition() const
{
	return m_Data.m_Position;
}

//--------------------------------------------------------------------
// Target property access
//--------------------------------------------------------------------
prtyPoint3d&	cmraCameraObject::PropertyTarget()
{
	return m_Data.m_Target;
}
const prtyPoint3d&	cmraCameraObject::GetPropertyTarget() const
{
	return m_Data.m_Target;
}

//--------------------------------------------------------------------
// FieldOfView property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyFieldOfView()
{
	return m_Data.m_FOV;
}
const prtyFloat&	cmraCameraObject::GetPropertyFieldOfView() const
{
	return m_Data.m_FOV;
}

//--------------------------------------------------------------------
// OrthoWidth property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyOrthoWidth()
{
	return m_Data.m_OrthoWidth;
}
const prtyFloat&	cmraCameraObject::GetPropertyOrthoWidth() const
{
	return m_Data.m_OrthoWidth;
}

//--------------------------------------------------------------------
// Tilt property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyTilt()
{
	return m_Data.m_Tilt;
}
const prtyFloat&	cmraCameraObject::GetPropertyTilt() const
{
	return m_Data.m_Tilt;
}

//--------------------------------------------------------------------
// Near Focus property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyNearFocusDistance()
{
	return m_Data.m_NearFocalDistance;
}
const prtyFloat&	cmraCameraObject::GetPropertyNearFocusDistance() const
{
	return m_Data.m_NearFocalDistance;
}

//--------------------------------------------------------------------
// Far Focus property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyFarFocusDistance()
{
	return m_Data.m_FarFocalDistance;
}
const prtyFloat&	cmraCameraObject::GetPropertyFarFocusDistance() const
{
	return m_Data.m_FarFocalDistance;
}

//--------------------------------------------------------------------
// Far Blur property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyFarBlurDistance()
{
	return m_Data.m_FarBlurDistance;
}
const prtyFloat&	cmraCameraObject::GetPropertyFarBlurDistance() const
{
	return m_Data.m_FarBlurDistance;
}

//--------------------------------------------------------------------
// Far Focus property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyNearBlurDistance()
{
	return m_Data.m_NearBlurDistance;
}
const prtyFloat&	cmraCameraObject::GetPropertyNearBlurDistance() const
{
	return m_Data.m_NearBlurDistance;
}

//--------------------------------------------------------------------
// NearBlur property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyNearClip()
{
	return m_Data.m_Near;
}
const prtyFloat&	cmraCameraObject::GetPropertyNearClip() const
{
	return m_Data.m_Near;
}

//--------------------------------------------------------------------
// FarClip property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyFarClip()
{
	return m_Data.m_Far;
}
const prtyFloat&	cmraCameraObject::GetPropertyFarClip() const
{
	return m_Data.m_Far;
}

//--------------------------------------------------------------------
// DOFMaxFarBlur property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyDOFMaxFarBlur()
{
	return m_Data.m_MaxFarBlur;
}
const prtyFloat&	cmraCameraObject::GetPropertyDOFMaxFarBlur() const
{
	return m_Data.m_MaxFarBlur;
}

//--------------------------------------------------------------------
// HDRMiddleGray property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyHDRMiddleGray()
{
	return m_Data.m_HDRMiddleGray;
}
const prtyFloat&	cmraCameraObject::GetPropertyHDRMiddleGray() const
{
	return m_Data.m_HDRMiddleGray;
}

//--------------------------------------------------------------------
// HDRBloomScale property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyHDRBloomScale()
{
	return m_Data.m_HDRBloomScale;
}
const prtyFloat&	cmraCameraObject::GetPropertyHDRBloomScale() const
{
	return m_Data.m_HDRBloomScale;
}

//--------------------------------------------------------------------
// HDRStarScale property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyHDRStarScale()
{
	return m_Data.m_HDRStarScale;
}
const prtyFloat&	cmraCameraObject::GetPropertyHDRStarScale() const
{
	return m_Data.m_HDRStarScale;
}

//--------------------------------------------------------------------
// HDRBrightPassThresh property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyHDRBrightPassThresh()
{
	return m_Data.m_HDRBrightPassThresh;
}
const prtyFloat&	cmraCameraObject::GetPropertyHDRBrightPassThresh() const
{
	return m_Data.m_HDRBrightPassThresh;
}

//--------------------------------------------------------------------
// HDRBrightPassOffset property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyHDRBrightPassOffset()
{
	return m_Data.m_HDRBrightPassOffset;
}
const prtyFloat&	cmraCameraObject::GetPropertyHDRBrightPassOffset() const
{
	return m_Data.m_HDRBrightPassOffset;
}

//--------------------------------------------------------------------
// HDRWhiteCutoff property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyHDRWhiteCutoff()
{
	return m_Data.m_HDRWhiteCutoff;
}
const prtyFloat&	cmraCameraObject::GetPropertyHDRWhiteCutoff() const
{
	return m_Data.m_HDRWhiteCutoff;
}

//--------------------------------------------------------------------
// HDRSceneLuminance property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyHDRSceneLuminance()
{
	return m_Data.m_HDRSceneLuminance;
}
const prtyFloat&	cmraCameraObject::GetPropertyHDRSceneLuminance() const
{
	return m_Data.m_HDRSceneLuminance;
}

//--------------------------------------------------------------------
//	UpdateIcon() - update the graphical icon using the current
//	cmraCameraObject settings.
//--------------------------------------------------------------------
void cmraCameraObject::UpdateIcon()
{
	if (m_pObject)
	{
		if (m_Data.m_bEnableDOF.GetValue())
		{
			m_pObject->Update( m_Data.m_Position.GetValue(), 
				m_Data.m_Target.GetValue(),
				m_Data.m_Tilt.GetValue(),
				m_Data.m_FOV.GetValue(),
				m_Camera->GetAspect(),
				m_Data.m_NearBlurDistance.GetValue(),
				m_Data.m_NearFocalDistance.GetValue(),
				m_Data.m_FarFocalDistance.GetValue(),
				m_Data.m_FarBlurDistance.GetValue()
				);
		}
		else
		{
			m_pObject->Update( m_Data.m_Position.GetValue(), 
				m_Data.m_Target.GetValue(),
				m_Data.m_Tilt.GetValue(),
				m_Data.m_FOV.GetValue(),
				m_Camera->GetAspect(), 1,1,1,1
				);
		}
	}

	if (m_pTargetObject)
	{
		maVector3d dir( (m_Data.m_Target.GetValue() - m_Data.m_Position.GetValue()) );
		
		//	set the position of the 3-D line
		//
		m_pTargetObject->SetPosition( m_Data.m_Position.GetValue() );

		float len = dir.Length();
		if (len > 0)
			m_pTargetObject->SetScale( maVector3d(1,1,len) );

		//	set the orientation of the 3-D line
		//
		maRotation dir_rot;
		dir_rot.SetValue(maVector3d(0,0,1), dir);
		m_pTargetObject->SetOrientation( dir_rot );
	}
}

//----------------------------------------------------------------------------
//	GetWorldBox returns a box which completely encloses the cmraCameraObject.
//----------------------------------------------------------------------------
const maAxisBox& cmraCameraObject::GetWorldBox() const
{
	return m_WorldBox;
}

//----------------------------------------------------------------------------
//	GetLocalBox returns a box which would enclose the cmraCameraObject
//	if its transformations were identity
//----------------------------------------------------------------------------
const maAxisBox& cmraCameraObject::GetLocalBox() const
{
	return l_SphereBox;
}

//--------------------------------------------------------------------
//	GetWorldPivot returns the point in world space that this
//		object will rotate around. Used to center rotation and
//		scale compasses.
//--------------------------------------------------------------------
//virtual 
maPoint3d cmraCameraObject::GetWorldPivot() const
{
	// pivot point in world space is based on manipulation mode
	if (m_ManipMode.GetValue() == e_Target)
		return m_Data.m_Target.GetValue();
	else
		return m_Data.m_Position.GetValue();
}

//--------------------------------------------------------------------
//	Name
//--------------------------------------------------------------------
void cmraCameraObject::SetName(const nameString& i_Name)
{
    // Set name first, which may change the ID number
    nameObject::SetName(i_Name);

    // Make sure that the data matches our true name (including
    // ID number)
    m_Data.m_Name.SetValue( this->GetName() );
}

//----------------------------------------------------------------------------
//	Position
//----------------------------------------------------------------------------
maPoint3d cmraCameraObject::GetPosition() const
{
	if (m_ManipMode.GetValue() == e_Target)
		return m_Data.m_Target.GetValue();
	else
		return m_Data.m_Position.GetValue();
}

//----------------------------------------------------------------------------
//	UpdatePosition - compass interaction has altered the position of
//	of this camera
//----------------------------------------------------------------------------
void cmraCameraObject::UpdatePosition(const maPoint3d& i_Position, 
										bool i_bNewOperation)
{
	//cmraOperations::ChangePosition( i_Position );
	switch (m_ManipMode.GetValue())
	{
	case e_Eye:
		m_Data.m_Position.SetValue(i_Position, 
			i_bNewOperation ? prtyProperty::eNewUndo : prtyProperty::eContinueUndo);
		break;
	case e_Target:
		m_Data.m_Target.SetValue(i_Position, 
			i_bNewOperation ? prtyProperty::eNewUndo : prtyProperty::eContinueUndo);
		break;
	case e_Together:
	{
		maVector3d diff = m_Data.m_Target.GetValue() - m_Data.m_Position.GetValue();
		m_Data.m_Position.SetValue(i_Position, 
			i_bNewOperation ? prtyProperty::eNewUndo : prtyProperty::eContinueUndo);
		m_Data.m_Target.SetValue(i_Position + diff, prtyProperty::eContinueUndo);
	}
		break;
	}
}

//----------------------------------------------------------------------------
//	Orientation
//----------------------------------------------------------------------------
maRotation cmraCameraObject::GetOrientation() const
{
//	return l_NoRot;

	
	// Construct view matrix
	//maMatrix4x4 cam_matx;
	//cam_matx.LookAt(m_Data.m_Position.GetValue(), m_Data.m_Target.GetValue(), maVector3d(0,1,0));
	//maVector3d cam_up(cam_matx.m_Mat[4], cam_matx.m_Mat[5], cam_matx.m_Mat[6]);

	//maRotation rotx(maVector3d(1,0,0), m_Pitch.GetValue() * maConstants::c_fAngleToRad);
	//maRotation roty(maVector3d(0,1,0), m_Yaw.GetValue() * maConstants::c_fAngleToRad);
	////maRotation rotz(maVector3d(0,0,1), m_Data.m_Tilt.GetValue() * maConstants::c_fAngleToRad);
	//maRotation rot = (roty * rotx);

	//maVector3d rot_up(0,1,0);
	//rot.RotateVector(rot_up);
	//
	//char text[64];
	//sprintf(text, "camera up(%6.3f,%6.3f,%6.3f)", cam_up.GetX(), cam_up.GetY(), cam_up.GetZ()  );
	//mnmDebugInfo::SetDebugInfo(20, text);
	//sprintf(text, "rot up(%6.3f,%6.3f,%6.3f)", rot_up.GetX(), rot_up.GetY(), rot_up.GetZ()  );
	//mnmDebugInfo::SetDebugInfo(21, text);

	//return rot;

	return maRotation(-m_Pitch.GetValue() * maConstants::c_fAngleToRad, 
					  m_Yaw.GetValue() * maConstants::c_fAngleToRad, 
					  0);	// don't do tilt here
					  //m_Data.m_Tilt.GetValue() * maConstants::c_fAngleToRad);

	//maVector3d view_dir = m_Data.m_Target.GetValue() - m_Data.m_Position.GetValue();
	//maRotation rot;
	//rot.SetValue(maVector3d(0,0,1), view_dir);
	//return rot;
}
void cmraCameraObject::UpdateOrientation(const maRotation& i_Orientation, 
										bool i_bNewOperation)
{
	// We don't store the camera view as a rotation. The rotation
	//	was derived from the pitch and yaw angles which are derived
	//	from vector from the eye position to the target point.
	// So, when the rotation compass changes, all we are doing is
	//	moving the target position.
	//
	maVector3d view_dir = m_Data.m_Target.GetValue() - m_Data.m_Position.GetValue();
	float view_length = view_dir.Length();

	maVector3d new_view(0,0,view_length);
	i_Orientation.RotateVector( new_view );

	if (m_ManipMode.GetValue() == e_Target)
	{
		m_Data.m_Position.SetValue( m_Data.m_Target.GetValue() - new_view, 
			i_bNewOperation ? prtyProperty::eNewUndo : prtyProperty::eContinueUndo);
	}
	else
	{
		m_Data.m_Target.SetValue( m_Data.m_Position.GetValue() + new_view, 
			i_bNewOperation ? prtyProperty::eNewUndo : prtyProperty::eContinueUndo);
	}
}

//----------------------------------------------------------------------------
//	Scale
//----------------------------------------------------------------------------
maPoint3d cmraCameraObject::GetScale() const
{
	return l_One;

}
void cmraCameraObject::UpdateScale(const maPoint3d& i_Scale, 
										bool i_bNewOperation)
{
	// do nothing
}



//----------------------------------------------------------------------------
//	RayPick returns true if the given ray intersects the cmraCameraObject.
//	If it does, the t value is also returned in o_T.
//----------------------------------------------------------------------------
bool cmraCameraObject::RayPick(	const maPoint3d& i_RayStart,
								const maPoint3d& i_RayEnd,
								float& o_T)
{
	if (!m_pObject || !m_pObject->GetRenderable())
		return false;

	float pick_radius = api3dScale::Scale(l_PickRadius);

	// Test if ray start is within our eye's pick sphere. If so, then return
	//	false. This allows us to pick other objects while looking through
	//	this camera.
	float dist_sq = (i_RayStart - m_Data.m_Position.GetValue()).LengthSqr();
	if (dist_sq < pick_radius*pick_radius)
	{
		return false;
	}

	// Test pick ray against eye and against target
	float eye_t  = -1.0f;
	bool bPickEye = geoRayIntersection::IntersectLineSphere(	
								i_RayStart,
								i_RayEnd - i_RayStart,
								m_Data.m_Position.GetValue(),
								pick_radius,
								eye_t);
	float tgt_t  = -1.0f;
	bool bPickTarget = geoRayIntersection::IntersectLineSphere(	
								i_RayStart,
								i_RayEnd - i_RayStart,
								m_Data.m_Target.GetValue(),
								pick_radius,
								tgt_t);

	// If both were picked, choose closest
	if (bPickEye && bPickTarget)
	{
		if (tgt_t < eye_t)
			bPickEye = false;
		else
			bPickTarget = false;
	}

	// Set the manip mode based on where the click was.
	// Note: no way to choose "together" this way, 
	// maybe we should use midpoint of ray as another pick point?
	if (bPickEye)
	{
		this->m_ManipMode.SetValue(e_Eye);
		o_T = eye_t;
		return true;
	}
	else if (bPickTarget)
	{
		this->m_ManipMode.SetValue(e_Target);
		o_T = tgt_t;
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
bool cmraCameraObject::MatchPickCode(envType::UInt32 i_PickCode)
{
	if (!m_pObject || !m_pObject->GetRenderable())
		return false;

	if (m_pObject->PositionContainsPickCode(i_PickCode))
	{
		this->m_ManipMode.SetValue(e_Eye);
		return true;
	}
	else if (m_pObject->TargetContainsPickCode(i_PickCode))
	{
		this->m_ManipMode.SetValue(e_Target);
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------
//	Renderable sets whether the cmraCameraObject can be selected.
//----------------------------------------------------------------------------
void cmraCameraObject::SetRenderable(bool i_bRenderable)
{
	m_bRenderable = i_bRenderable;

	// object is visible only if gui and layer are all
	// are set visible == true;
	bool bRenderable = m_Data.m_bEditorVisible.GetValue() 
		&& m_bRenderable 
		&& m_bLayerVisible;
	if (m_pObject) 
		m_pObject->SetRenderable(bRenderable);
	if (m_pTargetObject) 
		m_pTargetObject->SetRenderable(bRenderable);
}

//--------------------------------------------------------------------
//  Changes visible state of light based on GUI
//--------------------------------------------------------------------
void  cmraCameraObject::SetEditorVisible(bool i_bVisible)
{
	m_Data.m_bEditorVisible = i_bVisible;

	// icon is visible only if channel and gui and layer are all
	// are set visible == true;
	bool bRenderable = m_Data.m_bEditorVisible.GetValue() 
		&& m_bRenderable
		&& m_bLayerVisible;
	if (m_pObject) 
		m_pObject->SetRenderable( bRenderable );
	if (m_pTargetObject) 
		m_pTargetObject->SetRenderable(bRenderable);
}
bool cmraCameraObject::GetEditorVisible() const
{
	return m_Data.m_bEditorVisible.GetValue();
}

//--------------------------------------------------------------------
//	LayerVisible represents if objects in the layer are visible
//--------------------------------------------------------------------
void cmraCameraObject::SetLayerVisible(bool i_bVisible)
{
	m_bLayerVisible = i_bVisible;

	// object is visible only if gui and layer are all
	// are set visible == true;
	bool bRenderable = m_Data.m_bEditorVisible.GetValue() 
		&& m_bRenderable 
		&& m_bLayerVisible;
	if (m_pObject) 
		m_pObject->SetRenderable(bRenderable);
	if (m_pTargetObject) 
		m_pTargetObject->SetRenderable(bRenderable);
}

//--------------------------------------------------------------------
//	GPUPickable sets whether the icons should be rendered
//	in pick renders when doing GPU picking
//--------------------------------------------------------------------
void cmraCameraObject::SetGPUPickable(bool i_Pickable)
{
	if (m_pObject)
		m_pObject->SetPickable(i_Pickable);
}

//----------------------------------------------------------------------------
// Flags for how object can be modified
//----------------------------------------------------------------------------
mnmObject::RotateFlags cmraCameraObject::GetRotateFlags()
{
	//return mnmObject::e_RotateNone;
	return mnmObject::e_RotateXY;
	//return mnmObject::e_RotateAll;
}
mnmObject::ScaleFlags cmraCameraObject::GetScaleFlags()
{
	return mnmObject::e_ScaleNone;
}
mnmObject::TranslateFlags cmraCameraObject::GetTranslateFlags()
{
	return mnmObject::e_TranslateAll;
}


//----------------------------------------------------------------------------
// Get parent object of the spline in order for selection to trace from
//	this spline back to the controlled object.
//----------------------------------------------------------------------------
//virtual 
pick3dPickObject* cmraCameraObject::GetParentObject() const
{
	return m_pParent;
}
//virtual 
void cmraCameraObject::SetParentObject(pick3dPickObject* i_pParent)
{
	m_pParent = i_pParent;
}

//--------------------------------------------------------------------
// Accessor to camera
//--------------------------------------------------------------------
const camCamera&	cmraCameraObject::GetCamera() const
{
	return (*m_Camera);
}
camCamera&	cmraCameraObject::Camera()
{
	return (*m_Camera);
}
shared_ptr<camCamera>	cmraCameraObject::GetCameraPtr()
{
	return m_Camera;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
api3dObject* cmraCameraObject::create_line()
{
	maPoint3d	line_list[2];

	line_list[0].Set( 0.0f, 0.0f, 0.0f );
	line_list[1].Set( 0.0f, 0.0f, 1.0f );

	return api3dShape::CreateLineList( maFloatRGBA(0.0f, 1.0f, 1.0f,1), &line_list[0], 2 );
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void cmraCameraObject::NameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// The property's check will prevent an infinite
	// loop here.
	this->SetName( m_Data.m_Name.GetValue() );

	// Notify that we have a new name
	int index = camsCameraMgr::GetIndexForCamera(m_Camera.get());
	if (index >= 0)
		camsCameraMgr::SetCameraName(index, this->GetName() );

	if (i_bDirty)
	{
		cmraDocumentChunk::ActiveDataChanged();

		cmraDialogUtil::UpdateListDialog();
	}
}
void cmraCameraObject::DescriptionChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// Notify that we have a new description
	int index = camsCameraMgr::GetIndexForCamera(m_Camera.get());
	if (index >= 0)
		camsCameraMgr::SetCameraDescription(index, m_Data.m_Description.GetValue());

	if (i_bDirty)
	{
		cmraDialogUtil::UpdateListDialog();

		cmraDocumentChunk::ActiveDataChanged();
	}
}
void cmraCameraObject::TransformChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// This callback is triggered from position, target or tilt changes
	// This causes a change to the camera's LookAt

	// Rotate up vector through tilt angle
	maPoint3d pos	= m_Data.m_Position.GetValue();
	maPoint3d target	= m_Data.m_Target.GetValue();
	maRotation rot(target - pos, maConstants::c_fAngleToRad * m_Data.m_Tilt.GetValue());
	maVector3d up(0,1,0);
	rot.RotateVector(up);

	// set scripted camera directly
	m_Camera->LookAt(pos, target, up);

	// Update camera icon
	this->UpdateIcon();

	// Set the value of the pitch and yaw properties which are derived
	// from position and target
	maVector3d view_dir = target - pos;
	float pitch = 0, yaw = 0, distance = 0;
	convert_pitch_yaw(view_dir, pitch, yaw, distance);
	m_Pitch.SetValue(maConstants::c_fRadToAngle * pitch);
	m_Yaw.SetValue(maConstants::c_fRadToAngle * yaw);

	//	we need to update the world box if the position has changed
	//
	this->update_world_box();

	if (i_bDirty)
	{
		cmraDocumentChunk::ActiveDataChanged();
	}
}
void cmraCameraObject::FOVChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// Update camera value
	m_Camera->SetFOV(m_Data.m_FOV.GetValue());

	// Field of view is reflected in the camera's 3d icon, 
	// so update the icon. 
	this->UpdateIcon();

	if (i_bDirty)
	{
		cmraDocumentChunk::ActiveDataChanged();
	}
}
void cmraCameraObject::OrthoWidthChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// Update camera value
	m_Camera->SetOrthoWidth(m_Data.m_OrthoWidth.GetValue());

	// OrthoWidth should be reflected in the camera's 3d icon in the future
//	this->UpdateIcon();

	if (i_bDirty)
	{
		cmraDocumentChunk::ActiveDataChanged();
	}
}

void cmraCameraObject::DOFChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	if (m_Data.m_bEnableDOF.GetValue())
	{
		m_Camera->SetDOFParams(	m_Data.m_MaxFarBlur.GetValue(), 
								m_Data.m_NearBlurDistance.GetValue(), 
								m_Data.m_NearFocalDistance.GetValue(), 
								m_Data.m_FarFocalDistance.GetValue(), 
								m_Data.m_FarBlurDistance.GetValue());
	}
	else
	{
		m_Camera->SetDOFParams(-1, -1, -1, -1, -1);
	}

	this->UpdateIcon();

	if (i_bDirty)
		cmraDocumentChunk::ActiveDataChanged();
}

void cmraCameraObject::CameraChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// This callback is triggered when depth of field, and
	// other camera attributes besides position, target, tilt or FOV are changed.
	//
	// Update camera values here
	m_Camera->SetClip(m_Data.m_Near.GetValue(), m_Data.m_Far.GetValue());

	camHDRData hdrData;
	m_Camera->GetHDRParams(hdrData);
	// set only what is needed (cam will retain remaining settings)
	hdrData.m_MiddleGray = m_Data.m_HDRMiddleGray.GetValue();
	hdrData.m_BloomScale = m_Data.m_HDRBloomScale.GetValue();
	hdrData.m_StarType = m_Data.m_HDRStarType.GetValue();
	hdrData.m_StarScale = m_Data.m_HDRStarScale.GetValue();
	hdrData.m_BrightPassThresh = m_Data.m_HDRBrightPassThresh.GetValue();
	hdrData.m_BrightPassOffset = m_Data.m_HDRBrightPassOffset.GetValue();
	hdrData.m_WhiteCutoff = m_Data.m_HDRWhiteCutoff.GetValue();
	hdrData.m_SceneLuminance = m_Data.m_HDRSceneLuminance.GetValue();
	// pass settings back into cam
	m_Camera->SetHDRParams(hdrData);


	if (i_bDirty)
		cmraDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// AspectRatio is usually controlled by the size of the window,
// but sometimes it is scripted by the user specifically.
//--------------------------------------------------------------------
//void cmraCameraObject::AspectRatioChanged(prtyProperty *i_pProperty, bool i_bDirty)
//{
//	if (!m_Data.m_bMatchAspectToWindow.GetValue())
//	{
//		m_Camera->SetAspect(m_Data.m_AspectRatio.GetValue());
//	}
//}

//--------------------------------------------------------------------
// Checkbox controls whether AspectRatio property is used
//--------------------------------------------------------------------
//void cmraCameraObject::MatchAspectChanged(prtyProperty *i_pProperty, bool i_bDirty)
//{
//	m_Camera->SetMatchAspectToWindow(m_Data.m_bMatchAspectToWindow.GetValue());
//	if (!m_Data.m_bMatchAspectToWindow.GetValue())
//	{
//		m_Camera->SetAspect(m_Data.m_AspectRatio.GetValue());
//	}
//	if (m_pAspectRatioControl)
//	{
//		m_pAspectRatioControl->SetReadOnly(m_Data.m_bMatchAspectToWindow.GetValue());
//		m_pAspectRatioControl->UpdateControl();
//	}
//}

//--------------------------------------------------------------------
// This callback is for the pitch and yaw sliders that give a new
//	interface for altering the target, but are not real properties
//	that are animatable or written to a file.
//--------------------------------------------------------------------
void cmraCameraObject::PitchYawChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// We only want to do something if the property changed because of
	//	the user interface sliders. These properties also change
	//	when position or target change, but these are derived from those
	//	values.
	if (i_bDirty)
	{
		// Set the value of the pitch and yaw properties which are derived
		// from position and target
		maVector3d view_dir = m_Data.m_Target.GetValue() - m_Data.m_Position.GetValue();
		float distance = view_dir.Length();
		float pitch = m_Pitch.GetValue() * maConstants::c_fAngleToRad;
		float yaw = m_Yaw.GetValue() * maConstants::c_fAngleToRad;
		convert_pitch_yaw(pitch, yaw, distance, view_dir);
		
		if (m_ManipMode.GetValue() == e_Target)
			m_Data.m_Position.SetValue( m_Data.m_Target.GetValue() - view_dir );
		else
			m_Data.m_Target.SetValue( m_Data.m_Position.GetValue() + view_dir );
	}
}

//--------------------------------------------------------------------
// This callback is for the manipulation mode enumeration which
//	causes the compass manipulation to center on the eye position
//	or the target.
//--------------------------------------------------------------------
void cmraCameraObject::ManipModeChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	update_world_box();
}

//--------------------------------------------------------------------
// This callback is for when the manipulator changes the camera.
// Set the new values from the camera into the position and
// target properties.
//--------------------------------------------------------------------
void cmraCameraObject::CameraChanged(const camCamera* i_pCamera)
{
	maPoint3d pos = i_pCamera->GetPosition();
	maPoint3d target = i_pCamera->GetTarget();
	m_Data.m_Position = pos;
	m_Data.m_Target = target;
}

//--------------------------------------------------------------------
// update world box based on manip mode
//--------------------------------------------------------------------
void cmraCameraObject::update_world_box()
{
	float sph_rad = api3dScale::Scale( l_SphereRadius );
	m_WorldBox = maAxisBox(-sph_rad, sph_rad,
						   -sph_rad, sph_rad,
						   -sph_rad, sph_rad);

	if (m_ManipMode.GetValue() == e_Target)
		m_WorldBox.Translate(m_Data.m_Target.GetValue());
	else
		m_WorldBox.Translate(m_Data.m_Position.GetValue());
}


