/*****************************************************************************
**	cmraCameraObject.cpp
**
**		A cmraCameraObject is a derived class for displaying a point
**	camera's position.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Object/cmraCameraObject.hpp"
#include "Systems/Cameras/Data/cmraDocumentChunk.hpp"
#include "Systems/Cameras/GUI/cmraDialogDataUtil.hpp"
#include "Systems/Cameras/Object/cmraIconObject.hpp"
#include "Systems/Cameras/Undo/cmraOperations.hpp"
#include "Systems/Cameras/Data/cmraLensTable.hpp"

#include "Graphics/G3d/g3dThreadControl.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mat/matTexture.hpp"

#include "Support/cams/camsCameraMgr.hpp"
#include "Support/mnm/mnmDebugInfo.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"
#include "Support/xfrm/xfrmTransformMgr.hpp"

#include "Core/env/envExceptionX.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/geo/geoRayIntersection.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/cam3d/cam3dUtil.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/gpx/gpxSceneObject.hpp"
#include "Tool/icn/icnIconLayer.hpp"
#include "Tool/icn/icnIconSet.hpp"

#include <boost/bind.hpp>

//============================================================================
//============================================================================
namespace
{
	const float l_IconsScale    = 0.0025f;	//global scale for all icons (should really base this off of average screen size)
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
:	m_pParent(NULL),
	m_pObject(NULL), 
	m_pTargetObject(NULL),
	m_pTargetObjectProxy(NULL),
	m_bRenderable(true),
	m_bLayerVisible(true),
	m_bLayerPickable(true),
	m_bIsValid(true),
//	m_pAspectRatioControl(NULL),
	m_Pitch("Pitch", 0.0f),
	m_Yaw("Yaw", 0.0f),
	m_ManipMode("Manip Mode", e_Eye),
	m_FilmGate("Film Gate", cmraLensTable::GetUserType())
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

	m_Camera->SetClip(m_Data.m_Near.GetValue(), m_Data.m_Far.GetValue());
	camHDRData hdrData;
	hdrData.m_MiddleGray = m_Data.m_HDRMiddleGray.GetValue();
	hdrData.m_BloomScale = m_Data.m_HDRBloomScale.GetValue();
	hdrData.m_StarType = m_Data.m_HDRStarType.GetValue();
	hdrData.m_StarScale = m_Data.m_HDRStarScale.GetValue();
	hdrData.m_BrightPassThresh = m_Data.m_HDRBrightPassThresh.GetValue();
	hdrData.m_BrightPassOffset = m_Data.m_HDRBrightPassOffset.GetValue();
	hdrData.m_WhiteCutoff = m_Data.m_HDRWhiteCutoff.GetValue();
	hdrData.m_SceneLuminance = m_Data.m_HDRSceneLuminance.GetValue();
	m_Camera->SetHDRParams(hdrData);

	camDOFData dofData;
	dofData.m_bEnableDOF = m_Data.m_bEnableDOF.GetValue();

	if (m_Data.m_bEnableDOF.GetValue()) 
	{
		camDOFData dofData;
		dofData.m_MaxFarBlur = m_Data.m_MaxFarBlur.GetValue();
		dofData.m_NearBlurDist = m_Data.m_NearBlurDistance.GetValue();
		dofData.m_NearFocalDist = m_Data.m_NearFocalDistance.GetValue();
		dofData.m_FarFocalDist = m_Data.m_FarFocalDistance.GetValue();
		dofData.m_FarBlurDist = m_Data.m_FarBlurDistance.GetValue();
		dofData.m_MaxCoC = m_Data.m_MaxCoC.GetValue();
		m_Camera->SetDOFParams(dofData);
	}
	else
	{
		// default constructor means no DOF at all 
		m_Camera->SetDOFParams(camDOFData());
	}
	// Rotate up vector through tilt angle
	maPoint3d pos	= m_Data.m_Position.GetValue();
	maPoint3d target	= m_Data.m_Target.GetValue();
	maRotation rot(target - pos, maConstants::c_fAngleToRad * m_Data.m_Tilt.GetValue());
	maVector3d up(0,1,0);
	rot.RotateVector(up);
	// set scripted camera directly
	m_Camera->LookAt(pos, target, up);
	if (i_bOrthographic)
		m_Camera->SetOrthoWidth(m_Data.m_OrthoWidth.GetValue());
	else
		m_Camera->SetFOV(m_Data.m_FOV.GetValue());

	// After all of the values have been set into the camera, 
	// create the proxy for this camera. All changes to the camera that occur
	// in property callbacks in the gui thread should go through the proxy instead.
	m_CameraProxy.reset(new gpxCamera(*m_Camera));
	if (i_bUseEditorCamera)
		cam3dMgr::SetEditorCameraProxy(m_CameraProxy); // give proxy to cam3dMgr to use with camera manips

	// Add callback for when manipulator changes our camera proxy
	m_CameraProxy->AddCameraChangedCallback(this);

	// Set up property ranges
	//m_Pitch.SetMaximum(90.0f);
	//m_Pitch.SetMinimum(-90.0f);
	//m_Yaw.SetMaximum(180.0f);
	//m_Yaw.SetMinimum(-180.0f);

	// Set up enumeration for Lens
	cmraLensTable::SetUpFilmGateEnum(m_FilmGate);

	// Renderman needs this
	m_Camera->SetEnableALP( m_Data.m_bEnableALP.GetValue() );
	m_Camera->SetFStop( m_Data.m_Fstop.GetValue() );
	m_Camera->SetFocalDistance( m_Data.m_FocalDistance.GetValue() );
	m_Camera->SetFocalLength( m_Data.m_FocalLength.GetValue() );

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

		api3dObjectSimple *pTargetObjectBase = create_line();
		m_pTargetObject = icnIconLayer::CreateIconSet(pTargetObjectBase); // clones one icon per viewer
		m_pTargetObject->SetGPUPickable(false);
		//api3dScene::AddObject(m_pTargetObject, mnmApp::GetIconsLayerIndex());

		// Create thread-safe proxy for object
		m_pTargetObjectProxy = new gpxSceneObject(*m_pTargetObject);

		// If we are not the editor camera, then register with support managers...

		// give our name to transform manager with a callback for notifying
		// when the parent transformation changes.
		xfrmTransformMgr::AddObject(this, this); 

		// icon scaling based on camera view
		icnIconScale::RegisterScaleInterest(this);
	}

	//m_PassBuffersData.m_AOBuffer = NULL;
	//m_PassBuffersData.m_AOIntensity = 1.0f;
	//m_PassBuffersData.m_AOBlendOp = 0;

	//m_PassBuffersData.m_GIBuffer = NULL;
	//m_PassBuffersData.m_GIIntensity = 1.0f;
	//m_PassBuffersData.m_GIBlendOp = 0;

	//m_PassBuffersData.m_ReflBuffer = NULL;
	//m_PassBuffersData.m_ReflIntensity = 1.0f;
	//m_PassBuffersData.m_ReflBlendOp = 0;

	//m_PassBuffersData.m_ShadowMaskBuffer = NULL;
	//m_PassBuffersData.m_ShadowMaskIntensity = 1.0f;
	//m_PassBuffersData.m_ShadowMaskBlendOp = 0;

	//m_PassBuffersData.m_BeautyBuffer = NULL;
	//m_PassBuffersData.m_BeautyIntensity = 1.0f;
	//m_PassBuffersData.m_BeautyBlendOp = 0;

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

	//--------------------- Real world camera functions ------------------------------

	pPUII  = new prtyCheckBoxUIInfo(&(m_Data.m_bEnableALP), "Physical Camera Properties", "Use Properties");
	AddProperty( pPUII );
	m_pFocalLengthControl  = new prtyRangedFloatUIInfo(&(m_Data.m_FocalLength), "Physical Camera Properties", "Focal Length");
	m_pFocalLengthControl->SetMinimum(2.5);
	m_pFocalLengthControl->SetMaximum(3500);
	m_pFocalLengthControl->SetDecimalPlaces(1);
	m_pFocalLengthControl->SetExponent(2);	// exponential sliders have more values 
	AddProperty( m_pFocalLengthControl );
	m_pFStopControl  = new prtyRangedFloatUIInfo(&(m_Data.m_Fstop), "Physical Camera Properties", "FStop");
	m_pFStopControl->SetMinimum(1.0);
	m_pFStopControl->SetMaximum(64.0);
	m_pFStopControl->SetDecimalPlaces(1);
	AddProperty( m_pFStopControl );
	m_pFocusDistanceControl  = new prtyNumericUpDownUIInfo(&(m_Data.m_FocalDistance), "Physical Camera Properties", "Focal Distance");
	m_pFocusDistanceControl->SetMinimum(1.0);
	m_pFocusDistanceControl->SetMaximum(1000.0);
	m_pFocusDistanceControl->SetIncrement(0.01f);
	m_pFocusDistanceControl->SetDecimalPlaces(3);
	//m_pFocusDistanceControl->SetRestrictFlag( true );
	AddProperty( m_pFocusDistanceControl );

	m_pCoCControl  = new prtyNumericUpDownUIInfo(&(m_Data.m_CoC), "Physical Camera Properties", "CoC");
	m_pCoCControl->SetMinimum(0.001f);
	m_pCoCControl->SetIncrement(0.001f);
	m_pCoCControl->SetMaximum(0.1f);
	m_pCoCControl->SetDecimalPlaces(3);
	m_pCoCControl->SetRestrictFlag(true);
	AddProperty( m_pCoCControl );
	



	m_pFilmGateControl = new prtyComboBoxUIInfo(&(m_FilmGate), "Physical Camera Properties", "Film Gate");
	
	AddProperty( m_pFilmGateControl );
	
	m_pHorizontalApertureControl  = new prtyFloatEditUIInfo(&(m_Data.m_HorizontalAperture), "Physical Camera Properties", "Horizontal Aperture (inches)");
	m_pHorizontalApertureControl->SetDecimalPlaces(3);
	AddProperty( m_pHorizontalApertureControl );
	//--------------------- End of Real world camera functions ------------------------------

	if (i_bOrthographic)
	{
		m_pFOVControl = new prtyNumericUpDownUIInfo(&(m_Data.m_OrthoWidth), "View", "Field of View");
		AddProperty( m_pFOVControl );
	}
	else
	{
		m_pFOVControl = new prtyNumericUpDownUIInfo(&(m_Data.m_FOV), "View", "Field of View");
		AddProperty( m_pFOVControl );
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
	
	prtyNumericUpDownUIInfo* pNumUII = NULL;
	pNumUII  = new prtyNumericUpDownUIInfo(&(m_Data.m_Near), "View", "Near Clipping plane");
	pNumUII->SetMinimum(0.01f);
	pNumUII->SetIncrement(0.1f);
	pNumUII->SetDecimalPlaces(4);
	AddProperty( pNumUII );
	pNumUII = new prtyNumericUpDownUIInfo(&(m_Data.m_Far), "View", "Far Clipping plane");
	pNumUII->SetDecimalPlaces(4);
	pNumUII->SetIncrement(1.0f);
	AddProperty( pNumUII );

	pPUII  = new prtyCheckBoxUIInfo(&(m_Data.m_bEnableDOF), "Depth of Field", "Enable Depth of Field?");
	AddProperty( pPUII );
	m_pNearBlurDistance  = new prtyNumericUpDownUIInfo(&(m_Data.m_NearBlurDistance), "Depth of Field", "Near Blur Distance");
	AddProperty( m_pNearBlurDistance );
	m_pNearFocalDistance  = new prtyNumericUpDownUIInfo(&(m_Data.m_NearFocalDistance), "Depth of Field", "Near Focal Distance");
	AddProperty( m_pNearFocalDistance );
	m_pFarFocalDistance  = new prtyNumericUpDownUIInfo(&(m_Data.m_FarFocalDistance), "Depth of Field", "Far Focal Distance");
	AddProperty( m_pFarFocalDistance );
	m_pFarBlurDistance  = new prtyNumericUpDownUIInfo(&(m_Data.m_FarBlurDistance), "Depth of Field", "Far Blur Distance");
	AddProperty( m_pFarBlurDistance );
	pPUII  = new prtyNumericUpDownUIInfo(&(m_Data.m_MaxFarBlur), "Depth of Field", "Maximum Far Blurriness");
	AddProperty( pPUII );
	m_pMaxCoC  = new prtyNumericUpDownUIInfo(&(m_Data.m_MaxCoC), "Depth of Field", "Focus Blur");
	AddProperty( m_pMaxCoC );

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

	if (!i_bOrthographic)
	{
		//AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bEnableStereo), "Stereoscopy", "Enable") );	
		pPUII = new prtyComboBoxUIInfo(&(m_Data.m_StereoType), "Stereoscopy", "Output Type");
		AddProperty( pPUII );
		pPUII = new prtyComboBoxUIInfo(&(m_Data.m_StereoProjection), "Stereoscopy", "Projection Type");
		AddProperty( pPUII );
		pNUII  = new prtyNumericUpDownUIInfo(&(m_Data.m_StereoIOD), "Stereoscopy", "Interaxial Separation");
		pNUII->SetIncrement(0.01f);
		pNUII->SetDecimalPlaces(3);
		AddProperty( pNUII );
		pNUII  = new prtyNumericUpDownUIInfo(&(m_Data.m_StereoFD), "Stereoscopy", "Zero Parallax");
		pNUII->SetIncrement(0.01f);
		pNUII->SetDecimalPlaces(3);
		AddProperty( pNUII );
		pPUII = new prtyComboBoxUIInfo(&(m_Data.m_StereoFilterColor), "Stereoscopy", "Eyeglass Color");
		AddProperty( pPUII );

		m_Data.m_StereoFD.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::StereoFDChanged));		
		m_Data.m_StereoFilterColor.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::StereoFilterColorChanged));
		m_Data.m_StereoType.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::StereoTypeChanged));
		m_Data.m_StereoProjection.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::StereoProjectionChanged));
		m_Data.m_StereoIOD.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::StereoIODChanged));
	}

	// Set up initial enabled states of lens property controls
	enable_lens_controls();

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
	m_Data.m_MaxCoC.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::DOFChanged));
	m_Data.m_Description.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::DescriptionChanged));
	m_Data.m_bEnableALP.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::EnableALPChanged));
	//m_Data.m_bUseFOV.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::FOVChanged));
	//m_Data.m_bUseDOF.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::DOFChanged));
	//m_Data.m_bUseHDRE.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::HDREChanged));
	m_Data.m_FocalLength.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::FocalLengthChanged));
	m_Data.m_HorizontalAperture.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::ApertureChanged));
	m_Data.m_Fstop.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::DOFChanged));
	m_Data.m_FocalDistance.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::DOFChanged));
	m_Data.m_CoC.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::DOFChanged));
	
	//m_Data.m_Aperture.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::LensChanged));

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
	m_FilmGate.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::FilmGateChanged));

	m_Data.m_TextureFilenameAO.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::CameraPassBuffersAOTexChanged));
	m_Data.m_IntensityAO.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::CameraPassBuffersChanged));
	m_Data.m_BlendOpAO.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::CameraPassBuffersChanged));

	m_Data.m_TextureFilenameGI.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::CameraPassBuffersGITexChanged));
	m_Data.m_IntensityGI.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::CameraPassBuffersChanged));
	m_Data.m_BlendOpGI.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::CameraPassBuffersChanged));

	m_Data.m_TextureFilenameRefl.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::CameraPassBuffersReflTexChanged));
	m_Data.m_IntensityRefl.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::CameraPassBuffersChanged));
	m_Data.m_BlendOpRefl.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::CameraPassBuffersChanged));

	m_Data.m_TextureFilenameShadowMask.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::CameraPassBuffersShadowMaskTexChanged));
	m_Data.m_IntensityShadowMask.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::CameraPassBuffersChanged));
	m_Data.m_BlendOpShadowMask.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::CameraPassBuffersChanged));

	m_Data.m_TextureFilenameBeauty.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::CameraPassBuffersBeautyTexChanged));
	m_Data.m_IntensityBeauty.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::CameraPassBuffersChanged));
	m_Data.m_BlendOpBeauty.AddCallback(new prtyCallbackWrapper<cmraCameraObject>(this, &cmraCameraObject::CameraPassBuffersChanged));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmraCameraObject::~cmraCameraObject()
{
	if (m_pObject)
	{
		// Editor camera has no icon and does not register with support interests
		icnIconScale::UnRegisterScaleInterest(this);
		xfrmTransformMgr::RemoveObject(this); 
	}

	delete m_pObject;

	delete m_pTargetObjectProxy; // delete proxy for object first
	if (m_pTargetObject)
	{
		//api3dScene::RemoveObject(m_pTargetObject, mnmApp::GetIconsLayerIndex());
		delete m_pTargetObject;
	}

	CleanupPassBuffers();

	//api3dCameraMgr::DestroyCamera(m_pCamera);
}

//--------------------------------------------------------------------
// CleanupPassBuffers()
//--------------------------------------------------------------------
void cmraCameraObject::CleanupPassBuffers()
{
	matTextureMgr::ReleaseTexture( m_PassBuffersData.m_AOBuffer );
	matTextureMgr::ReleaseTexture( m_PassBuffersData.m_GIBuffer );
	matTextureMgr::ReleaseTexture( m_PassBuffersData.m_ReflBuffer );
	matTextureMgr::ReleaseTexture( m_PassBuffersData.m_ShadowMaskBuffer );
	matTextureMgr::ReleaseTexture( m_PassBuffersData.m_BeautyBuffer );
}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string cmraCameraObject::GetDisplayName() const
{
	return GetName().GetString();
}

//--------------------------------------------------------------------
//	CreateReferenceToSelf - create a reference that refers to the
//	given object. This reference should be able to refer to 
//  future instances of this object persistent through deletion
//	and recreation through undo.
//--------------------------------------------------------------------
shared_ptr<relObjectReference> cmraCameraObject::CreateReferenceToSelf()
{
	// Editor camera property object needs to use static reference,
	// other camera objects can use a named reference
	if (m_Camera == cam3dMgr::GetEditorCameraPtr())
		return relObject::CreateReferenceToSelf(); // Static reference using this pointer
	else
		return cmmNamedPropertyObject::CreateReferenceToSelf(); // Named reference
}

//--------------------------------------------------------------------
//	GlobalScaleChanged - notification that global scale has changed.
//--------------------------------------------------------------------
//virtual 
//void cmraCameraObject::GlobalScaleChanged( float i_Scale )
//{
//	if (m_pObject)
//		m_pObject->SetUniformScale( i_Scale );
//	update_world_box();
//}

//--------------------------------------------------------------------
//	UpdateIconScale - function that sets the icons scale.
//--------------------------------------------------------------------
//virtual 
void cmraCameraObject::UpdateIconScale( int i_IconLayerIndex, const camCamera& i_Camera, float i_Scale )
{
	if( m_pObject )
	{
		m_pObject->UpdateIconScale(i_IconLayerIndex, i_Camera, i_Scale);
	}
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

	// Set the film gate enumeration to match the value set from the data
	m_FilmGate.SetValue( cmraLensTable::GetEnumerationForAperture( m_Data.m_HorizontalAperture.GetValue() ) );
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
// MaxCoC property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyMaxCoC()
{
	return m_Data.m_MaxCoC;
}
const prtyFloat&	cmraCameraObject::GetPropertyMaxCoC() const
{
	return m_Data.m_MaxCoC;
}

//--------------------------------------------------------------------
// Focal Length property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyFocalLength()
{
	return m_Data.m_FocalLength;
}
const prtyFloat&	cmraCameraObject::GetPropertyFocalLength() const
{
	return m_Data.m_FocalLength;
}

//--------------------------------------------------------------------
// Horizontal Aperture property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyHorizontalAperture()
{
	return m_Data.m_HorizontalAperture;
}
const prtyFloat&	cmraCameraObject::GetPropertyHorizontalAperture() const
{
	return m_Data.m_HorizontalAperture;
}

//--------------------------------------------------------------------
// FStop property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyFStop()
{
	return m_Data.m_Fstop;
}
const prtyFloat&	cmraCameraObject::GetPropertyFStop() const
{
	return m_Data.m_Fstop;
}

//--------------------------------------------------------------------
// Focal Distance property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyFocalDistance()
{
	return m_Data.m_FocalDistance;
}
const prtyFloat&	cmraCameraObject::GetPropertyFocalDistance() const
{
	return m_Data.m_FocalDistance;
}

//--------------------------------------------------------------------
// CoC property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyCoC()
{
	return m_Data.m_CoC;
}
const prtyFloat&	cmraCameraObject::GetPropertyCoC() const
{
	return m_Data.m_CoC;
}
//--------------------------------------------------------------------
// Aperture property access
//--------------------------------------------------------------------
		
//prtyFloat&	cmraCameraObject::PropertyMaxCoc()
//{
//	return m_Data.m_CoC;
//}
//const prtyFloat&	cmraCameraObject::GetPropertyMaxCoc() const
//{
//	return m_Data.m_CoC;
//}

		
//--------------------------------------------------------------------
// StereoFD property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyStereoFD()
{
	return m_Data.m_StereoFD;
}
const prtyFloat&	cmraCameraObject::GetPropertyStereoFD() const
{
	return m_Data.m_StereoFD;
}

//--------------------------------------------------------------------
// StereoFD property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyStereoIOD()
{
	return m_Data.m_StereoIOD;
}
const prtyFloat&	cmraCameraObject::GetPropertyStereoIOD() const
{
	return m_Data.m_StereoIOD;
}

//--------------------------------------------------------------------
// PropertyTextureFileNameAO property access
//--------------------------------------------------------------------
prtyTextureFileName&	cmraCameraObject::PropertyTextureFileNameAO()
{
	return m_Data.m_TextureFilenameAO;
}
const prtyTextureFileName&	cmraCameraObject::GetPropertyTextureFileNameAO() const
{
	return m_Data.m_TextureFilenameAO;
}

//--------------------------------------------------------------------
// PropertyIntensityAO property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyIntensityAO()
{
	return m_Data.m_IntensityAO;
}
const prtyFloat&	cmraCameraObject::GetPropertyIntensityAO() const
{
	return m_Data.m_IntensityAO;
}

//--------------------------------------------------------------------
// PropertyBlendOpAO property access
//--------------------------------------------------------------------
prtyInt32&	cmraCameraObject::PropertyBlendOpAO()
{
	return m_Data.m_BlendOpAO;
}
const prtyInt32&	cmraCameraObject::GetPropertyBlendOpAO() const
{
	return m_Data.m_BlendOpAO;
}

//--------------------------------------------------------------------
// PropertyTextureFileNameGI property access
//--------------------------------------------------------------------
prtyTextureFileName&	cmraCameraObject::PropertyTextureFileNameGI()
{
	return m_Data.m_TextureFilenameGI;
}
const prtyTextureFileName&	cmraCameraObject::GetPropertyTextureFileNameGI() const
{
	return m_Data.m_TextureFilenameGI;
}

//--------------------------------------------------------------------
// PropertyIntensityGI property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyIntensityGI()
{
	return m_Data.m_IntensityGI;
}
const prtyFloat&	cmraCameraObject::GetPropertyIntensityGI() const
{
	return m_Data.m_IntensityGI;
}

//--------------------------------------------------------------------
// PropertyBlendOpGI property access
//--------------------------------------------------------------------
prtyInt32&	cmraCameraObject::PropertyBlendOpGI()
{
	return m_Data.m_BlendOpGI;
}
const prtyInt32&	cmraCameraObject::GetPropertyBlendOpGI() const
{
	return m_Data.m_BlendOpGI;
}

//--------------------------------------------------------------------
// PropertyTextureFileNameRefl property access
//--------------------------------------------------------------------
prtyTextureFileName&	cmraCameraObject::PropertyTextureFileNameRefl()
{
	return m_Data.m_TextureFilenameRefl;
}
const prtyTextureFileName&	cmraCameraObject::GetPropertyTextureFileNameRefl() const
{
	return m_Data.m_TextureFilenameRefl;
}

//--------------------------------------------------------------------
// PropertyIntensityRefl property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyIntensityRefl()
{
	return m_Data.m_IntensityRefl;
}
const prtyFloat&	cmraCameraObject::GetPropertyIntensityRefl() const
{
	return m_Data.m_IntensityRefl;
}

//--------------------------------------------------------------------
// PropertyBlendOpRefl property access
//--------------------------------------------------------------------
prtyInt32&	cmraCameraObject::PropertyBlendOpRefl()
{
	return m_Data.m_BlendOpRefl;
}
const prtyInt32&	cmraCameraObject::GetPropertyBlendOpRefl() const
{
	return m_Data.m_BlendOpRefl;
}

//--------------------------------------------------------------------
// PropertyTextureFileNameShadowMask property access
//--------------------------------------------------------------------
prtyTextureFileName&	cmraCameraObject::PropertyTextureFileNameShadowMask()
{
	return m_Data.m_TextureFilenameShadowMask;
}
const prtyTextureFileName&	cmraCameraObject::GetPropertyTextureFileNameShadowMask() const
{
	return m_Data.m_TextureFilenameShadowMask;
}

//--------------------------------------------------------------------
// PropertyIntensityShadowMask property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyIntensityShadowMask()
{
	return m_Data.m_IntensityShadowMask;
}
const prtyFloat&	cmraCameraObject::GetPropertyIntensityShadowMask() const
{
	return m_Data.m_IntensityShadowMask;
}

//--------------------------------------------------------------------
// PropertyBlendOpRefl property access
//--------------------------------------------------------------------
prtyInt32&	cmraCameraObject::PropertyBlendOpShadowMask()
{
	return m_Data.m_BlendOpShadowMask;
}
const prtyInt32&	cmraCameraObject::GetPropertyBlendOpShadowMask() const
{
	return m_Data.m_BlendOpShadowMask;
}

//--------------------------------------------------------------------
// PropertyTextureFileNameBeauty property access
//--------------------------------------------------------------------
prtyTextureFileName&	cmraCameraObject::PropertyTextureFileNameBeauty()
{
	return m_Data.m_TextureFilenameBeauty;
}
const prtyTextureFileName&	cmraCameraObject::GetPropertyTextureFileNameBeauty() const
{
	return m_Data.m_TextureFilenameBeauty;
}

//--------------------------------------------------------------------
// PropertyIntensityBeauty property access
//--------------------------------------------------------------------
prtyFloat&	cmraCameraObject::PropertyIntensityBeauty()
{
	return m_Data.m_IntensityBeauty;
}
const prtyFloat&	cmraCameraObject::GetPropertyIntensityBeauty() const
{
	return m_Data.m_IntensityBeauty;
}

//--------------------------------------------------------------------
// PropertyBlendOpBeauty property access
//--------------------------------------------------------------------
prtyInt32&	cmraCameraObject::PropertyBlendOpBeauty()
{
	return m_Data.m_BlendOpBeauty;
}
const prtyInt32&	cmraCameraObject::GetPropertyBlendOpBeauty() const
{
	return m_Data.m_BlendOpBeauty;
}


//--------------------------------------------------------------------
//	UpdateIcon() - update the graphical icon using the current
//	cmraCameraObject settings.
//--------------------------------------------------------------------
void cmraCameraObject::UpdateIcon()
{
	maPoint3d pos, target;
	get_world_positions(pos, target);
	if (m_pObject)
	{
		if (m_Data.m_bEnableDOF.GetValue())
		{
			m_pObject->Update( pos, target,
				m_Data.m_Tilt.GetValue(),
				m_Camera->GetFOV(),
				m_Camera->GetAspect(),
				m_Data.m_NearBlurDistance.GetValue(),
				m_Data.m_NearFocalDistance.GetValue(),
				m_Data.m_FarFocalDistance.GetValue(),
				m_Data.m_FarBlurDistance.GetValue()
				);
		}

		else
		{
			m_pObject->Update( pos, target,
				m_Data.m_Tilt.GetValue(),
				m_Camera->GetFOV(),
				m_Camera->GetAspect(), 1,1,1,1
				);
		}
	}

	if (m_pTargetObjectProxy)
	{
		maVector3d dir( target - pos );
		
		//	set the position of the 3-D line
		//
		m_pTargetObjectProxy->SetPosition( pos );

		float len = dir.Length();
		if (len > 0)
			m_pTargetObjectProxy->SetScale( maVector3d(1,1,len) );

		//	set the orientation of the 3-D line
		//
		maRotation dir_rot;
		dir_rot.SetValue(maVector3d(0,0,1), dir);
		m_pTargetObjectProxy->SetOrientation( dir_rot );
	}
}

//----------------------------------------------------------------------------
//	GetWorldBox returns a box which completely encloses the cmraCameraObject.
//----------------------------------------------------------------------------
maAxisBox cmraCameraObject::GetWorldBox(int i_IconLayerIndex) const
{
	float sph_rad = l_SphereRadius;

	if (m_pObject)
	{
		if (m_ManipMode.GetValue() == e_Target)
			sph_rad = m_pObject->GetPickTargetScale(i_IconLayerIndex);
		else
			sph_rad = m_pObject->GetCameraObjectScale(i_IconLayerIndex); 
	}

	maAxisBox icon_box(-sph_rad, sph_rad,
						   -sph_rad, sph_rad,
						   -sph_rad, sph_rad);

	maPoint3d pos = GetWorldPivot();	// world pivot is either target or position in world space
	icon_box.Translate(pos);
	return icon_box;
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
	maPoint3d pivot;
	if (m_ManipMode.GetValue() == e_Target)
		pivot = m_Data.m_Target.GetValue();
	else
		pivot = m_Data.m_Position.GetValue();

	// transform to world space
	maMatrix4x4 parent_matrix;
	this->GetParentMatrix(parent_matrix);
	parent_matrix.Transform(pivot);
	return pivot;
}

//--------------------------------------------------------------------
//  Get sum of matrices of all parents of this node. 
//--------------------------------------------------------------------
void cmraCameraObject::GetParentMatrix(maMatrix4x4 &o_Transformation) const
{
	// let transform manager compute the sum of the parent matrices
	xfrmTransformMgr::ComputeExclusiveMatrixForNode(this->GetName(), o_Transformation);
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
	const bool bSetDirty = true;
	switch (m_ManipMode.GetValue())
	{
	case e_Eye:
		if (i_bNewOperation)
			this->CreateUndoForProperty(m_Data.m_Position);
		m_Data.m_Position.SetValue(i_Position, bSetDirty);
		break;
	case e_Target:
		if (i_bNewOperation)
			this->CreateUndoForProperty(m_Data.m_Target);
		m_Data.m_Target.SetValue(i_Position, bSetDirty);
		break;
	case e_Together:
	{
		if (i_bNewOperation)
		{
			undoUndoMgr::BeginMultipleOperationBlock();
			this->CreateUndoForProperty(m_Data.m_Position);
			this->CreateUndoForProperty(m_Data.m_Target);
			undoUndoMgr::EndMultipleOperationBlock();
		}

		maVector3d diff = m_Data.m_Target.GetValue() - m_Data.m_Position.GetValue();
		m_Data.m_Position.SetValue(i_Position, bSetDirty);
		m_Data.m_Target.SetValue(i_Position + diff, bSetDirty);
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

	const bool bSetDirty = true;
	if (m_ManipMode.GetValue() == e_Target)
	{
		if (i_bNewOperation) this->CreateUndoForProperty(m_Data.m_Position);
		m_Data.m_Position.SetValue( m_Data.m_Target.GetValue() - new_view, bSetDirty);
	}
	else
	{
		if (i_bNewOperation) this->CreateUndoForProperty(m_Data.m_Target);
		m_Data.m_Target.SetValue( m_Data.m_Position.GetValue() + new_view, bSetDirty);
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
	if (m_pTargetObjectProxy) 
		m_pTargetObjectProxy->SetRenderable(bRenderable);
}

//--------------------------------------------------------------------
// Set object active state in the render layer
//--------------------------------------------------------------------
void cmraCameraObject::SetActiveRenderLayer(bool i_bActive)
{
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
	if (m_pTargetObjectProxy) 
		m_pTargetObjectProxy->SetRenderable(bRenderable);
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
	if (m_pTargetObjectProxy) 
		m_pTargetObjectProxy->SetRenderable(bRenderable);
}

//--------------------------------------------------------------------
//	GPUPickable sets whether the icons should be rendered
//	in pick renders when doing GPU picking
//--------------------------------------------------------------------
void cmraCameraObject::SetGPUPickable(bool i_Pickable)
{
	m_bLayerPickable = i_Pickable;
	bool bParentPickable = xfrmTransformMgr::GetParentPickable(this->GetName());
	if (m_pObject)
		m_pObject->SetPickable(m_bLayerPickable && bParentPickable);
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
//sel3dObject* cmraCameraObject::GetParentObject() const
//{
//	return m_pParent;
//}
////virtual 
//void cmraCameraObject::SetParentObject(sel3dObject* i_pParent)
//{
//	m_pParent = i_pParent;
//}

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
shared_ptr<gpxCamera>	cmraCameraObject::GetCameraProxyPtr()
{
	return m_CameraProxy;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
api3dObjectSimple* cmraCameraObject::create_line()
{
	maPoint3d	line_list[2];

	line_list[0].Set( 0.0f, 0.0f, 0.0f );
	line_list[1].Set( 0.0f, 0.0f, 1.0f );

	return api3dShape::CreateLineList( maFloatRGBA(0.0f, 1.0f, 1.0f,1), &line_list[0], 2 );
}


//----------------------------------------------------------------------------
// Setup enabled state for contrls based on EnableALP setting
//----------------------------------------------------------------------------
void cmraCameraObject::enable_lens_controls()
{
	// Renderman needs this
	m_Camera->SetEnableALP( m_Data.m_bEnableALP.GetValue() );

	bool bUseALP = m_Data.m_bEnableALP.GetValue();

	// FOV is enabled when ALP is disabled
	m_pFOVControl->SetReadOnly(bUseALP);
	m_pFOVControl->UpdateControl();

	// The lens properties are enabled when ALP is enabled
	m_pFilmGateControl->SetReadOnly(!bUseALP);
	m_pFilmGateControl->UpdateControl();
	m_pFocalLengthControl->SetReadOnly(!bUseALP);
	m_pFocalLengthControl->UpdateControl();
	m_pFStopControl->SetReadOnly(!bUseALP);
	m_pFStopControl->UpdateControl();
	m_pFocusDistanceControl->SetReadOnly(!bUseALP);
	m_pFocusDistanceControl->UpdateControl();
	m_pCoCControl->SetReadOnly(!bUseALP);
	m_pCoCControl->UpdateControl();
	m_pHorizontalApertureControl->SetReadOnly(!bUseALP);
	m_pHorizontalApertureControl->UpdateControl();


	m_pNearBlurDistance->SetReadOnly(bUseALP);
	m_pNearBlurDistance->UpdateControl();
	m_pNearFocalDistance->SetReadOnly(bUseALP);
	m_pNearFocalDistance->UpdateControl();
	m_pFarBlurDistance->SetReadOnly(bUseALP);
	m_pFarBlurDistance->UpdateControl();
	m_pFarFocalDistance->SetReadOnly(bUseALP);
	m_pFarFocalDistance->UpdateControl();
	
}

//----------------------------------------------------------------------------
// These functions set the camCamera properties depending on the lens
// properties
//----------------------------------------------------------------------------
void cmraCameraObject::set_camera_fov()
{
	// Field of View will be set either from the lens properties
	// m_Data.m_FocalLength and m_Data.m_HorizontalAperture or it
	// will be set directly from m_Data.m_FOV depending on the settings
	// for m_Data.m_bEnableALP (whether to use Advanced Lens Properties)
	//
	if (m_Data.m_bEnableALP.GetValue())
	//if (m_Data.m_bEnableALP.GetValue() && m_Data.m_bUseFOV.GetValue())
	{
		const float c_InchesToMM = 25.4f;
		float horz_aper_mm = c_InchesToMM * m_Data.m_HorizontalAperture.GetValue();
		float FieldOfViewDegrees = cam3dUtil::CalculateFieldOfView( m_Data.m_FocalLength.GetValue(), horz_aper_mm );
		m_Data.m_FOV.SetValue(FieldOfViewDegrees);
		m_CameraProxy->SetFOV(FieldOfViewDegrees);
	}
	else
	{
		// Update camera value
		m_CameraProxy->SetFOV(m_Data.m_FOV.GetValue());
	}
}
bool cmraCameraObject::check_divideby_zero_error(float& f, float&s, float&n, float& c, float& H, float&H2)
{
	bool ShowError = false;
	if(f<=0) 
	{
		f = 1;
		ShowError = true;
	}

	if(n<=0) 
	{
		n = 2.5f;
		ShowError = true;
	}
	if(c<=0) 
	{
		c = .005f;
		ShowError = true;
	}
	
	if(((H + s - 2*f) == 0) || ((H-s)==0) || (( H2 + ( s-f))==0) || (( H2 - ( s-f))==0))
		ShowError = true;
	if(ShowError)
		return false;
	else 
		return true;
}
//----------------------------------------------------------------------------
// This sets the real world camera Depth of Field
//----------------------------------------------------------------------------
void cmraCameraObject::set_camera_dof()
{
	// Renderman needs this
	camDOFData dofData;
	dofData.m_bEnableDOF = m_Data.m_bEnableDOF.GetValue();
	m_Camera->SetFStop( m_Data.m_Fstop.GetValue() );
	m_Camera->SetFocalDistance( m_Data.m_FocalDistance.GetValue() );

	if (m_Data.m_bEnableALP.GetValue() && m_Data.m_bEnableDOF.GetValue())
	{
		// Get all units in terms of millimeters
		float f = m_Data.m_FocalLength.GetValue();
		float s = m_Data.m_FocalDistance.GetValue()*10.0f; // Focal distance is in cm because it is prtyDistance
		float n = m_Data.m_Fstop.GetValue();
		float c = m_Data.m_CoC.GetValue();		// Coc is  a prtyFloat since the values are too small

		float H = (f*f)/(n*c) + f;  // This is the hyperfocal distance
		float H2 = (f*f)/(n*c);
		
		if(check_divideby_zero_error(f,s,n,c,H, H2))
		{
			float Dn = s * (H - f)/(H + s - 2*f);
			float Df = s * (H - f)/(H - s);
			float Dbn = (H2 * s )/ ( H2 + ( s-f));
			float Dbf = (H2 * s)/ ( H2 - ( s-f));

			if ( Df < Dn )
			{
				Df = m_Data.m_Far.GetValue();
				Dbf = Df;
			}
			else
			{
				Df = s;
				Dn = s;
			}

			dofData.m_NearBlurDist = Dbn/10;
			dofData.m_NearFocalDist =  m_Data.m_FocalDistance.GetValue();
			dofData.m_FarFocalDist = Df/10;
			dofData.m_FarBlurDist = Dbf/10;
			dofData.m_MaxCoC = m_Data.m_MaxCoC.GetValue();
			dofData.m_MaxFarBlur = m_Data.m_MaxFarBlur.GetValue();
			m_CameraProxy->SetDOFParams(dofData);
			// Set the values on the dof category
			m_Data.m_NearBlurDistance.SetValue(dofData.m_NearBlurDist);
			m_Data.m_NearFocalDistance.SetValue(dofData.m_NearFocalDist);
			m_Data.m_FarFocalDistance.SetValue(dofData.m_FarFocalDist);
			m_Data.m_FarBlurDistance.SetValue(dofData.m_FarBlurDist);
		}
		else
			m_CameraProxy->SetDOFParams(camDOFData());
	}

	// If only the DOF is checked
	else if (m_Data.m_bEnableDOF.GetValue())
	{
		dofData.m_MaxFarBlur = m_Data.m_MaxFarBlur.GetValue();
		dofData.m_NearBlurDist = m_Data.m_NearBlurDistance.GetValue();
		dofData.m_NearFocalDist = m_Data.m_NearFocalDistance.GetValue();
		dofData.m_FarFocalDist = m_Data.m_FarFocalDistance.GetValue();
		dofData.m_FarBlurDist = m_Data.m_FarBlurDistance.GetValue();
		dofData.m_MaxCoC = m_Data.m_MaxCoC.GetValue();
		m_CameraProxy->SetDOFParams(dofData);
	}
	else
	{
		// default constructor means no DOF at all 
		m_CameraProxy->SetDOFParams(camDOFData());
	}
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
	this->transformation_changed();

	if (i_bDirty)
	{
		cmraDocumentChunk::ActiveDataChanged();
	}
}
void cmraCameraObject::FOVChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// Set FOV based on the the settings for lenses
	this->set_camera_fov();

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
	m_CameraProxy->SetOrthoWidth(m_Data.m_OrthoWidth.GetValue());

	// OrthoWidth should be reflected in the camera's 3d icon in the future
//	this->UpdateIcon();

	if (i_bDirty)
	{
		cmraDocumentChunk::ActiveDataChanged();
	}
}

void cmraCameraObject::DOFChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->set_camera_dof();
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
	m_CameraProxy->SetClip(m_Data.m_Near.GetValue(), m_Data.m_Far.GetValue());

	camHDRData hdrData;
	m_CameraProxy->GetHDRParams(hdrData);
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
	m_CameraProxy->SetHDRParams(hdrData);
	

	if (i_bDirty)
		cmraDocumentChunk::ActiveDataChanged();
}

void cmraCameraObject::CameraPassBuffersChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// AO
	m_PassBuffersData.m_AOIntensity = m_Data.m_IntensityAO.GetValue();
	m_PassBuffersData.m_AOBlendOp = m_Data.m_BlendOpAO.GetValue();

	// GI
	m_PassBuffersData.m_GIIntensity = m_Data.m_IntensityGI.GetValue();
	m_PassBuffersData.m_GIBlendOp = m_Data.m_BlendOpGI.GetValue();

	// Refl
	m_PassBuffersData.m_ReflIntensity = m_Data.m_IntensityRefl.GetValue();
	m_PassBuffersData.m_ReflBlendOp = m_Data.m_BlendOpRefl.GetValue();

	// Shadow Mask
	m_PassBuffersData.m_ShadowMaskIntensity = m_Data.m_IntensityShadowMask.GetValue();
	m_PassBuffersData.m_ShadowMaskBlendOp = m_Data.m_BlendOpShadowMask.GetValue();

	// Beauty
	m_PassBuffersData.m_BeautyIntensity = m_Data.m_IntensityBeauty.GetValue();
	m_PassBuffersData.m_BeautyBlendOp = m_Data.m_BlendOpBeauty.GetValue();

	// pass settings back into cam
	m_CameraProxy->SetPassBuffersParams(m_PassBuffersData);

	if (i_bDirty)
		cmraDocumentChunk::ActiveDataChanged();
}

matTexture* cmraCameraObject::CameraPassBuffersTexChanged(matTexture* i_Mat, const prtyTextureFileName& i_Loc)
{
	gpxRenderControl::ConfirmSingleThread();

	matTexture* out = i_Mat;

	matTextureMgr::ReleaseTexture( out );
	out = NULL;

	fsLocator texLoc = i_Loc.GetValue();
	
	if ( texLoc.GetNumNames() > 0 && fsFileUtil::FileExists(texLoc) )
	{
		try
		{
			std::auto_ptr<matTexture> tex;
			tex.reset(matTextureMgr::LoadTexture(texLoc));

			if (tex.get() != NULL)
			{
				out = tex.release();
			}
		}
		catch ( const envExceptionX& i_Ex )
		{
			std::string msg = "Problem loading texture, " + i_Ex.GetErrorMessage();
			out = NULL;
			DBG_ERROR(msg);
		}	
	}

	return out;
}

void cmraCameraObject::CameraPassBuffersAOTexChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// Update camera values here
	m_PassBuffersData.m_AOBuffer = CameraPassBuffersTexChanged(m_PassBuffersData.m_AOBuffer,m_Data.m_TextureFilenameAO);

	// pass settings back into cam
	m_CameraProxy->SetPassBuffersParams(m_PassBuffersData);

	if (i_bDirty)
		cmraDocumentChunk::ActiveDataChanged();
}

void cmraCameraObject::CameraPassBuffersGITexChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// Update camera values here
	m_PassBuffersData.m_GIBuffer = CameraPassBuffersTexChanged(m_PassBuffersData.m_GIBuffer,m_Data.m_TextureFilenameGI);

	// pass settings back into cam
	m_CameraProxy->SetPassBuffersParams(m_PassBuffersData);

	if (i_bDirty)
		cmraDocumentChunk::ActiveDataChanged();
}

void cmraCameraObject::CameraPassBuffersReflTexChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// Update camera values here
	m_PassBuffersData.m_ReflBuffer = CameraPassBuffersTexChanged(m_PassBuffersData.m_ReflBuffer,m_Data.m_TextureFilenameRefl);

	// pass settings back into cam
	m_CameraProxy->SetPassBuffersParams(m_PassBuffersData);

	if (i_bDirty)
		cmraDocumentChunk::ActiveDataChanged();
}

void cmraCameraObject::CameraPassBuffersShadowMaskTexChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// Update camera values here
	m_PassBuffersData.m_ShadowMaskBuffer = CameraPassBuffersTexChanged(m_PassBuffersData.m_ShadowMaskBuffer,m_Data.m_TextureFilenameShadowMask);

	// pass settings back into cam
	m_CameraProxy->SetPassBuffersParams(m_PassBuffersData);

	if (i_bDirty)
		cmraDocumentChunk::ActiveDataChanged();
}

void cmraCameraObject::CameraPassBuffersBeautyTexChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// Update camera values here
	m_PassBuffersData.m_BeautyBuffer = CameraPassBuffersTexChanged(m_PassBuffersData.m_BeautyBuffer,m_Data.m_TextureFilenameBeauty);

	// pass settings back into cam
	m_CameraProxy->SetPassBuffersParams(m_PassBuffersData);

	if (i_bDirty)
		cmraDocumentChunk::ActiveDataChanged();
}


//--------------------------------------------------------------------
// Callback for switching between real-world and MSP lenses.
// Enables other UI elements
//--------------------------------------------------------------------
void cmraCameraObject::EnableALPChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	this->enable_lens_controls();
	this->set_camera_fov();
	this->set_camera_dof();
}

//--------------------------------------------------------------------
// FilmGate enumeration is not a real property of the camera, it is
// a convenience for setting the horizontal aperture.
//--------------------------------------------------------------------
void cmraCameraObject::FilmGateChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// We only want to do something if the property changed because of
	//	the user interface edit box. This property also changes
	//	when horizontal aperture is changed by the user.
	if (i_bDirty)
	{
		int film_gate_type = m_FilmGate.GetValue();
		if (!cmraLensTable::IsUserType(film_gate_type))
		{
			m_Data.m_HorizontalAperture.SetValue( cmraLensTable::GetHorizontalAperture(film_gate_type) );
		}
	}
}

//--------------------------------------------------------------------
// Aperture changes FOV, but it also needs to set the FilmGate enum
// if it has been changed by hand.
//--------------------------------------------------------------------
void cmraCameraObject::ApertureChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// Set FOV based on the the settings for lenses
	this->set_camera_fov();

	this->UpdateIcon();
	
	if (i_bDirty)
	{
		// If i_bDirty is true, then the user has altered the 
		// aperture by hand, so the film gate enumeration should switch to "User".
		//bga - Although maybe we should set it like in the else case also?
		m_FilmGate.SetValue( cmraLensTable::GetUserType() );

		cmraDocumentChunk::ActiveDataChanged();
	}
	else
	{
		// Set the film gate enumeration to match the value set from the data
		m_FilmGate.SetValue( cmraLensTable::GetEnumerationForAperture( m_Data.m_HorizontalAperture.GetValue() ) );
	}
}
//--------------------------------------------------------------------
// Since Focal Length sets both the FOV and DOF it is being handled here
//--------------------------------------------------------------------
void cmraCameraObject::FocalLengthChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// Renderman needs this
	m_Camera->SetFocalLength( m_Data.m_FocalLength.GetValue() );

	this->set_camera_fov();
	this->set_camera_dof();

	this->UpdateIcon();
	
	if (i_bDirty)
		cmraDocumentChunk::ActiveDataChanged();

}

//--------------------------------------------------------------------
// Callback function for real world lenses. This has been created since
// multiple functions use the same values 
// This call is redundant now.
//--------------------------------------------------------------------
void cmraCameraObject::LensChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// Set FOV based on the the settings for lenses
	this->set_camera_fov();

	this->UpdateIcon();
	
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
//		m_CameraProxy->SetAspect(m_Data.m_AspectRatio.GetValue());
//	}
//}

//--------------------------------------------------------------------
// Checkbox controls whether AspectRatio property is used
//--------------------------------------------------------------------
//void cmraCameraObject::MatchAspectChanged(prtyProperty *i_pProperty, bool i_bDirty)
//{
//	m_CameraProxy->SetMatchAspectToWindow(m_Data.m_bMatchAspectToWindow.GetValue());
//	if (!m_Data.m_bMatchAspectToWindow.GetValue())
//	{
//		m_CameraProxy->SetAspect(m_Data.m_AspectRatio.GetValue());
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
	//update_world_box();
}

//--------------------------------------------------------------------
// This callback is for when the manipulator changes the camera.
// Set the new values from the camera into the position and
// target properties.
//--------------------------------------------------------------------
void cmraCameraObject::CameraChanged(const gpxCamera* i_pCamera)
{
	maPoint3d pos = i_pCamera->GetPosition();
	maPoint3d target = i_pCamera->GetTarget();

	// The camera manipulator that is altering this camera
	// is not correctly setting the tilt. It assumes a tilt of 0
	// all the time. So, we have to set that value using these 
	// new numbers coming in. And we need to do this all the time,
	// even when the position and target properties don't change.
	maRotation rot(target - pos, maConstants::c_fAngleToRad * m_Data.m_Tilt.GetValue());
	maVector3d up(0,1,0);
	rot.RotateVector(up);
	m_CameraProxy->LookAt(pos, target, up);

	m_Data.m_Position = pos;
	m_Data.m_Target = target;

	// Update camera icon
	this->UpdateIcon();
}


//--------------------------------------------------------------------
// Stereo FD handler
//--------------------------------------------------------------------
void cmraCameraObject::StereoFDChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	m_CameraProxy->SetStereoFD(m_Data.m_StereoFD.GetValue());
}

//--------------------------------------------------------------------
// Stereo color filter combobox handler
//--------------------------------------------------------------------
void cmraCameraObject::StereoFilterColorChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	m_CameraProxy->SetStereoFilterColor(m_Data.m_StereoFilterColor.GetValue());
}

//--------------------------------------------------------------------
// Stereo type combobox handler
//--------------------------------------------------------------------
void cmraCameraObject::StereoTypeChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	m_CameraProxy->SetStereoType(m_Data.m_StereoType.GetValue());
}

//--------------------------------------------------------------------
// Stereo projection combobox handler
//--------------------------------------------------------------------
void cmraCameraObject::StereoProjectionChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	m_CameraProxy->SetStereoProjection(m_Data.m_StereoProjection.GetValue());
}

//--------------------------------------------------------------------
// Stereo IOD Handler
//--------------------------------------------------------------------
void cmraCameraObject::StereoIODChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	m_CameraProxy->SetStereoIOD(m_Data.m_StereoIOD.GetValue());
}

//--------------------------------------------------------------------
// compute world position of light using parent transformations
//--------------------------------------------------------------------
void cmraCameraObject::get_world_positions(maPoint3d &o_Position, maPoint3d &o_Target) const
{
	// New method incorporates transformation matrix into property and lets
	// the property compute and cache both world and object space
	o_Position = m_Data.m_Position.GetWorldSpaceValue();
	o_Target = m_Data.m_Target.GetWorldSpaceValue();

	// Old method was to transform to word space here
	//o_Position = m_Data.m_Position.GetValue();
	//o_Target = m_Data.m_Target.GetValue();

	// transform to world space
	//maMatrix4x4 parent_matrix;
	//this->GetParentMatrix(parent_matrix);
	//parent_matrix.Transform(o_Position);
	//parent_matrix.Transform(o_Target);

}

//--------------------------------------------------------------------
// common code when a property related to position of light is changed
//--------------------------------------------------------------------
void cmraCameraObject::transformation_changed()
{
	// Rotate up vector through tilt angle
	maPoint3d pos, target;
	get_world_positions(pos, target);
	maRotation rot(target - pos, maConstants::c_fAngleToRad * m_Data.m_Tilt.GetValue());
	maVector3d up(0,1,0);
	rot.RotateVector(up);

	// set scripted camera directly
	if (m_CameraProxy->LookAt(pos, target, up))
	{
		// Update camera icon
		this->UpdateIcon();
	}

	// Set the value of the pitch and yaw properties which are derived
	// from position and target
	//maVector3d view_dir = target - pos;	// this is world space pitch & yaw
	maVector3d view_dir = m_Data.m_Target.GetValue() - m_Data.m_Position.GetValue(); // this is object space pitch & yaw
	float pitch = 0, yaw = 0, distance = 0;
	convert_pitch_yaw(view_dir, pitch, yaw, distance);
	m_Pitch.SetValue(maConstants::c_fRadToAngle * pitch);
	m_Yaw.SetValue(maConstants::c_fRadToAngle * yaw);
}

//--------------------------------------------------------------------
// Called from transformation manager once per frame, update camera
//	position based on parent transformation matrix.
//--------------------------------------------------------------------
void cmraCameraObject::UpdateParentTransform()
{
	// Give the properties the transformation matrix and let them compute
	// the object and world space as needed.
	maMatrix4x4 parent_matrix;
	this->GetParentMatrix(parent_matrix);
	m_Data.m_Position.SetTransformation(parent_matrix);
	m_Data.m_Target.SetTransformation(parent_matrix);

	// This function is called once per frame in order to avoid
	// multiple updates everytime any property of any parent transformation
	// node changes. Update camera and icons based on parent transformation.
	this->transformation_changed();

	
	// Also update pickable flag here, once per frame,
	// because it is inherited from the parent transforms
	bool bParentPickable = xfrmTransformMgr::GetParentPickable(this->GetName());
	if (m_pObject)
		m_pObject->SetPickable(m_bLayerPickable && bParentPickable);
}

//--------------------------------------------------------------------
// Called from transformation manager when the parenting of this 
// object changes in a way that we need to alter our values to
// saty in the same world position.
//--------------------------------------------------------------------
void cmraCameraObject::ApplyTransformation(const maMatrix4x4& i_Matrix)
{
	maPoint3d position = m_Data.m_Position.GetValue();
	maPoint3d target = m_Data.m_Target.GetValue();

	i_Matrix.Transform(position);
	i_Matrix.Transform(target);

	m_Data.m_Position.SetValue(position);
	m_Data.m_Target.SetValue(target);

}

//--------------------------------------------------------------------
// Return true if this object has a pivot point based on its icon.
// If so, return pivot point in the o_Pivot argument.
//--------------------------------------------------------------------
bool cmraCameraObject::HasIconPivotPoint(maPoint3d& o_Pivot) 
{ 
	maPoint3d position, target;
	get_world_positions(position, target);
	o_Pivot = position;
	return true; 
}


