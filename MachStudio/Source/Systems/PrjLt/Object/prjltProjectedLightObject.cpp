/*****************************************************************************
**  prjltProjectedLightObject.cpp
**
**      A prjltProjectedLightObject is a derived class for displaying a projected
**	light's position.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/PrjLt/Object/prjltProjectedLightObject.hpp"
#include "Systems/PrjLt/Data/prjltDocumentChunk.hpp"
#include "Systems/PrjLt/GUI/prjltDialogDataUtil.hpp"
#include "Systems/PrjLt/GUI/prjltTextureList.hpp"
#include "Systems/PrjLt/Object/prjltIconDirectional.hpp"
#include "Systems/PrjLt/Object/prjltIconProjected.hpp"
#include "Systems/PrjLt/Object/prjltIconSpot.hpp"
#include "Systems/PrjLt/Object/prjltRangeIcon.hpp"
#include "Systems/PrjLt/Undo/prjltOperations.hpp"

#include "Features/Prefs/PrefsMgr.hpp"
#include "Support/ltst/ltstIsolateMgr.hpp"
#include "Support/ltst/ltstLightSetMgr.hpp"
#include "Support/brsh/brshPaintBrushMgr.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mnm/mnmReferencePosChannel.hpp"
#include "Support/rmp/rmpDialogMgr.hpp"
#include "Support/rmp/rmpDialogUtil.hpp"
#include "Support/rmp/rmpObject.hpp"
#include "Support/rmp/rmpTextureMgr.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"
#include "Support/xfrm/xfrmTransformMgr.hpp"

#include "Core/fs/fsResourceTracker.hpp"
#include "Core/geo/geoRayIntersection.hpp"
#include "Core/gf/gfFileTxt.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyColorRGBAEditUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyGradientEditUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextureFileChooserUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Graphics/eff/effLightGlowData.hpp"
#include "Graphics/eff/effRampData.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#include "Graphics/g3d/g3dSceneRendererCreate.hpp"
#include "Graphics/g3d/g3dTargetRenderer.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/sc/scObject.hpp"
#include "Tool/api3d/api3dLightMgr.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Tool/api3d/api3dTargetRendererMgr.hpp"
#include "Tool/gpx/gpxCamera.hpp"
#include "Tool/gpx/gpxProjectedLight.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"

#include <assert.h>
#include <sstream>

#undef ReplaceFile

//============================================================================
//============================================================================
namespace
{
	g2dPFD l_ldrPFD( 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000, 32 );

	const float l_SphereRadius = 0.4f;
	const float l_CircleRadius = 0.25f;
	const float l_BoxRadius = 4 * l_SphereRadius;
	const float l_PickRadius = 3.0f;	// pick larger than icon
	const float c_MinimumScale = 0.1f;
	const maAxisBox l_SphereBox(-l_BoxRadius, l_BoxRadius,
								-l_BoxRadius, l_BoxRadius,
								-l_BoxRadius, l_BoxRadius);
	const maVector3d l_Zero(0,0,0);
	const maVector3d l_One(1,1,1);
	const maRotation l_NoRot;		
	
	const float c_InvLn = 2.7182818284590452353602874713527f;

	enum ManipModes
	{
		e_Light = 0,
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
//----------------------------------------------------------------------------
prjltProjectedLightObject::prjltProjectedLightObject(prjltData::LightType i_LightType)
:	m_pTexture(NULL), 
	m_DepthMapSize(0),
	m_HairShadowSize(0),
	m_HairShadowType(HAIR_SHADOW_OSM4),
	m_pShadowMap(NULL), 
	m_pDepthRenderer(NULL), 
	m_pMapRenderer(NULL),
	m_pOpacityMapRenderer(NULL),
	m_pOpacityRenderer(NULL),
	m_pLightShaft(NULL),
	m_pLightShaftMat(NULL),
	m_pShaftTexture(NULL),
	m_pRampTexture(NULL),
	m_pShaftRampTexture(NULL),
    m_bRenderable(true),
	m_bLayerVisible(true),
	m_bLayerPickable(true),
	m_ParentScale(1),
	m_Pitch("Pitch", 0.0f),
	m_Yaw("Yaw", 0.0f),
	m_ManipMode("Manip Mode", e_Light),
	m_LogAspect("Aspect (log)", 0.0f),
	m_ShowFrustrum("Show Frustum", true),
	m_ShowTexture("Show Texture", true),
	m_TexturePosition("Texture position", 0.0f),
	m_pAngleControl(NULL),
	m_pPenumbraControl(NULL)
	/*m_pReflectiveMap(NULL),
	m_pRSMRenderer(NULL),
	m_pRSMTargetRenderer(NULL),
	m_ReflectiveMapSize(0)*/
	//m_pRampTargetRenderer(NULL)
{
	// Light can be projected, spot or directional
	m_Data.m_LightType = i_LightType;

	// Certain aspects of the projected light are turned on or off depending on the
	// light type
	bool bDoTexture = (i_LightType == prjltData::e_ProjectedLight);
	bool bDoShaft = (i_LightType != prjltData::e_DirectionalLight);

	for( int i = 0; i < NUM_OPACITY_MAPS; i++ )
	{
		m_pOpacityShadowMap[i] = NULL;
	}
	m_pOpacityVolume = NULL;

	// Set up property ranges
	//m_Pitch.SetMaximum(90.0f);
	//m_Pitch.SetMinimum(-90.0f);
	//m_Yaw.SetMaximum(180.0f);
	//m_Yaw.SetMinimum(-180.0f);

	// Set up enumeration for manip mode
	m_ManipMode.SetEnumTag(e_Light,"Light");
	m_ManipMode.SetEnumTag(e_Target,"Target");
	m_ManipMode.SetEnumTag(e_Together,"Together");

	// Create g3d light
	m_pProjectedLight = api3dLightMgr::CreateProjectedLight();
	m_pProjectedLight->SetCastsShadow(true);
	// ensure current defaults are set in light.
	m_pProjectedLight->SetIntensity(m_Data.m_Color.GetValue());
	m_pProjectedLight->SetEnable(m_Data.m_Enabled.GetValue());
	m_pProjectedLight->SetPosition( m_Data.m_Position.GetValue() );
	m_pProjectedLight->SetTarget( m_Data.m_Target.GetValue() );
	m_pProjectedLight->SetRange(m_Data.m_Range.GetValue());
	m_pProjectedLight->SetIsDirectional(m_Data.m_bDirectional.GetValue());
	if (m_Data.m_bConeLighting.GetValue())
	{
		// Cone lighting has angle as iner "hot spot" lighting angle
		m_pProjectedLight->SetInnerAngle(m_Data.m_Angle.GetValue());
		float total_angle = m_Data.m_Angle.GetValue() + m_Data.m_Penumbra.GetValue();
		maFunctions::Clamp(total_angle, 0.0f, 179.9f);
		m_pProjectedLight->SetAngle(total_angle);
	}
	else
	{
		// Otherwise, inner angle is set to maximum and the full 
		// rectangular light frustrum is used.
		m_pProjectedLight->SetAngle(m_Data.m_Angle.GetValue());
		m_pProjectedLight->SetInnerAngle(180.0f);
	}
	m_pProjectedLight->SetScale(m_Data.m_Scale.GetValue());
	m_pProjectedLight->SetAspect(m_Data.m_Aspect.GetValue());
	m_pProjectedLight->SetTilt(m_Data.m_Tilt.GetValue());
	m_pProjectedLight->SetDepthBias(m_Data.m_DepthBias.GetValue() * 0.02f);
	m_pProjectedLight->SetIntensityFactor(m_Data.m_Intensity.GetValue());
	m_pProjectedLight->SetFalloff0(m_Data.m_Falloff.GetValue()[0]);
	m_pProjectedLight->SetFalloff1(m_Data.m_Falloff.GetValue()[1]);
	m_pProjectedLight->SetFalloff2(m_Data.m_Falloff.GetValue()[2]);
	m_pProjectedLight->SetDiffuseEnabled( m_Data.m_bDiffuseEnabled.GetValue() );
	m_pProjectedLight->SetSpecularEnabled( m_Data.m_bSpecularEnabled.GetValue() );
	m_pProjectedLight->SetAffectsGlow(m_Data.m_bAffectsGlow.GetValue());
	m_pProjectedLight->SetLightSize(m_Data.m_LightSize.GetValue() * 0.05f);
	m_pProjectedLight->SetPCSSAdjust(m_Data.m_PCSSAdjust.GetValue());
//	m_pProjectedLight->SetSceneScale(m_Data.m_SceneScale.GetValue());
	m_pProjectedLight->SetShadowQuality((g3dProjectedLight::ShadowQuality)m_Data.m_ShadowQuality.GetValue());
	m_pProjectedLight->SetShadowIntensity(m_Data.m_ShadowIntensity.GetValue());
	m_pProjectedLight->SetShadowColor(m_Data.m_ShadowColor.GetValue());
	m_pProjectedLight->SetHairMinBound(m_Data.m_HairMinBound.GetValue());
	m_pProjectedLight->SetHairMaxBound(m_Data.m_HairMaxBound.GetValue());
	m_pProjectedLight->SetHairShadowType((HAIR_SHADOW_TYPE)m_Data.m_HairShadowType.GetValue());

	// Directional lights are automatically resized to light the whole scene
	if (m_Data.m_LightType == prjltData::e_DirectionalLight)
		api3dScene::AddAutoResizeLight(m_pProjectedLight, &m_ShadowCamera);

	// Create proxies
	m_pLightProxy = new gpxProjectedLight(*m_pProjectedLight);
	m_ShadowCameraProxy = new gpxCamera(m_ShadowCamera);

	// simple renderer for shadow maps (put into api3d somehow?)
	m_pDepthRenderer = g3dSceneRendererCreate::CreateDepthMapRenderer();

	// simple renderer for reflective shadow maps (put into api3d somehow?)
	//m_pRSMRenderer = g3dSceneRendererCreate::CreateRSMRenderer();

	// simple renderer for Opacity shadow maps (put into api3d somehow?)
	m_pOpacityRenderer = g3dSceneRendererCreate::CreateOpacityMapRenderer( m_pProjectedLight );

	// Create 3D icon based on light type
	if (m_Data.m_LightType == prjltData::e_SpotLight)
		m_pObject = new prjltIconSpot();
	else if (m_Data.m_LightType == prjltData::e_DirectionalLight)
		m_pObject = new prjltIconDirectional();
	else
		m_pObject = new prjltIconProjected();

	//m_pObject = api3dShape::CreateSphere(maFloatRGBA(1,1,1,1), l_SphereRadius, 8, 8);
	//m_pObject->SetPosition(i_Data.m_Position);
	m_pObject->SetColor(m_Data.m_Color.GetValue());

	// falloff icon
	m_pRangeIcon = new prjltRangeIcon();

	// Create Ramp texture
	m_pRampTexture = rmpTextureMgr::CreateRampTexture(512);
	m_pShaftRampTexture = rmpTextureMgr::CreateRampTexture(512);
	
	ltstLightSetMgr::AddLight(this, m_pProjectedLight);	
	ltstIsolateMgr::AddLight(this, this);	
	// give our name to transform manager with a callback for notifying
	// when the parent transformation changes.
	xfrmTransformMgr::AddObject(this, this);

	icnIconScale::RegisterScaleInterest(this);

	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	prtyRangedFloatUIInfo* pRFUII;
	prtyComboBoxUIInfo* pCBUII;
	prtyNumericUpDownUIInfo* pNUDUII;
	pPUII = new prtyTextBoxUIInfo(&(m_Data.m_Name), "Asset", "Name of the object");
	AddProperty( pPUII );

	//prtyFileChooserUIInfo* pFCI;
	prtyTextureFileChooserUIInfo *pTFCI;
	if (bDoTexture)
	{
		pTFCI = new prtyTextureFileChooserUIInfo(&(m_Data.m_TextureFilename), "Asset", "File Name of the texture");
		//fsLocator dir;
		//dir.Push( gfPaths::GetPath( mnmPaths::e_DataStock ) );
		//dir.Push("Effects");
		//dir.Push("General");
		//dir.Push("Textures");
		//pFCI->SetInitialDirectory(dir);
		pTFCI->AddItem(pTFCI->e_Ramp);
		//pTFCI->AddItem(pTFCI->e_Paint);

		pTFCI->SetDirectoryCategory("Gels");
		AddProperty( pTFCI );
	}

	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_Enabled), "Asset", "Enabled object") );

	if (bDoShaft)
	{
		// Falloff group
		cmmFalloffAspect::RegisterProperties( this, &(m_Data.m_Falloff));

		// Checkboxes for control of icon display
		AddProperty( new prtyCheckBoxUIInfo(&(m_ShowFrustrum), "Icon Display", "Display frustum of light") );
	}
	if (bDoTexture)
	{
		AddProperty( new prtyCheckBoxUIInfo(&(m_ShowTexture), "Icon Display", "Show texture in icon") );
		pRFUII  = new prtyRangedFloatUIInfo(&(m_TexturePosition), "Icon Display", "Position texture along light frustum");
		pRFUII->SetMinimum(0.0f);
		pRFUII->SetMaximum(1.0f);
		pRFUII->SetNumTicks(1000);
		AddProperty( pRFUII );
	}

	pPUII  = new prtyColorRGBAEditUIInfo(&(m_Data.m_Color), "Light", "Color of the object");
	AddProperty( pPUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Intensity), "Light", "Brightness multiplier (>1 for HDR only)");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(10.0f);
	pRFUII->SetNumTicks(1000);
	AddProperty( pRFUII );

	if (m_Data.m_LightType == prjltData::e_SpotLight)
	{
	}
	else if (m_Data.m_LightType == prjltData::e_ProjectedLight)
	{
		AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bDirectional), "Light", "Enable parallel lighting within ortographic frustrum") );
		AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bConeLighting), "Light", "Enable cone shaped spot lighting using angle and penumbra properties") );
	}
	if (m_Data.m_LightType != prjltData::e_DirectionalLight)
	{
		m_pAngleControl  = new prtyRangedFloatUIInfo(&(m_Data.m_Angle), "Light", "Angle of the light");
		m_pAngleControl->SetMinimum(0.0f); //1.0f);
		m_pAngleControl->SetMaximum(179.0f);
		m_pAngleControl->SetDecimalPlaces(2);
		m_pAngleControl->SetNumTicks(100);
		AddProperty( m_pAngleControl );

		m_pPenumbraControl  = new prtyRangedFloatUIInfo(&(m_Data.m_Penumbra), "Light", "Falloff Angle of the spot light");
		m_pPenumbraControl->SetMinimum(0.0f); 
		m_pPenumbraControl->SetMaximum(180.0f);
		m_pPenumbraControl->SetDecimalPlaces(2);
		m_pPenumbraControl->SetNumTicks(100);
		AddProperty( m_pPenumbraControl );	
		
		// Projected lights have modes that enable and disable certain angle properties
		if (m_Data.m_LightType == prjltData::e_ProjectedLight)
		{
			m_pAngleControl->SetReadOnly(m_Data.m_bDirectional.GetValue() && !m_Data.m_bConeLighting.GetValue());
			m_pPenumbraControl->SetReadOnly(!m_Data.m_bConeLighting.GetValue());
		}

		pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Scale), "Light", "Scale of the light");
		pRFUII->SetMinimum(c_MinimumScale); //bga - this can't be zero or else you get uninvertible matrix bugs
		pRFUII->SetMaximum(1000.0f);
		pRFUII->SetDecimalPlaces(1);
		pRFUII->SetNumTicks(100);
		pRFUII->SetExponent(3);
		AddProperty( pRFUII );
		pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Range), "Light", "Range of the object");
		pRFUII->SetMinimum(0.0f); //0.1f);
		pRFUII->SetMaximum(3000.0f);
		pRFUII->SetDecimalPlaces(2);
		pRFUII->SetNumTicks(100);
		pRFUII->SetExponent(3);
		AddProperty( pRFUII );
	}
	if (m_Data.m_LightType == prjltData::e_ProjectedLight)
	{
		//pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Aspect), "Light", "Aspect of the light´s view region");
		pRFUII  = new prtyRangedFloatUIInfo(&(m_LogAspect), "Light", "Aspect (log) of the light´s view region");
		pRFUII->SetMinimum(-4.0f);
		pRFUII->SetMaximum(4.0f);
		pRFUII->SetDecimalPlaces(2);
		pRFUII->SetNumTicks(100);
		AddProperty( pRFUII );
	}

	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bDiffuseEnabled), "Light", "Enable Diffuse") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bSpecularEnabled), "Light", "Enable Specular") );
//	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bAffectsFur), "Light", "Does the light affect fur") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bAffectsGlow), "Light", "Does the light affect glow") );

	pCBUII  = new prtyComboBoxUIInfo(&(m_Data.m_DepthMapSize), "Shadow Quality", "Depth Map Size");
	pCBUII->AddItem(std::string("256"), 0);
	pCBUII->AddItem(std::string("512"), 1);
	pCBUII->AddItem(std::string("1024"), 2);
	pCBUII->AddItem(std::string("2048"), 3);
	pCBUII->AddItem(std::string("4096"), 4);
	pCBUII->AddItem(std::string("8192"), 5);
	//pCBUII->SetReadOnly(true);
	AddProperty( pCBUII );
	// Editor visible is set from other part of GUI
	//AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bEditorVisible), "Editor", "Is the object visible in the editor") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_ShadowSource), "Shadow Quality", "Is this a Shadow Source") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_GISource), "GI Quality", "Is this a gi Source") );

	//pCBUII  = new prtyComboBoxUIInfo(&(m_Data.m_ReflectiveMapSize), "GI Quality", "Depth Map Size");
	//pCBUII->AddItem(std::string("256"), 0);
	//pCBUII->AddItem(std::string("512"), 1);
	//pCBUII->AddItem(std::string("1024"), 2);
	//pCBUII->AddItem(std::string("2048"), 3);
	////pCBUII->SetReadOnly(true);
	//AddProperty( pCBUII );
	
	pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_ShadowQuality), "Shadow Quality", "Shadow quality level");
	AddProperty( pCBUII );


	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_DepthBias), "Shadow Quality", "Depth Bias for shadow map");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	pRFUII->SetDecimalPlaces(3);
	pRFUII->SetNumTicks(100);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_LightSize), "Shadow Quality", "Light Size of the object");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(5.0f);
	pRFUII->SetDecimalPlaces(3);
	pRFUII->SetNumTicks(100);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_PCSSAdjust), "Shadow Quality", "Distance Softness Scale");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	pRFUII->SetDecimalPlaces(3);
	pRFUII->SetNumTicks(100);
	AddProperty( pRFUII );
//	pPUII  = new prtyFloatEditUIInfo(&(m_Data.m_SceneScale), "Shadow Quality", "Scene Scale of the object");
//	AddProperty( pPUII );
	pPUII  = new prtyColorRGBAEditUIInfo(&(m_Data.m_ShadowColor), "Shadow Quality", "Color of the shadows");
	AddProperty( pPUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_ShadowIntensity), "Shadow Quality", "How dark is the shadow");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	AddProperty( pRFUII );

#ifdef HAIR_SUPPORTED
	//--------------Hair properties--------------------------------------------

	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_HairShadowEnable), "Hair", "Enable shadow maps for hair") );

	pCBUII  = new prtyComboBoxUIInfo(&(m_Data.m_HairShadowSize), "Hair", "Hair Shadow Map Resolution");
	pCBUII->AddItem(std::string("64"), 0);
	pCBUII->AddItem(std::string("128"), 1);
	pCBUII->AddItem(std::string("256"), 2);
	pCBUII->AddItem(std::string("512"), 3);
	pCBUII->AddItem(std::string("1024"), 4);
	pCBUII->AddItem(std::string("2048"), 5);
	pCBUII->AddItem(std::string("4096"), 6);
	pCBUII->AddItem(std::string("8192"), 7);
	AddProperty( pCBUII );

	AddProperty( new prtyComboBoxUIInfo(&(m_Data.m_HairShadowType), "Hair", "Shadow map type") );
/*
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_HairShadowBias), "Hair", "Depth Bias for shadow map");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	pRFUII->SetDecimalPlaces(3);
	pRFUII->SetNumTicks(100);
	AddProperty( pRFUII );

	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_HairShadowDensity), "Hair", "Shadow Density");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(6.0f);
	pRFUII->SetDecimalPlaces(3);
	pRFUII->SetNumTicks(100);
	AddProperty( pRFUII );

	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_HairMinBound), "Hair", "Minimum distance of Opacity Shadow Map");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(3000.0f);
	pRFUII->SetDecimalPlaces(2);
	pRFUII->SetNumTicks(100);
	pRFUII->SetExponent(3);
	AddProperty( pRFUII );

	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_HairMaxBound), "Hair", "Maximum distance of Opacity Shadow Map");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(3000.0f);
	pRFUII->SetDecimalPlaces(2);
	pRFUII->SetNumTicks(100);
	pRFUII->SetExponent(3);
	AddProperty( pRFUII );
*/
#endif//HAIR_SUPPORTED

	if (m_Data.m_LightType != prjltData::e_DirectionalLight)
	{
		// Transform category
		pPUII = new prtyComboBoxUIInfo(&(m_ManipMode), "Transform", "Manipulations center on light or target");
		AddProperty( pPUII );
	}

	// Pitch and Yaw work for all light types
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Pitch), "Transform", "Pitch angle of light direction");
	pRFUII->SetMinimum(-89.9f);
	pRFUII->SetMaximum(89.9f);
	pRFUII->SetDecimalPlaces(1);
	pRFUII->SetNumTicks(180);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Yaw), "Transform", "Yaw angle of light direction");
	pRFUII->SetMinimum(-180.0f);
	pRFUII->SetMaximum(180.0f);
	pRFUII->SetDecimalPlaces(1);
	pRFUII->SetNumTicks(90);
	AddProperty( pRFUII );
	
	if (m_Data.m_LightType == prjltData::e_ProjectedLight)
	{
		pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Tilt), "Transform", "Tilt angle around light direction");
		pRFUII->SetMinimum(-180.0f);
		pRFUII->SetMaximum(180.0f);
		pRFUII->SetDecimalPlaces(1);
		pRFUII->SetNumTicks(90);
		AddProperty( pRFUII );
	}
	pPUII = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_Position), "Transform", "Position of the object");
	AddProperty( pPUII );

	if (m_Data.m_LightType == prjltData::e_DirectionalLight)
	{
		// Orientation field for directional lights
		prtyVector3dEditUpDownUIInfo *pPVEUDUII  = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_Orientation), "Transform", "Orientation of the directional light");
		pPVEUDUII->SetIncrement(0.5f, 0.5f, 0.5f);
		pPVEUDUII->SetDecimalPlaces(1);
		AddProperty( pPVEUDUII );
	}
	
	if (m_Data.m_LightType != prjltData::e_DirectionalLight)
	{
		pPUII  = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_Target), "Transform", "Target of the object");
		AddProperty( pPUII );
	}

	if (bDoShaft)
	{
		// Light Shaft properties
		AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bShaftVisible), "Light Shaft", "show shaft visibly") );
		pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_ShaftAlpha), "Light Shaft", "Shaft Alpha of the object");
		pRFUII->SetMinimum(0.0f);
		pRFUII->SetMaximum(1.0f);
		//pRFUII->SetNumTicks(20);
		AddProperty( pRFUII );
		pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_ShaftDensity), "Light Shaft", "Shaft Density of the object");
		pRFUII->SetMinimum(0.0f);
		pRFUII->SetMaximum(1.0f);
		//pRFUII->SetNumTicks(20);
		AddProperty( pRFUII );
		pPUII  = new prtyFloatEditUIInfo(&(m_Data.m_ShaftDistFalloffStart), "Light Shaft", "Shaft Distance Falloff Start of the object");
		AddProperty( pPUII );
		pPUII  = new prtyFloatEditUIInfo(&(m_Data.m_ShaftDistFalloffEnd), "Light Shaft", "Shaft Distance Falloff End of the object");
		AddProperty( pPUII );
		pTFCI = new prtyTextureFileChooserUIInfo(&(m_Data.m_ShaftTextureFilename), "Light Shaft", "File Name of the texture");
		//fsLocator shaftDir;
		//shaftDir.Push( gfPaths::GetPath( mnmPaths::e_DataStock ) );
		//shaftDir.Push("Effects");
		//shaftDir.Push("General");
		//shaftDir.Push("Textures");
		//pFCI->SetInitialDirectory(shaftDir);
		pTFCI->AddItem(pTFCI->e_Ramp);
		pTFCI->SetDirectoryCategory("Gels");
		AddProperty( pTFCI );
	}

	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bMRayAreaLight), "mental ray", "Enable Area Light") );
	AddProperty( new prtyComboBoxUIInfo(&(m_Data.m_MRayAreaLightType), "mental ray", "Area Light Type") );
	pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MRayAreaLightSampling), "mental ray", "Number of Samples");
	pNUDUII->SetDecimalPlaces(0);
	pNUDUII->SetMinimum(1);
	pNUDUII->SetMaximum(256);
	pNUDUII->SetRestrictFlag(true);
	AddProperty( pNUDUII );
	//AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bMRayAreaLightVisible), "Mental Ray", "Visible") );

	// Register callbacks to update particle generator and icons
	// when properties change
	m_Data.m_Name.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::NameChanged));
	m_Data.m_TextureFilename.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::TextureChanged));
	m_Data.m_DepthMapSize.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::DepthMapChanged));
	m_Data.m_Color.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::ColorChanged));
	m_Data.m_Intensity.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::ColorChanged));
	m_Data.m_Position.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::TransformChanged));
	m_Data.m_Orientation.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::TransformChanged));
	m_Data.m_Scale.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::TransformChanged));
	m_Data.m_Target.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::TransformChanged));
	m_Data.m_bDirectional.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::AngleMeaningChanged));
	m_Data.m_bConeLighting.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::AngleMeaningChanged));
	m_Data.m_Angle.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::TransformChanged));
	m_Data.m_Penumbra.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::TransformChanged));
	m_Data.m_Aspect.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::TransformChanged));
	m_Data.m_Tilt.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::TransformChanged));

	m_Data.m_Enabled.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::EnabledChanged));
	m_Data.m_ShadowSource.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::ShadowSourceChanged));
	m_Data.m_GISource.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::GISourceChanged));
	//m_Data.m_ReflectiveMapSize.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::GISourceChanged));
	m_Data.m_Falloff.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightChanged));
	m_Data.m_Range.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::TransformChanged));
	m_Data.m_ShadowQuality.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightChanged));
	m_Data.m_bDiffuseEnabled.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightChanged));
	m_Data.m_bSpecularEnabled.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightChanged));
	m_Data.m_LightSize.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightChanged));
	m_Data.m_PCSSAdjust.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightChanged));
//	m_Data.m_SceneScale.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightChanged));
	m_Data.m_ShadowIntensity.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightChanged));
	m_Data.m_ShadowColor.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightChanged));
	m_Data.m_DepthBias.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::TransformChanged));
//	m_Data.m_bAffectsFur.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightChanged));
	m_Data.m_bAffectsGlow.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightChanged));

	//--------Hair Properties-----------------
#ifdef HAIR_SUPPORTED
	m_Data.m_HairShadowBias.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::HairChanged));
	m_Data.m_HairShadowDensity.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::HairChanged));
	m_Data.m_HairShadowEnable.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::OpacityMapChanged));
	m_Data.m_HairShadowSize.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::OpacityMapChanged));
	m_Data.m_HairShadowType.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::OpacityMapChanged));
	m_Data.m_HairMinBound.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::HairChanged));
	m_Data.m_HairMaxBound.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::HairChanged));
#endif//HAIR_SUPPORTED

	m_Data.m_bShaftVisible.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightShaftChanged));
	m_Data.m_ShaftAlpha.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightShaftChanged));
	m_Data.m_ShaftDensity.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightShaftChanged));
	m_Data.m_ShaftDistFalloffStart.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightShaftChanged));
	m_Data.m_ShaftDistFalloffEnd.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightShaftChanged));
	m_Data.m_ShaftTextureFilename.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightShaftTextureChanged));
	//m_Data.m_bEnabledRamp.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::RampEnabledChanged));
	//m_Data.m_RampTrigger.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::RampTriggerChanged));
	
	// this callback is shared by all the properties in ramp data, user must use shared-pointer here
	shared_ptr<prtyPropertyCallback> rampCallbackPtr(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::RampChangedFromData));
	m_Data.m_RampData.RegisterCallback(rampCallbackPtr);
	shared_ptr<prtyPropertyCallback> shaftRampCallbackPtr(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::ShaftRampChangedFromData));
	m_Data.m_RampData.RegisterCallback(shaftRampCallbackPtr);
	/*m_Data.m_RampGradient.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::RampChanged));
	m_Data.m_RampShape.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::RampChanged));
	m_Data.m_RampInterpolation.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::RampChanged));
	m_Data.m_RampTexSize.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::RampChanged));
	m_Data.m_RampUWave.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::RampChanged));
	m_Data.m_RampVWave.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::RampChanged));
	m_Data.m_RampNoise.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::RampChanged));
	m_Data.m_RampNoiseFreq.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::RampChanged));*/

	// Properties just for the interface
	m_Yaw.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::PitchYawChanged));
	m_Pitch.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::PitchYawChanged));
	m_ManipMode.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::ManipModeChanged));
	m_LogAspect.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LogAspectChanged));
	m_ShowFrustrum.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::IconDisplayChanged));
	m_ShowTexture.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::IconDisplayChanged));
	m_TexturePosition.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::IconDisplayChanged));	
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prjltProjectedLightObject::~prjltProjectedLightObject()
{
	if (m_Data.m_LightType == prjltData::e_DirectionalLight)
		api3dScene::RemoveAutoResizeLight(m_pProjectedLight);

	icnIconScale::UnRegisterScaleInterest(this);

	ltstLightSetMgr::RemoveLight(this, m_pProjectedLight);	
	ltstIsolateMgr::RemoveLight(this, this);	
	xfrmTransformMgr::RemoveObject(this); 

	// delete proxies
	delete m_pLightProxy;
	delete m_ShadowCameraProxy;

	delete m_pRangeIcon;

	delete m_pObject;
	api3dLightMgr::DestroyLight(m_pProjectedLight);

	//	release textures
	if (m_pTexture)
		matTextureMgr::ReleaseTexture(m_pTexture);
	if (m_pShadowMap)
		matTextureMgr::ReleaseTexture(m_pShadowMap);
	/*if (m_pReflectiveMap)
		matTextureMgr::ReleaseTexture(m_pReflectiveMap);*/

	for( int i = 0; i < NUM_OPACITY_MAPS; i++ )
	{
		if( m_pOpacityShadowMap[i] )
		{
			matTextureMgr::ReleaseTexture( m_pOpacityShadowMap[i] );
		}
	}
	if( m_pOpacityVolume )
	{
		matTextureMgr::ReleaseTexture( m_pOpacityVolume );
	}
	if (m_pMapRenderer)
	{
		api3dTargetRendererMgr::RemoveTargetRenderer(m_pMapRenderer);
		delete m_pMapRenderer;
	}
	/*if (m_pRSMTargetRenderer)
	{
		api3dTargetRendererMgr::RemoveTargetRenderer(m_pRSMTargetRenderer);
		delete m_pRSMTargetRenderer;
	}*/
	if (m_pOpacityMapRenderer)
	{
		api3dTargetRendererMgr::RemoveTargetRenderer(m_pOpacityMapRenderer);
		delete m_pOpacityMapRenderer;
	}
	delete m_pDepthRenderer;
	delete m_pOpacityRenderer;

	//delete m_pRSMRenderer;

	if (m_pShaftTexture)
	{
		if(m_pShaftTexture == m_pShaftRampTexture)
			m_pShaftRampTexture = NULL;
		matTextureMgr::ReleaseTexture(m_pShaftTexture);
	}
	if (m_pShaftRampTexture)
	{
		matTextureMgr::ReleaseTexture(m_pShaftRampTexture);
		m_pShaftRampTexture = NULL;
	}
	if (m_pLightShaft)
	{
		api3dScene::RemoveObject(m_pLightShaft);
		delete m_pLightShaft;
		//delete m_pLightShaftMat;
	}
	
	if (m_pRampTexture)
	{
		matTextureMgr::ReleaseTexture(m_pRampTexture);
		m_pRampTexture = NULL;
	}
	
	//CleanupRampScene();
}


//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string prjltProjectedLightObject::GetDisplayName() const
{
	return GetName().GetString();
}

//--------------------------------------------------------------------
//	UpdateIconScale - function that sets the icons scale.
//--------------------------------------------------------------------
//virtual 
void prjltProjectedLightObject::UpdateIconScale( int i_IconLayerIndex, const camCamera& i_Camera, float i_Scale )
{
	if( m_pObject )
	{
		m_pObject->UpdateIconScale(i_IconLayerIndex, i_Camera, i_Scale);
	}
}


//--------------------------------------------------------------------
// Get values as a data structure
//--------------------------------------------------------------------
const prjltData& prjltProjectedLightObject::GetData() const
{
	return m_Data;

}

//--------------------------------------------------------------------
// Set from light data structure
//--------------------------------------------------------------------
void prjltProjectedLightObject::SetData( const prjltData &i_Data )
{
	// I am going to leave these "confirm" functions in SetData, because
	// we need to make sure that the textures and depth maps are created even
	// when all of the property values are at their default value 
	// (when no callbacks are called)
	this->ConfirmTexture();

	// don't tell the light if we're not casting shadows.
	// this is because that flag is used to determine whether
	// the light contributes to the ambient pass or not.
	// maybe these 2 concepts should be separated semantically?
	//m_pLightProxy->SetCastsShadow(i_Data.m_ShadowSource);
	
	this->ConfirmDepthMap();
	this->ConfirmOpacityMap();
	this->ConfirmLightShaft();
	//this->ConfirmReflectiveMap();

	// Set the new data into our properties, triggering callbacks
	m_Data = i_Data;

	// Get initial value of falloff type and then let user change it after this
	cmmFalloffAspect::InitializeFalloffType();

	if( m_Data.m_TextureFilename.GetFullValue().m_CurrentCallback == 
		prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Ramp) )
	{
		m_Data.m_bEnabledRamp.SetValue(true);
		this->RampChanged(false);
	}

	if( m_Data.m_ShaftTextureFilename.GetFullValue().m_CurrentCallback == 
		prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Ramp) )
	{
		m_Data.m_bEnabledShaftRamp.SetValue(true);
		this->ShaftRampChanged(false);
		UpdateLightShaft();
	}
}

//--------------------------------------------------------------------
// Name property access
//--------------------------------------------------------------------
prtyName&	prjltProjectedLightObject::PropertyName()
{
	return m_Data.m_Name;
}
const prtyName&	prjltProjectedLightObject::GetPropertyName() const
{
	return m_Data.m_Name;
}

//--------------------------------------------------------------------
// FileName property access
//--------------------------------------------------------------------
prtyTextureFileName&	prjltProjectedLightObject::PropertyTextureFileName()
{
	return m_Data.m_TextureFilename;
}
const prtyTextureFileName&	prjltProjectedLightObject::GetPropertyTextureFileName() const
{
	return m_Data.m_TextureFilename;
}

//--------------------------------------------------------------------
// Position property access
//--------------------------------------------------------------------
prtyPoint3d&	prjltProjectedLightObject::PropertyPosition()
{
	return m_Data.m_Position;
}
const prtyPoint3d&	prjltProjectedLightObject::GetPropertyPosition() const
{
	return m_Data.m_Position;
}

//--------------------------------------------------------------------
// Target property access
//--------------------------------------------------------------------
prtyPoint3d&	prjltProjectedLightObject::PropertyTarget()
{
	return m_Data.m_Target;
}
const prtyPoint3d&	prjltProjectedLightObject::GetPropertyTarget() const
{
	return m_Data.m_Target;
}

//--------------------------------------------------------------------
// Orientation property access
//--------------------------------------------------------------------
prtyRotation&	prjltProjectedLightObject::PropertyOrientation()
{
	return m_Data.m_Orientation;
}
const prtyRotation&	prjltProjectedLightObject::GetPropertyOrientation() const
{
	return m_Data.m_Orientation;
}

//--------------------------------------------------------------------
// Color property access
//--------------------------------------------------------------------
prtyColor&	prjltProjectedLightObject::PropertyColor()
{
	return m_Data.m_Color;
}
const prtyColor&	prjltProjectedLightObject::GetPropertyColor() const
{
	return m_Data.m_Color;
}

//--------------------------------------------------------------------
// Enabled property access
//--------------------------------------------------------------------
prtyBoolean&	prjltProjectedLightObject::PropertyEnabled()
{
	return m_Data.m_Enabled;
}
const prtyBoolean&	prjltProjectedLightObject::GetPropertyEnabled() const
{
	return m_Data.m_Enabled;
}

//--------------------------------------------------------------------
// ShadowSource property access
//--------------------------------------------------------------------
prtyBoolean&	prjltProjectedLightObject::PropertyShadowSource()
{
	return m_Data.m_ShadowSource;
}
const prtyBoolean&	prjltProjectedLightObject::GetPropertyShadowSource() const
{
	return m_Data.m_ShadowSource;
}

//--------------------------------------------------------------------
// Tilt property access
//--------------------------------------------------------------------
prtyFloat&	prjltProjectedLightObject::PropertyTilt()
{
	return m_Data.m_Tilt;
}
const prtyFloat&	prjltProjectedLightObject::GetPropertyTilt() const
{
	return m_Data.m_Tilt;
}

//--------------------------------------------------------------------
// Range property access
//--------------------------------------------------------------------
prtyFloat&	prjltProjectedLightObject::PropertyRange()
{
	return m_Data.m_Range;
}
const prtyFloat&	prjltProjectedLightObject::GetPropertyRange() const
{
	return m_Data.m_Range;
}

//--------------------------------------------------------------------
// Intensity property access
//--------------------------------------------------------------------
prtyFloat&	prjltProjectedLightObject::PropertyIntensity()
{
	return m_Data.m_Intensity;
}
const prtyFloat&	prjltProjectedLightObject::GetPropertyIntensity() const
{
	return m_Data.m_Intensity;
}

//--------------------------------------------------------------------
// Angle property access
//--------------------------------------------------------------------
prtyFloat&	prjltProjectedLightObject::PropertyAngle()
{
	return m_Data.m_Angle;
}
const prtyFloat&	prjltProjectedLightObject::GetPropertyAngle() const
{
	return m_Data.m_Angle;
}

//--------------------------------------------------------------------
// Penumbra property access
//--------------------------------------------------------------------
prtyFloat&	prjltProjectedLightObject::PropertyPenumbra()
{
	return m_Data.m_Penumbra;
}
const prtyFloat&	prjltProjectedLightObject::GetPropertyPenumbra() const
{
	return m_Data.m_Penumbra;
}

//--------------------------------------------------------------------
// Scale property access
//--------------------------------------------------------------------
prtyFloat&	prjltProjectedLightObject::PropertyScale()
{
	return m_Data.m_Scale;
}
const prtyFloat&	prjltProjectedLightObject::GetPropertyScale() const
{
	return m_Data.m_Scale;
}

//--------------------------------------------------------------------
// Aspect property access
//--------------------------------------------------------------------
prtyFloat&	prjltProjectedLightObject::PropertyAspect()
{
	return m_Data.m_Aspect;
}
const prtyFloat&	prjltProjectedLightObject::GetPropertyAspect() const
{
	return m_Data.m_Aspect;
}

//--------------------------------------------------------------------
// ShadowIntensity property access
//--------------------------------------------------------------------
prtyFloat&	prjltProjectedLightObject::PropertyShadowIntensity()
{
	return m_Data.m_ShadowIntensity;
}
const prtyFloat&	prjltProjectedLightObject::GetPropertyShadowIntensity() const
{
	return m_Data.m_ShadowIntensity;
}

//--------------------------------------------------------------------
// DepthBias property access
//--------------------------------------------------------------------
prtyFloat&	prjltProjectedLightObject::PropertyDepthBias()
{
	return m_Data.m_DepthBias;
}
const prtyFloat&	prjltProjectedLightObject::GetPropertyDepthBias() const
{
	return m_Data.m_DepthBias;
}

//--------------------------------------------------------------------
// LightSize property access
//--------------------------------------------------------------------
prtyFloat&	prjltProjectedLightObject::PropertyLightSize()
{
	return m_Data.m_LightSize;
}
const prtyFloat&	prjltProjectedLightObject::GetPropertyLightSize() const
{
	return m_Data.m_LightSize;
}

//--------------------------------------------------------------------
// PCSSAdjust property access
//--------------------------------------------------------------------
prtyFloat&	prjltProjectedLightObject::PropertyPCSSAdjust()
{
	return m_Data.m_PCSSAdjust;
}
const prtyFloat&	prjltProjectedLightObject::GetPropertyPCSSAdjust() const
{
	return m_Data.m_PCSSAdjust;
}

//--------------------------------------------------------------------
// DepthMapSize property access
//--------------------------------------------------------------------
prtyInt32&	prjltProjectedLightObject::PropertyDepthMapSize()
{
	return m_Data.m_DepthMapSize;
}
const prtyInt32&	prjltProjectedLightObject::GetPropertyDepthMapSize() const
{
	return m_Data.m_DepthMapSize;
}

//--------------------------------------------------------------------
// DepthMapSize property access
//--------------------------------------------------------------------
prtyInt32&	prjltProjectedLightObject::PropertyRSMapSize()
{
	return m_Data.m_ReflectiveMapSize;
}
const prtyInt32&	prjltProjectedLightObject::GetPropertyRSMapSize() const
{
	return m_Data.m_ReflectiveMapSize;
}

//--------------------------------------------------------------------
// ShaftVisible property access
//--------------------------------------------------------------------
prtyBoolean&	prjltProjectedLightObject::PropertyShaftVisible()
{
	return m_Data.m_bShaftVisible;
}
const prtyBoolean&	prjltProjectedLightObject::GetPropertyShaftVisible() const
{
	return m_Data.m_bShaftVisible;
}

//--------------------------------------------------------------------
// ShaftAlpha property access
//--------------------------------------------------------------------
prtyFloat&	prjltProjectedLightObject::PropertyShaftAlpha()
{
	return m_Data.m_ShaftAlpha;
}
const prtyFloat& prjltProjectedLightObject::GetPropertyShaftAlpha() const
{
	return m_Data.m_ShaftAlpha;
}

//--------------------------------------------------------------------
// ShaftDensity property access
//--------------------------------------------------------------------
prtyFloat&	prjltProjectedLightObject::PropertyShaftDensity()
{
	return m_Data.m_ShaftDensity;
}
const prtyFloat&	prjltProjectedLightObject::GetPropertyShaftDensity() const
{
	return m_Data.m_ShaftDensity;
}

//--------------------------------------------------------------------
// EnableDiffuse property access
//--------------------------------------------------------------------
prtyBoolean&	prjltProjectedLightObject::PropertyEnableDiffuse()
{
	return m_Data.m_bDiffuseEnabled;
}
const prtyBoolean&	prjltProjectedLightObject::GetPropertyEnableDiffuse() const
{
	return m_Data.m_bDiffuseEnabled;
}

//--------------------------------------------------------------------
// EnableSpecular property access
//--------------------------------------------------------------------
prtyBoolean&	prjltProjectedLightObject::PropertyEnableSpecular()
{
	return m_Data.m_bSpecularEnabled;
}
const prtyBoolean&	prjltProjectedLightObject::GetPropertyEnableSpecular() const
{
	return m_Data.m_bSpecularEnabled;
}

//--------------------------------------------------------------------
// AffectsGlow property access
//--------------------------------------------------------------------
prtyBoolean&	prjltProjectedLightObject::PropertyAffectsGlow()
{
	return m_Data.m_bAffectsGlow;
}
const prtyBoolean&	prjltProjectedLightObject::GetPropertyAffectsGlow() const
{
	return m_Data.m_bAffectsGlow;
}

//--------------------------------------------------------------------
// Falloff property access
//--------------------------------------------------------------------
prtyPoint3d&	prjltProjectedLightObject::PropertyFalloff()
{
	return m_Data.m_Falloff;
}
const prtyPoint3d&	prjltProjectedLightObject::GetPropertyFalloff() const
{
	return m_Data.m_Falloff;
}

//--------------------------------------------------------------------
// ShaftFalloffStart property access
//--------------------------------------------------------------------
prtyFloat&	prjltProjectedLightObject::PropertyShaftFalloffStart()
{
	return m_Data.m_ShaftDistFalloffStart;
}
const prtyFloat&	prjltProjectedLightObject::GetPropertyShaftFalloffStart() const
{
	return m_Data.m_ShaftDistFalloffStart;
}

//--------------------------------------------------------------------
// ShaftFalloffEnd property access
//--------------------------------------------------------------------
prtyFloat& prjltProjectedLightObject::PropertyShaftFalloffEnd()
{
	return m_Data.m_ShaftDistFalloffEnd;
}
const prtyFloat& prjltProjectedLightObject::GetPropertyShaftFalloffEnd() const
{
	return m_Data.m_ShaftDistFalloffEnd;
}

//--------------------------------------------------------------------
// ShowTexture property access
//--------------------------------------------------------------------
prtyBoolean& prjltProjectedLightObject::PropertyShowTexture()
{
	return m_ShowTexture;
}
const prtyBoolean& prjltProjectedLightObject::GetPropertyShowTexture() const
{
	return m_ShowTexture;
}

//----------------------------------------------------------------------------
//	GetWorldBox returns a box which completely encloses the prjltProjectedLightObject.
//----------------------------------------------------------------------------
maAxisBox prjltProjectedLightObject::GetWorldBox(int i_IconLayerIndex) const
{
	float sph_rad = l_SphereRadius;

	if (m_pObject)
	{
		if (m_ManipMode.GetValue() == e_Target)
			sph_rad = m_pObject->GetPickTargetScale(i_IconLayerIndex);
		else
			sph_rad = m_pObject->GetPickPositionScale(i_IconLayerIndex); 
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
maPoint3d prjltProjectedLightObject::GetWorldPivot() const
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
void prjltProjectedLightObject::GetParentMatrix(maMatrix4x4 &o_Transformation) const
{
	// let transform manager compute the sum of the parent matrices
	xfrmTransformMgr::ComputeExclusiveMatrixForNode(this->GetName(), o_Transformation);
}


//--------------------------------------------------------------------
//	Name
//--------------------------------------------------------------------
void prjltProjectedLightObject::SetName(const nameString& i_Name)
{
    // Set name first, which may change the ID number
    nameObject::SetName(i_Name);

    // Make sure that the data matches our true name (including
    // ID number)
    m_Data.m_Name.SetValue(this->GetName());

	// Update LightSetMgr
	ltstLightSetMgr::LightRenamed(this);
}

//----------------------------------------------------------------------------
//	Position
//----------------------------------------------------------------------------
maPoint3d prjltProjectedLightObject::GetPosition() const
{
	if (m_ManipMode.GetValue() == e_Target)
		return m_Data.m_Target.GetValue();
	else
		return m_Data.m_Position.GetValue();
}

//----------------------------------------------------------------------------
//	UpdatePosition - compass interaction has altered the position of
//	of this light
//----------------------------------------------------------------------------
void prjltProjectedLightObject::UpdatePosition(const maPoint3d& i_Position, 
										bool i_bNewOperation)
{
	const bool bSetDirty = true;
	switch (m_ManipMode.GetValue())
	{
	case e_Light:
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
maRotation prjltProjectedLightObject::GetOrientation() const
{
	//return l_NoRot;

	if (m_Data.m_LightType == prjltData::e_DirectionalLight)
	{
		return m_Data.m_Orientation.GetValue();
	}
	else
	{
		return maRotation(-m_Pitch.GetValue() * maConstants::c_fAngleToRad, 
						  m_Yaw.GetValue() * maConstants::c_fAngleToRad, 
						  0);	// don't do tilt here
	}
}
void prjltProjectedLightObject::UpdateOrientation(const maRotation& i_Orientation, 
										bool i_bNewOperation)
{		
	// We don't store the light's direction as a rotation. The rotation
	//	was derived from the pitch and yaw angles which are derived
	//	from vector from the light position to the target point.
	// So, when the rotation compass changes, all we are doing is
	//	moving the target or light position based on the manip mode.
	//
	if (m_Data.m_LightType != prjltData::e_DirectionalLight)
	{
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
	else
	{
		// Directional lights do have an orientation property
		m_Data.m_Orientation = i_Orientation;
	}
}

//----------------------------------------------------------------------------
//	Scale
//----------------------------------------------------------------------------
maPoint3d prjltProjectedLightObject::GetScale() const
{
	float scale = m_pLightProxy->GetScale();
	return maPoint3d(scale, scale, scale);
	//return l_One;
}

void prjltProjectedLightObject::UpdateScale(const maPoint3d& i_Scale, 
										bool i_bNewOperation)
{
	if (i_bNewOperation) this->CreateUndoForProperty(m_Data.m_Scale);
	const bool bSetDirty = true;
	m_Data.m_Scale.SetValue((i_Scale.m_X > c_MinimumScale) ? i_Scale.m_X : c_MinimumScale, bSetDirty);
}



//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
bool prjltProjectedLightObject::MatchPickCode(envType::UInt32 i_PickCode)
{
	if (!m_pObject->GetRenderable())
		return false;

	if (m_pObject->PositionContainsPickCode(i_PickCode))
	{
		this->m_ManipMode.SetValue(e_Light);
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
//	Renderable sets whether the icon is visible
//----------------------------------------------------------------------------
void prjltProjectedLightObject::SetRenderable(bool i_bRenderable)
{
	m_bRenderable = i_bRenderable;

	// icon is visible only if channel and gui and layer are all
	// are set visible == true;
	bool bRenderable = m_Data.m_bEditorVisible.GetValue() 
		&& m_bRenderable
		&& m_bLayerVisible
		&& this->GetIsolationVisible();
	m_pObject->SetRenderable( bRenderable );
}

//--------------------------------------------------------------------
// Set object active state in the render layer
//--------------------------------------------------------------------
void prjltProjectedLightObject::SetActiveRenderLayer(bool i_bActive)
{
}

//--------------------------------------------------------------------
//  Changes visible state of light based on GUI
//--------------------------------------------------------------------
void  prjltProjectedLightObject::SetEditorVisible(bool i_bVisible)
{
	m_Data.m_bEditorVisible = i_bVisible;

	// icon is visible only if channel and gui and layer are all
	// are set visible == true;
	bool bRenderable = m_Data.m_bEditorVisible.GetValue() 
		&& m_bRenderable
		&& m_bLayerVisible;
	m_pObject->SetRenderable( bRenderable );
}
bool prjltProjectedLightObject::GetEditorVisible() const
{
	return m_Data.m_bEditorVisible.GetValue();
}
 
//--------------------------------------------------------------------
//	LayerVisible represents if objects in the layer are visible
//--------------------------------------------------------------------
void prjltProjectedLightObject::SetLayerVisible(bool i_bVisible)
{
	m_bLayerVisible = i_bVisible;

	// object is visible only if channel and gui and layer are all
	// are set visible == true;
	bool bRenderable = m_Data.m_bEditorVisible.GetValue() 
		&& m_bRenderable
		&& m_bLayerVisible;
	m_pObject->SetRenderable(bRenderable);
}

//--------------------------------------------------------------------
//	GPUPickable sets whether the icons should be rendered
//	in pick renders when doing GPU picking
//--------------------------------------------------------------------
void prjltProjectedLightObject::SetGPUPickable(bool i_Pickable)
{
	m_bLayerPickable = i_Pickable;
	bool bParentPickable = xfrmTransformMgr::GetParentPickable(this->GetName());
	if (m_pObject)
		m_pObject->SetPickable(m_bLayerPickable && bParentPickable);
}

//----------------------------------------------------------------------------
// Flags for how object can be modified
//----------------------------------------------------------------------------
mnmObject::RotateFlags	prjltProjectedLightObject::GetRotateFlags()
{
	return mnmObject::e_RotateXY;
}
mnmObject::ScaleFlags	prjltProjectedLightObject::GetScaleFlags()
{
	return mnmObject::e_ScaleUniform;
}
mnmObject::TranslateFlags	prjltProjectedLightObject::GetTranslateFlags()
{
	return mnmObject::e_TranslateAll;
}

//--------------------------------------------------------------------
// Return this light's range icon
//--------------------------------------------------------------------
prjltRangeIcon* prjltProjectedLightObject::GetRangeIcon()
{
	return m_pRangeIcon;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prjltProjectedLightObject::SelectObject()
{
	// In case the dialog util hasn't been initialized
	//rmpDialogUtil::Show();
	NotifyRampUI();

	if (!m_Data.m_bEnabledRamp.GetValue())
	{
		rmpDialogUtil::Hide();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prjltProjectedLightObject::DeSelectObject()
{
	rmpDialogMgr::SetRampChangedCallback(NULL);
	rmpDialogUtil::Hide();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dProjectedLight* prjltProjectedLightObject::GetLight()
{
	return m_pProjectedLight;
}

//----------------------------------------------------------------------------
// Get parent object of the spline in order for selection to trace from
//	this spline back to the controlled object.
//----------------------------------------------------------------------------
//virtual 
//sel3dObject* prjltProjectedLightObject::GetParentObject() const
//{
//	return m_pParent;
//}
////virtual 
//void prjltProjectedLightObject::SetParentObject(sel3dObject* i_pParent)
//{
//	m_pParent = i_pParent;
//}
		
//--------------------------------------------------------------------
// Makes sure that the texture in the light matches the 
//	properties in the data structure
//--------------------------------------------------------------------
void prjltProjectedLightObject::ConfirmTexture()
{	
	// Create texture to project for the light
	//
	
	if (!m_pTexture || (m_Data.m_TextureFilename.GetValue() != m_TextureNameLoaded))
	{
		// stop any render threads for texture manager changes
		gpxRenderControl::ConfirmSingleThread();

		if (m_pTexture)
		{
			matTextureMgr::ReleaseTexture(m_pTexture);
			m_pTexture = NULL;
		}

		fsLocator tex_loc = m_Data.m_TextureFilename.GetValue();
		if (tex_loc.GetNumNames() > 0)
		{
			// load texture to project (have to make sure this isn't a mip-map
			// texture, or else you get a line at the light's plane.)
			//
			// FIX - the texture filename coming in may be ".dds", but the
			//	texture loader will search for other extensions as well.  The
			//	result is that the filename stored in data fields (and the resource
			//	tracker) may have the wrong extension.  This is a problem when 
			//	trying to replace a file in the resource tracker. [rjk]
			//
			m_pTexture = matTextureMgr::LoadTexture(tex_loc, TEXTURE_TYPE_2D, false);

			//	replace the filename in the resource tracker
			//
			fsResourceTracker::ReplaceFile(m_TextureNameLoaded, tex_loc);
		}

		//	set the name
		m_TextureNameLoaded = m_Data.m_TextureFilename.GetValue();

		// Assign texture to projected light
		//
		if(!m_Data.m_bEnabledRamp.GetValue())
			m_pProjectedLight->SetTexture(m_pTexture);
	}

	// 3d icon may be displaying texture, so we need to update it here
	this->update_3d_icon();
}

//--------------------------------------------------------------------
// Makes sure that the depth map in the light matches the 
//	properties in the data structure
//--------------------------------------------------------------------
void prjltProjectedLightObject::ConfirmDepthMap()
{	
	if (m_Data.m_ShadowSource.GetValue())
	{
		int new_depth_map_size = m_Data.m_DepthMapSize.GetValue();
		if (new_depth_map_size < 256)
			new_depth_map_size = 256;
		else if (new_depth_map_size > 8192)
			new_depth_map_size = 8192;
		else
		{
			// nearest power of 2:
			// x = 2 ^ (round(log2(x)))
			new_depth_map_size = 1 << (int)floor(maFunctions::Log((float)new_depth_map_size,2.0f)+0.5f);
		}

		//if (PrefsMgr::Data().m_bNoDepthMaps.GetValue() || !m_Data.m_Enabled.GetValue())
		if (!m_Data.m_Enabled.GetValue())
		{
			// Instead of dealing with NULL pointers, just set the
			// depth map to a very small size when they are turned off.
			new_depth_map_size = 4;
		}
		else if (PrefsMgr::Data().m_DepthMapReduce.GetValue() > 0)
		{
			// Reduce size of depth maps
			new_depth_map_size >>= PrefsMgr::Data().m_DepthMapReduce.GetValue();
			if (new_depth_map_size < 4)
				new_depth_map_size = 4;
		}

		// Create depth map for rendering shadows
		if (!m_pShadowMap || (new_depth_map_size != m_DepthMapSize))
		{
			// stop any render threads for texture manager changes
			gpxRenderControl::ConfirmSingleThread();

			if (m_pShadowMap)
				matTextureMgr::ReleaseTexture(m_pShadowMap);

			// make the render target texture
			try
			{
				m_pShadowMap = matTextureMgr::CreateShadowMap( new_depth_map_size, new_depth_map_size );
			}
			catch (const g2dOutOfVideoMemoryX& )
			{
				guiMessageBox::Show("Not enough video mem to create shadow map texture. Switching shadows off for this light.", "Error", guiMessageBox::e_OKOnly);
				m_pShadowMap = NULL;
				m_Data.m_ShadowSource.SetValue(false);
			}


			m_DepthMapSize = new_depth_map_size;

			// Assign textures to projected light
			m_pProjectedLight->SetShadowMap(m_pShadowMap);

			// Set up Target renderer for updating the depth map
			if (m_pMapRenderer)
			{
				api3dTargetRendererMgr::RemoveTargetRenderer(m_pMapRenderer);
				delete m_pMapRenderer;
				m_pMapRenderer = NULL;
			}
			
			if (m_pShadowMap != NULL)
			{
				g2dRenderTarget* pTarget = m_pShadowMap->GetRenderTargetAPI();
				// this might need to be within api3d somehow?
				m_pMapRenderer = new g3dTargetRenderer(pTarget, m_pDepthRenderer, api3dScene::GetScene(), &m_ShadowCamera );
				m_pMapRenderer->SetBackgroundColor(g2dRGBColor(0xff, 0xff, 0xff));
				api3dTargetRendererMgr::AddTargetRenderer(m_pMapRenderer);
			}
		}
	}
	else
	{
		// Remove shadow map, if we have one
		if (m_pShadowMap || m_pMapRenderer || m_pProjectedLight->GetShadowMap())
		{		
			// stop any render threads for texture manager changes
			gpxRenderControl::ConfirmSingleThread();

			if (m_pShadowMap)
			{
				matTextureMgr::ReleaseTexture(m_pShadowMap);
				m_pShadowMap = NULL;
			}
			if (m_pMapRenderer)
			{
				api3dTargetRendererMgr::RemoveTargetRenderer(m_pMapRenderer);
				delete m_pMapRenderer;
				m_pMapRenderer = NULL;
			}
			m_pProjectedLight->SetShadowMap(NULL);
		}
	}
}

//--------------------------------------------------------------------
// Makes sure that the depth map in the light matches the 
//	properties in the data structure
//--------------------------------------------------------------------
void prjltProjectedLightObject::ConfirmOpacityMap()
{
	if (m_Data.m_HairShadowEnable.GetValue())
	{
		int new_depth_map_size = m_Data.m_HairShadowSize.GetValue();
		if (new_depth_map_size < 64)
			new_depth_map_size = 64;
		else if (new_depth_map_size > 8192)
			new_depth_map_size = 8192;
		else
		{
			// nearest power of 2:
			// x = 2 ^ (round(log2(x)))
			new_depth_map_size = 1 << (int)floor(maFunctions::Log((float)new_depth_map_size,2.0f)+0.5f);
		}

		if (!m_Data.m_Enabled.GetValue())
		{
			// Instead of dealing with NULL pointers, just set the
			// depth map to a very small size when they are turned off.
			new_depth_map_size = 4;
		}
		else if (PrefsMgr::Data().m_DepthMapReduce.GetValue() > 0)
		{
			// Reduce size of depth maps
			new_depth_map_size >>= PrefsMgr::Data().m_DepthMapReduce.GetValue();
			if (new_depth_map_size < 4)
				new_depth_map_size = 4;
		}

		HAIR_SHADOW_TYPE new_shadow_type = (HAIR_SHADOW_TYPE)m_Data.m_HairShadowType.GetValue();

		// Create opacity map for rendering shadows
		if ((new_depth_map_size != m_HairShadowSize) || (new_shadow_type != m_HairShadowType))
		{
			// stop any render threads for texture manager changes
			gpxRenderControl::ConfirmSingleThread();

			for( int i = 0; i < NUM_OPACITY_MAPS; i++ )
			{
				if(	m_pOpacityShadowMap[i] )
				{
					matTextureMgr::ReleaseTexture( m_pOpacityShadowMap[i] );
					m_pOpacityShadowMap[i] = NULL;
				}
			}

			if( m_pOpacityVolume )
			{
				matTextureMgr::ReleaseTexture( m_pOpacityVolume );
				m_pOpacityVolume = NULL;
			}

			// make the render target texture
			try
			{
				int count = 1;
				if( new_shadow_type == HAIR_SHADOW_OSM16 ||
					new_shadow_type == HAIR_SHADOW_DOSM16 ) count = 4;
				else if( new_shadow_type == HAIR_SHADOW_OSM32 ||
					     new_shadow_type == HAIR_SHADOW_DOSM32 ) count = 8;
				for( int i = 0; i < count; i++ )
				{
					m_pOpacityShadowMap[i] = matTextureMgr::CreateRenderTargetTexture( new_depth_map_size, new_depth_map_size, 
						false, &l_ldrPFD, false, true, matTextureMgr::e_ShadowMap );
				}
				if( new_shadow_type == HAIR_SHADOW_VOLUME4 )
				{
					g2dPFD pfd8 = g2dPFD( g2dPFD::e_Luminance8, 8 );
					unsigned char* data = new unsigned char[ new_depth_map_size * new_depth_map_size * 4 ];
					unsigned char* wdata = data;

					memset( wdata, 0x00, (new_depth_map_size*new_depth_map_size*2) );
					wdata += new_depth_map_size* new_depth_map_size*2;
					memset( wdata, 0xff, (new_depth_map_size*new_depth_map_size*2) );

					m_pOpacityVolume = matTextureMgr::CreateTexture3D( new_depth_map_size, new_depth_map_size, 4, &pfd8, BIND_SHADER_RESOURCE | BIND_UNORDERED_ACCESS, data );
					delete[] data;
				}
			}
			catch (const g2dOutOfVideoMemoryX& )
			{
				guiMessageBox::Show("Not enough video mem to create opacity shadow map texture. Switching hair shadows off for this light.", "Error", guiMessageBox::e_OKOnly);
				for( int i = 0; i < NUM_OPACITY_MAPS; i++ )
				{
					if(	m_pOpacityShadowMap[i] )
					{
						matTextureMgr::ReleaseTexture( m_pOpacityShadowMap[i] );
						m_pOpacityShadowMap[i] = NULL;
					}
				}
				if( m_pOpacityVolume )
				{
					matTextureMgr::ReleaseTexture( m_pOpacityVolume );
					m_pOpacityVolume = NULL;
				}
				m_Data.m_HairShadowEnable.SetValue(false);
			}

			m_HairShadowSize = new_depth_map_size;
			m_HairShadowType = new_shadow_type;

			// Assign textures to projected light
			for( int s = 0; s < NUM_OPACITY_MAPS; s++ )
			{
				m_pProjectedLight->SetOpacityShadowMap( m_pOpacityShadowMap[s],s );
			}
			m_pProjectedLight->SetOpacityVolume( m_pOpacityVolume );

			// Set up Target renderer for updating the depth map
			if (m_pOpacityMapRenderer)
			{
				api3dTargetRendererMgr::RemoveTargetRenderer(m_pOpacityMapRenderer);
				delete m_pOpacityMapRenderer;
				m_pOpacityMapRenderer = NULL;
			}

			if (m_pOpacityShadowMap[0] != NULL)
			{
				g2dRenderTarget* pTarget = m_pOpacityShadowMap[0]->GetRenderTargetAPI();
				// this might need to be within api3d somehow?
				m_pOpacityMapRenderer = new g3dTargetRenderer(pTarget, m_pOpacityRenderer, api3dScene::GetScene(), &m_ShadowCamera );
				api3dTargetRendererMgr::AddTargetRenderer(m_pOpacityMapRenderer, api3dTargetRendererMgr::e_Opacity );
			}
		}
	}
	else
	{
		// Remove shadow map, if we have one
		if (m_pOpacityShadowMap[0] || m_pOpacityMapRenderer || m_pProjectedLight->GetOpacityShadowMap())
		{		
			// stop any render threads for texture manager changes
			gpxRenderControl::ConfirmSingleThread();

			for( int i = 0; i < NUM_OPACITY_MAPS; i++ )
			{
				if(	m_pOpacityShadowMap[i] )
				{
					matTextureMgr::ReleaseTexture( m_pOpacityShadowMap[i] );
					m_pOpacityShadowMap[i] = NULL;
				}
				m_pProjectedLight->SetOpacityShadowMap(NULL,i);
			}
			m_pProjectedLight->SetOpacityVolume( NULL );

			if (m_pOpacityMapRenderer)
			{
				api3dTargetRendererMgr::RemoveTargetRenderer(m_pOpacityMapRenderer);
				delete m_pOpacityMapRenderer;
				m_pOpacityMapRenderer = NULL;
			}
		}
	}
}

//--------------------------------------------------------------------
// Creates/destroys light shaft object depending on 
//	properties in the data structure
//--------------------------------------------------------------------
void prjltProjectedLightObject::ConfirmLightShaft()
{
	// Moved into the individual cases to avoid aborting when not needed...
	//gpxRenderControl::ConfirmSingleThread();

	if (m_Data.m_bShaftVisible.GetValue())
	{
		// Do we always need to delete the old light shaft? 
		if (m_pLightShaft)
		{
			gpxRenderControl::ConfirmSingleThread();

			// clean up the old one
			api3dScene::RemoveObject(m_pLightShaft);
			delete m_pLightShaft;
			m_pLightShaft = NULL;
			//delete m_pLightShaftMat; // material is deleted with api3dobject
		}

		if (m_Data.m_Enabled.GetValue())
		{
			gpxRenderControl::ConfirmSingleThread();

			// cone is located at adjusted light pos = pos - scale*direction
			// therefore the height of the cone is scale+range.
			// the cone should be truncated at scale.
			// that would make it a tapered cylinder.
			float fullConeHeight = m_Data.m_Scale.GetValue() + m_Data.m_Range.GetValue();
			float truncatedHeight = m_Data.m_Range.GetValue();
			float baseRadius = fullConeHeight*tan(maConstants::c_fAngleToRad*m_Data.m_Angle.GetValue()*0.5f);
			float truncatedRadius = baseRadius * m_Data.m_Scale.GetValue() / fullConeHeight;
			int divisions = 360;
			m_pLightShaft = api3dShape::CreateCylinder(m_Data.m_Color.GetValue(), 
				truncatedRadius, baseRadius, truncatedHeight, divisions);
			m_pLightShaft->Fragment()->SetCastsShadow(false);
			m_pLightShaft->Object()->GetBase()->SetCastsShadow(false);
			m_pLightShaft->Fragment()->SetReceivesShadow(true);
			m_pLightShaft->Fragment()->SetDoubleSided(true);
			m_pLightShaft->SetGPUPickable(false);
			m_pLightShaft->Fragment()->SetReceivesGI(false);

			m_pLightShaftMat = m_pLightShaft->Material();
			m_pLightShaftMat->ForceTransparency(true);
			m_pLightShaftMat->SetAdditive(true);
			m_pLightShaftMat->SetBelongsToLightShaft(true);

			// set up the shader properties of the material
			effLightGlowData lgdata;
			m_pLightShaftMat->SetShaderEffect("LightGlow.fx", &lgdata);
			api3dScene::AddObject(m_pLightShaft);
		}
	}
	else
	{
		if (m_pLightShaft)
		{
			gpxRenderControl::ConfirmSingleThread();

			// clean up the old one
			api3dScene::RemoveObject(m_pLightShaft);
			delete m_pLightShaft;
			m_pLightShaft = NULL;
			//delete m_pLightShaftMat; // material is deleted with api3dobject
		}
	}
}

//--------------------------------------------------------------------
// Makes sure that the depth map in the light matches the 
//	properties in the data structure
//--------------------------------------------------------------------
//void prjltProjectedLightObject::ConfirmReflectiveMap()
//{	
//	if (m_Data.m_GISource.GetValue())
//	{
//		int new_rsm_size = m_Data.m_ReflectiveMapSize.GetValue();
//		if (new_rsm_size < 256)
//			new_rsm_size = 256;
//		else if (new_rsm_size > 8192)
//			new_rsm_size = 8192;
//		else
//		{
//			// nearest power of 2:
//			// x = 2 ^ (round(log2(x)))
//			new_rsm_size = 1 << (int)floor(maFunctions::Log((float)new_rsm_size,2.0f)+0.5f);
//		}
//
//		//if (PrefsMgr::Data().m_bNoDepthMaps.GetValue() || !m_Data.m_Enabled.GetValue())
//		if (!m_Data.m_Enabled.GetValue())
//		{
//			// Instead of dealing with NULL pointers, just set the
//			// depth map to a very small size when they are turned off.
//			new_rsm_size = 4;
//		}
//
//		// Create depth map for rendering shadows
//		if (!m_pReflectiveMap || (new_rsm_size != m_ReflectiveMapSize))
//		{
//			// stop any render threads for texture manager changes
//			gpxRenderControl::ConfirmSingleThread();
//
//			if (m_pReflectiveMap)
//				matTextureMgr::ReleaseTexture(m_pReflectiveMap);
//
//			// make the render target texture
//			try
//			{
//				m_pReflectiveMap = matTextureMgr::CreateReflectiveShadowMap( new_rsm_size, new_rsm_size );
//			}
//			catch (const g2dOutOfVideoMemoryX& )
//			{
//				guiMessageBox::Show("Not enough video mem to create shadow map texture. Switching shadows off for this light.", "Error", guiMessageBox::e_OKOnly);
//				m_pReflectiveMap = NULL;
//				m_Data.m_GISource.SetValue(false);
//			}
//
//
//			m_ReflectiveMapSize = new_rsm_size;
//
//			// Assign textures to projected light
//			m_pProjectedLight->SetReflectiveMap(m_pReflectiveMap);
//
//			// Set up Target renderer for updating the depth map
//			if (m_pRSMTargetRenderer)
//			{
//				api3dTargetRendererMgr::RemoveTargetRenderer(m_pRSMTargetRenderer);
//				delete m_pRSMTargetRenderer;
//				m_pRSMTargetRenderer = NULL;
//			}
//			
//			if (m_pReflectiveMap != NULL)
//			{
//				g2dRenderTarget* pTarget = m_pReflectiveMap->GetRenderTargetAPI();
//				// this might need to be within api3d somehow?
//				m_pRSMTargetRenderer = new g3dTargetRenderer(pTarget, m_pRSMRenderer, api3dScene::GetScene(), &m_ShadowCamera );
//				m_pRSMTargetRenderer->SetBackgroundColor(g2dRGBColor(0xff, 0xff, 0xff));
//				api3dTargetRendererMgr::AddTargetRenderer(m_pRSMTargetRenderer);
//			}
//		}
//	}
//	else
//	{
//		// Remove shadow map, if we have one
//		if (m_pReflectiveMap || m_pRSMRenderer || m_pProjectedLight->GetReflectiveMap())
//		{		
//			// stop any render threads for texture manager changes
//			gpxRenderControl::ConfirmSingleThread();
//
//			if (m_pReflectiveMap)
//			{
//				matTextureMgr::ReleaseTexture(m_pReflectiveMap);
//				m_pReflectiveMap = NULL;
//			}
//			if (m_pRSMTargetRenderer)
//			{
//				api3dTargetRendererMgr::RemoveTargetRenderer(m_pRSMTargetRenderer);
//				delete m_pRSMTargetRenderer;
//				m_pRSMTargetRenderer = NULL;
//			}
//			m_pProjectedLight->SetReflectiveMap(NULL);
//		}
//	}
//}
//--------------------------------------------------------------------
// Updates position of light shaft
//--------------------------------------------------------------------
void prjltProjectedLightObject::UpdateLightShaft()
{
	if ((!m_pLightShaft) || (!m_Data.m_bShaftVisible.GetValue()))
		return;

	//bga - this shouldn't be here, the light shaft should be using proxies....
	gpxRenderControl::ConfirmSingleThread();

	// cone's origin is at its base, which is the target pos of our light
	maPoint3d pos, target;
	get_world_positions(pos, target);
	maVector3d dir = target - pos;
	dir.Normalize();
	//m_pLightShaft->SetPosition(m_pProjectedLight->GetPosition() + m_pProjectedLight->GetDirection()*m_Data.m_Range.GetValue());
	m_pLightShaft->SetPosition(pos+ dir * m_Data.m_Range.GetValue());

	// Squash based on aspect of light
	maVector3d scalevec( 1.0f/m_Data.m_Aspect.GetValue(), 1, 1);
	m_pLightShaft->SetScale(scalevec);

	// orient along light direction
	maRotation rot;
	maVector3d rotvec = (-dir);
	rotvec.Normalize();
	rot.SetValue(maVector3d(0,1,0), rotvec);
	// adjust for tilt and off-axis scaling:
	maRotation rotTilt(dir, 
		maConstants::c_fAngleToRad * m_Data.m_Tilt.GetValue() + atan2(rotvec.GetZ(), rotvec.GetX()));
	rot = rotTilt * rot;
	m_pLightShaft->SetOrientation(rot);

//	matShaderEffect* eff = m_pLightShaftMat->GetShaderEffect();
//	eff->SetTechnique(matShaderEffect::e_Default);

	effLightGlowData* pData = dynamic_cast<effLightGlowData*>(m_pLightShaftMat->GetEffectData());
	DBG_ASSERT(pData != NULL, "prjlt Light Shaft not using effLightGlow");
	pData->m_pLight = m_pProjectedLight;
	// set up texture in different func
	pData->m_TextureDiffuse = m_pShaftTexture;
	pData->m_EdgeFuzzCutoff = m_Data.m_ShaftDensity.GetValue();
	pData->m_DistFalloffStart = m_Data.m_ShaftDistFalloffStart.GetValue();// + m_Data.m_Scale.GetValue();
	pData->m_DistFalloffEnd = m_Data.m_ShaftDistFalloffEnd.GetValue();// + m_Data.m_Scale.GetValue();
	pData->m_GlowAlpha = m_Data.m_ShaftAlpha.GetValue();
	// guarantee transparency:
	if (pData->m_GlowAlpha >= 1)
		pData->m_GlowAlpha = 0.9999f;
}

//--------------------------------------------------------------------
// Makes sure that the texture in the light matches the 
//	properties in the data structure
//--------------------------------------------------------------------
void prjltProjectedLightObject::ConfirmLightShaftTexture()
{	
	// Create texture to project for the light
	//
	if (!m_pShaftTexture || (m_Data.m_ShaftTextureFilename.GetValue() != m_ShaftTextureNameLoaded))
	{
		// stop any render threads for texture manager changes
		gpxRenderControl::ConfirmSingleThread();

		if (m_pShaftTexture)
		{
			if(m_pShaftTexture == m_pShaftRampTexture)
				m_pShaftRampTexture = NULL;
			matTextureMgr::ReleaseTexture(m_pShaftTexture);
			m_pShaftTexture = NULL;
		}

		fsLocator tex_loc = m_Data.m_ShaftTextureFilename.GetValue();
		if (tex_loc.GetNumNames() > 0)
		{
			//
			// FIX - the texture filename coming in may be ".dds", but the
			//	texture loader will search for other extensions as well.  The
			//	result is that the filename stored in data fields (and the resource
			//	tracker) may have the wrong extension.  This is a problem when 
			//	trying to replace a file in the resource tracker. [rjk]
			//
			m_pShaftTexture = matTextureMgr::LoadTexture(tex_loc);

			//	replace the filename in the resource tracker
			//
			fsResourceTracker::ReplaceFile(m_ShaftTextureNameLoaded, tex_loc);
		}

		//	set the name
		m_ShaftTextureNameLoaded = tex_loc;

		// MUST call UpdateLightShaft() to install the texture properly!!!!
		// 3d icon may be displaying texture, so we need to update it here
		this->update_3d_icon();
	}
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void prjltProjectedLightObject::NameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// The property's check will prevent an infinite
	// loop here.
	this->SetName( m_Data.m_Name.GetValue() );

	if (i_bDirty)
	{
		prjltDialogDataUtil::UpdateListDialog();
		prjltDocumentChunk::ActiveDataChanged();
	}
}

void prjltProjectedLightObject::TextureChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	std::string callback( m_Data.m_TextureFilename.GetFullValue().m_CurrentCallback );

	//if the button on one of the texture options was clicked we need to pass
	//the necessary info to the manager of that operation
	if( m_Data.m_TextureFilename.GetFullValue().m_bButtonPressed )
	{
		//reset our texture locator
		prtyTextureFileData val;
		val.m_TextureLocator = m_Data.m_TextureFilename.GetValue();

		//decide which callback was executed
		if( callback == prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Paint) )
		{
			//do paint manager operations
			//brshPaintBrushMgr::SetMatTexture(pParam);
			m_Data.m_bEnabledRamp.SetValue(false);
		}
		else if( callback == prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Ramp) )
		{
			m_Data.m_RampData = GetRampData(m_Data.m_TextureFilename, m_Data.m_RampData);
			NotifyRampUI();
			m_Data.m_bEnabledRamp.SetValue(true);
			this->RampChanged(i_bDirty);
		}
		else if( callback == prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Reset) )
		{
			callback = prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Texture);
			m_Data.m_bEnabledRamp.SetValue(false);
			m_Data.m_RampData = rmpData();
			UpdateRampData(m_Data.m_TextureFilename, m_Data.m_RampData);
			this->ConfirmTexture();
		}

		val.m_CurrentCallback = callback;
		m_Data.m_TextureFilename.SetValueWithoutNotify(val);
	}

	else
	{
		m_Data.m_bEnabledRamp.SetValue(false);
		this->ConfirmTexture();
	}
	if (i_bDirty)
		prjltDocumentChunk::ActiveDataChanged();
}

void prjltProjectedLightObject::DepthMapChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// round depth map size to nearest positive power of 2
	this->ConfirmDepthMap();
	this->UpdateLightShaft();

	if (i_bDirty)
		prjltDocumentChunk::ActiveDataChanged();
}


void prjltProjectedLightObject::OpacityMapChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// round opacity map size to nearest positive power of 2
	ConfirmOpacityMap();

	m_pLightProxy->SetHairShadowType( (HAIR_SHADOW_TYPE)m_Data.m_HairShadowType.GetValue() );

	if (i_bDirty)
		prjltDocumentChunk::ActiveDataChanged();
}

void prjltProjectedLightObject::TransformChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// This callback is triggered from changes to the properties 
	// that require a call to "OrientCamera":
	// Position, Target, Scale, Aspect, Range, Angle
	//
	// Since this changes the transformation updates are made to
	// the world box and 3d icon also.
	this->transformation_changed();

	if (i_bDirty)
		prjltDocumentChunk::ActiveDataChanged();
}

void prjltProjectedLightObject::AngleMeaningChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// This callback is for the boolean checkboxes (Directional, Cone Lighting)
	// that control the  type of light frustrum and the meaning of the angle controls

	if (m_Data.m_LightType == prjltData::e_ProjectedLight)
	{
		// Adjust the UI controls
		if (this->m_pAngleControl)
		{
			this->m_pAngleControl->SetReadOnly(m_Data.m_bDirectional.GetValue() && !m_Data.m_bConeLighting.GetValue());
			this->m_pAngleControl->UpdateControl();
		}
		if (this->m_pPenumbraControl)
		{
			this->m_pPenumbraControl->SetReadOnly(!m_Data.m_bConeLighting.GetValue());
			this->m_pPenumbraControl->UpdateControl();
		}
	}

	// Since meaning of the angle properties changed, update light transformation
	this->transformation_changed();

	if (i_bDirty)
		prjltDocumentChunk::ActiveDataChanged();
}
void prjltProjectedLightObject::ColorChanged(prtyProperty *i_pProperty, bool i_bDirty)
{		
	// color is reflected in the light, the 3d icon and the light shaft
	m_pLightProxy->SetIntensity(m_Data.m_Color.GetValue());

	// Combine intensity with the isolation alpha for fades in and out
	m_pLightProxy->SetIntensityFactor(m_Data.m_Intensity.GetValue() * this->GetIsolationAlpha());


	m_pObject->SetColor(m_Data.m_Color.GetValue());

	this->UpdateLightShaft();

	if (i_bDirty)
		prjltDocumentChunk::ActiveDataChanged();
}

void prjltProjectedLightObject::EnabledChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// Enable light
	// Combine the isoalted and enabled states to determine if the light is "on"
	m_pLightProxy->SetEnable( m_Data.m_Enabled.GetValue() && this->GetIsolationVisible() );

	// These confirm calls will create/delete the related objects.
	this->ConfirmDepthMap();
	this->ConfirmOpacityMap();
	this->ConfirmLightShaft();
	//this->ConfirmReflectiveMap();
	this->UpdateLightShaft();

	if (i_bDirty)
		prjltDocumentChunk::ActiveDataChanged();
}
void prjltProjectedLightObject::ShadowSourceChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	//DBG_LOG("Light casts shadow: " << (m_Data.m_ShadowSource ? "True" : "False"));
//	m_pLightProxy->SetCastsShadow(m_Data.m_ShadowSource);

	this->ConfirmDepthMap();
	//this->ConfirmReflectiveMap();
	this->UpdateLightShaft();

	if (i_bDirty)
		prjltDocumentChunk::ActiveDataChanged();
}

void prjltProjectedLightObject::GISourceChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	m_pLightProxy->SetGIEnabled(m_Data.m_GISource.GetValue());
	//this->ConfirmReflectiveMap();

	/*if (i_bDirty)
		prjltDocumentChunk::ActiveDataChanged();*/
}

void prjltProjectedLightObject::LightChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// Set properties of light that are only reflected in the light itself,
	// not in the 3d icon or in the depth map
	//
	m_pLightProxy->SetFalloff0(m_Data.m_Falloff.GetValue()[0]);
	m_pLightProxy->SetFalloff1(m_Data.m_Falloff.GetValue()[1]);
	m_pLightProxy->SetFalloff2(m_Data.m_Falloff.GetValue()[2]);

	m_pLightProxy->SetDiffuseEnabled( m_Data.m_bDiffuseEnabled.GetValue() );
	m_pLightProxy->SetSpecularEnabled( m_Data.m_bSpecularEnabled.GetValue() );
	m_pLightProxy->SetAffectsGlow(m_Data.m_bAffectsGlow.GetValue());

	m_pLightProxy->SetLightSize(m_Data.m_LightSize.GetValue() * 0.05f);
	m_pLightProxy->SetPCSSAdjust(m_Data.m_PCSSAdjust.GetValue());

//	m_pLightProxy->SetSceneScale(m_Data.m_SceneScale.GetValue());

	m_pLightProxy->SetShadowQuality((g3dProjectedLight::ShadowQuality)m_Data.m_ShadowQuality.GetValue());

	m_pLightProxy->SetShadowIntensity(m_Data.m_ShadowIntensity.GetValue());
	m_pLightProxy->SetShadowColor(m_Data.m_ShadowColor.GetValue());

	//m_pLightProxy->SetShadowColor(m_Data.m_ShadowColor.GetValue());
	//m_pLightProxy->SetShadowColor(m_Data.m_ShadowColor.GetValue());
	//m_pLightProxy->SetShadowColor(m_Data.m_ShadowColor.GetValue());

	this->UpdateLightShaft();

	if (i_bDirty)
		prjltDocumentChunk::ActiveDataChanged();
}
void prjltProjectedLightObject::LightShaftChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// properties related to light shaft have changed

	this->ConfirmLightShaft();
	this->UpdateLightShaft();

	if (i_bDirty)
		prjltDocumentChunk::ActiveDataChanged();
}
void prjltProjectedLightObject::LightShaftTextureChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	std::string callback( m_Data.m_ShaftTextureFilename.GetFullValue().m_CurrentCallback );

	//if the button on one of the texture options was clicked we need to pass
	//the necessary info to the manager of that operation
	if( m_Data.m_ShaftTextureFilename.GetFullValue().m_bButtonPressed )
	{
		//reset our texture locator
		prtyTextureFileData val;
		val.m_TextureLocator = m_Data.m_ShaftTextureFilename.GetValue();

		//decide which callback was executed
		if( callback == prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Paint) )
		{
			//do paint manager operations
			//brshPaintBrushMgr::SetMatTexture(pParam);
			m_Data.m_bEnabledShaftRamp.SetValue(false);
		}
		else if( callback == prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Ramp) )
		{
			if( m_pShaftTexture && (m_pShaftTexture != m_pShaftRampTexture) )
			{
				matTextureMgr::ReleaseTexture(m_pShaftTexture);
				m_pShaftTexture = NULL;
			}
			m_Data.m_ShaftRampData = GetRampData(m_Data.m_ShaftTextureFilename, m_Data.m_ShaftRampData);
			NotifyShaftRampUI();
			m_Data.m_bEnabledShaftRamp.SetValue(true);
			this->ShaftRampChanged(i_bDirty);
		}
		else if( callback == prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Reset) )
		{
			if( m_pShaftTexture && (m_pShaftTexture == m_pShaftRampTexture) )
			{
				matTextureMgr::ReleaseTexture(m_pShaftTexture);
				m_pShaftTexture = NULL;
				m_pShaftRampTexture = NULL;
			}
			callback = prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Texture);
			m_Data.m_bEnabledShaftRamp.SetValue(false);
			m_Data.m_ShaftRampData = rmpData();
			UpdateRampData(m_Data.m_ShaftTextureFilename, m_Data.m_ShaftRampData);
			this->ConfirmLightShaftTexture();
		}

		val.m_CurrentCallback = callback;
		m_Data.m_ShaftTextureFilename.SetValueWithoutNotify(val);
	}

	else
	{
		m_Data.m_bEnabledShaftRamp.SetValue(false);
		this->ConfirmLightShaftTexture();
	}

	this->UpdateLightShaft();

	if (i_bDirty)
		prjltDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// This callback is for the pitch and yaw sliders that give a new
//	interface for altering the target, but are not real properties
//	that are animatable or written to a file.
//--------------------------------------------------------------------
void prjltProjectedLightObject::PitchYawChanged(prtyProperty *i_pProperty, bool i_bDirty)
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

		if (m_Data.m_LightType == prjltData::e_DirectionalLight)
		{
			// Directional lights set orientation
			maRotation euler(-pitch, yaw, 0);
			m_Data.m_Orientation.SetValue(euler);
		}
		else
		{
			// Other projected lights use target and position to set view
			if (m_ManipMode.GetValue() == e_Target)
				m_Data.m_Position.SetValue( m_Data.m_Target.GetValue() - view_dir );
			else
				m_Data.m_Target.SetValue( m_Data.m_Position.GetValue() + view_dir );
		}
	}
}

//--------------------------------------------------------------------
// This callback is for the manipulation mode enumeration which
//	causes the compass manipulation to center on the eye position
//	or the target.
//--------------------------------------------------------------------
void prjltProjectedLightObject::ManipModeChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
//	update_world_box();
}

//--------------------------------------------------------------------
// This callback is for the pitch and yaw sliders that give a new
//	interface for altering the target, but are not real properties
//	that are animatable or written to a file.
//--------------------------------------------------------------------
void prjltProjectedLightObject::LogAspectChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// We only want to do something if the property changed because of
	//	the user interface sliders. This property is a variation of the
	//	Aspect property, but is just for the UI.
	if (i_bDirty)
	{
		m_Data.m_Aspect.SetValue( powf(c_InvLn, m_LogAspect.GetValue()) );
	}
}

//--------------------------------------------------------------------
// This callback is for when the checkboxes related to the display
//	icon change. Update the icon object again.
//--------------------------------------------------------------------
void prjltProjectedLightObject::IconDisplayChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	this->update_3d_icon();
}

void prjltProjectedLightObject::RampEnabledChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	gpxRenderControl::ConfirmSingleThread();

	if(m_Data.m_bEnabledRamp.GetValue())
	{
		/*rmpData data;
		data.m_Gradient = m_Data.m_RampGradient;
		data.m_Interpolation = m_Data.m_RampInterpolation;
		data.m_Shape = m_Data.m_RampShape;
		data.m_TexSize = m_Data.m_RampTexSize;
		data.m_UWave = m_Data.m_RampUWave;
		data.m_VWave = m_Data.m_RampVWave;
		data.m_Noise = m_Data.m_RampNoise;
		data.m_NoiseFreq = m_Data.m_RampNoiseFreq;*/

		ConfirmRampTexture(m_Data.m_RampData.m_TexSize.GetValue());
		if (m_pRampTexture)
			m_pProjectedLight->SetTexture(m_pRampTexture);
		rmpTextureMgr::EnableRampTexture(m_Data.m_RampData, m_pRampTexture);
	}
	else
	{
		m_pProjectedLight->SetTexture(m_pTexture);
	}

	this->update_3d_icon();
	
	if (i_bDirty)
	{
		prjltDocumentChunk::ActiveDataChanged();
	}
}

void prjltProjectedLightObject::RampTriggerChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	//NotifyRampUI();
	
	rmpDialogUtil::Show();
}

//--------------------------------------------------------------------
// Callback for ramp color change
//--------------------------------------------------------------------
void prjltProjectedLightObject::RampChanged(bool i_bDirty)
{
	if (m_Data.m_bEnabledRamp.GetValue())
	{
		gpxRenderControl::ConfirmSingleThread();

		ConfirmRampTexture(m_Data.m_RampData.m_TexSize.GetValue());
		rmpTextureMgr::EnableRampTexture(m_Data.m_RampData, m_pRampTexture);
		if (m_pRampTexture)
			m_pProjectedLight->SetTexture(m_pRampTexture);

		this->update_3d_icon();

		if (i_bDirty)
		{
			prjltDocumentChunk::ActiveDataChanged();
		}
	}
}

void prjltProjectedLightObject::RampChangedFromUI(bool i_bDirty)
{
	if (m_Data.m_bEnabledRamp.GetValue())
	{
		m_Data.m_RampData = rmpDialogMgr::Data();
		UpdateRampData(m_Data.m_TextureFilename, m_Data.m_RampData);
		RampChanged(i_bDirty);
	}
}

void prjltProjectedLightObject::RampChangedFromData(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (m_Data.m_bEnabledRamp.GetValue())
	{
		UpdateRampData(m_Data.m_TextureFilename, m_Data.m_RampData);
		RampChanged(i_bDirty);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prjltProjectedLightObject::NotifyRampUI()
{
	rmpDialogMgr::SetRampChangedCallback(NULL);
	rmpDialogMgr::SetData(m_Data.m_RampData);
	rmpDialogMgr::SetRampChangedCallback(std::bind(std::mem_fn(&prjltProjectedLightObject::RampChangedFromUI), this, std::placeholders::_1));
}

//----------------------------------------------------------------------------
// Update the ramp object stored in the texture control with the new value.
//----------------------------------------------------------------------------
void prjltProjectedLightObject::UpdateRampData(prtyTextureFileName& io_Texture, const rmpData& i_RampData)
{
	prtyTextureFileData val = io_Texture.GetFullValue();
	shared_ptr<rmpObject> newRamp(new rmpObject);
	newRamp.get()->m_Data = i_RampData;
	val.m_RampObject = (shared_ptr<prtyObject>)newRamp;
	io_Texture.SetValueWithoutNotify(val);
}

//----------------------------------------------------------------------------
// return the ramp data of the texture control
//----------------------------------------------------------------------------
rmpData prjltProjectedLightObject::GetRampData(prtyTextureFileName& io_Texture, const rmpData& i_RampData)
{
	rmpData data = i_RampData;
	rmpObject* pRampObject = dynamic_cast<rmpObject*>(io_Texture.GetFullValue().m_RampObject.get());
	if(pRampObject)
	{
		data = pRampObject->m_Data;
	}
	return data;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prjltProjectedLightObject::ConfirmRampTexture(int i_texSize)
{
	gpxRenderControl::ConfirmSingleThread();

	// check if texture size change
	if (!m_pRampTexture || m_RampTextureSize != i_texSize)
	{
		m_RampTextureSize = i_texSize;

		if (m_pProjectedLight->GetTexture() == m_pRampTexture)
			m_pProjectedLight->SetTexture(NULL);

		rmpTextureMgr::ReleaseRampTexture(m_pRampTexture);

		try
		{
			m_pRampTexture = rmpTextureMgr::CreateRampTexture(i_texSize);
		}
		catch (const g2dOutOfVideoMemoryX& )
		{
			guiMessageBox::Show("Not enough video mem to create ramp texture. Disable ramp texture.", "Error", guiMessageBox::e_OKOnly);
			m_pRampTexture = NULL;
			m_Data.m_bEnabledRamp.SetValue(false);
		}
	}
}


//--------------------------------------------------------------------
// Callback for ramp color change
//--------------------------------------------------------------------
void prjltProjectedLightObject::ShaftRampChanged(bool i_bDirty)
{
	if (m_Data.m_bEnabledShaftRamp.GetValue())
	{
		gpxRenderControl::ConfirmSingleThread();

		ConfirmShaftRampTexture(m_Data.m_ShaftRampData.m_TexSize.GetValue());
		rmpTextureMgr::EnableRampTexture(m_Data.m_ShaftRampData, m_pShaftRampTexture);

		if (m_pShaftRampTexture)
			m_pShaftTexture = m_pShaftRampTexture;

		if (i_bDirty)
		{
			prjltDocumentChunk::ActiveDataChanged();
		}
	}
}

void prjltProjectedLightObject::ShaftRampChangedFromUI(bool i_bDirty)
{
	if (m_Data.m_bEnabledShaftRamp.GetValue())
	{
		m_Data.m_ShaftRampData = rmpDialogMgr::Data();
		UpdateRampData(m_Data.m_ShaftTextureFilename, m_Data.m_ShaftRampData);
		ShaftRampChanged(i_bDirty);
	}
}

void prjltProjectedLightObject::ShaftRampChangedFromData(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (m_Data.m_bEnabledShaftRamp.GetValue())
	{
		UpdateRampData(m_Data.m_ShaftTextureFilename, m_Data.m_ShaftRampData);
		ShaftRampChanged(i_bDirty);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prjltProjectedLightObject::NotifyShaftRampUI()
{
	rmpDialogMgr::SetRampChangedCallback(NULL);
	rmpDialogMgr::SetData(m_Data.m_ShaftRampData);
	rmpDialogMgr::SetRampChangedCallback(std::bind(std::mem_fn(&prjltProjectedLightObject::ShaftRampChangedFromUI), this, std::placeholders::_1));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prjltProjectedLightObject::ConfirmShaftRampTexture(int i_texSize)
{
	gpxRenderControl::ConfirmSingleThread();

	// check if texture size change
	if (!m_pShaftRampTexture || m_ShaftRampTextureSize != i_texSize)
	{
		m_ShaftRampTextureSize = i_texSize;

		if (m_pShaftTexture == m_pShaftRampTexture)
			m_pShaftTexture = NULL;

		rmpTextureMgr::ReleaseRampTexture(m_pShaftRampTexture);

		try
		{
			m_pShaftRampTexture = rmpTextureMgr::CreateRampTexture(i_texSize);
		}
		catch (const g2dOutOfVideoMemoryX& )
		{
			guiMessageBox::Show("Not enough video mem to create ramp texture. Disable ramp texture.", "Error", guiMessageBox::e_OKOnly);
			m_pShaftRampTexture = NULL;
			m_Data.m_bEnabledShaftRamp.SetValue(false);
		}
	}
}

//--------------------------------------------------------------------
// update 3d icon with projected light values
//--------------------------------------------------------------------
void prjltProjectedLightObject::update_3d_icon()
{
	if (!m_pObject) return;

	// If we are not showing the frustum, then shorten the range to 
	// a shorter value.
	const float c_ShortFrustrum = 0.1f;
	float range = (m_ShowFrustrum.GetValue()) ? 
		(m_ParentScale * m_Data.m_Range.GetValue()) : c_ShortFrustrum;

	matTexture* pTexture = (m_ShowTexture.GetValue() && m_pTexture) ? m_pTexture : NULL;
	pTexture = (m_Data.m_bEnabledRamp.GetValue() && m_ShowTexture.GetValue() && m_pRampTexture) ? m_pRampTexture : pTexture;

	// update 3d icon
	maPoint3d position, target;
	get_world_positions(position, target);
	m_pObject->Update( position, target, m_Data.m_Orientation.GetValue(),
		m_ParentScale * m_Data.m_Scale.GetValue(),
		m_Data.m_Tilt.GetValue(), m_Data.m_bDirectional.GetValue(), 
		m_pLightProxy->GetAngle(), m_pLightProxy->GetInnerAngle(), 
		m_Data.m_Aspect.GetValue(), range, pTexture, m_TexturePosition.GetValue());
}

//--------------------------------------------------------------------
// compute world position of light using parent transformations
//--------------------------------------------------------------------
void prjltProjectedLightObject::get_world_positions(maPoint3d &o_Position, maPoint3d &o_Target) const
{
	// New method incorporates transformation matrix into property and lets
	// the property compute and cache both world and object space
	o_Position = m_Data.m_Position.GetWorldSpaceValue();
	o_Target = m_Data.m_Target.GetWorldSpaceValue();

	// Old method was to transform to word space here
	//o_Position = m_Data.m_Position.GetValue();
	//o_Target = m_Data.m_Target.GetValue();

	//// transform to world space
	//maMatrix4x4 parent_matrix;
	//this->GetParentMatrix(parent_matrix);
	//parent_matrix.Transform(o_Position);
	//parent_matrix.Transform(o_Target);
}

//--------------------------------------------------------------------
// common code when a property related to position of light is changed.
// If i_bCheckForDifference==true, only submit changes to proxy if the
// target or position changes.
//--------------------------------------------------------------------
void prjltProjectedLightObject::transformation_changed(bool i_bCheckForDifference)
{
	// World space
	maPoint3d pos, target;
	get_world_positions(pos, target);

	// Object space (for pitch, yaw below)
	maPoint3d obj_target = m_Data.m_Target.GetValue();

	// pass values to light based on light type
	// (some are adjusted by parent scale)
	if (m_Data.m_LightType == prjltData::e_SpotLight)
	{
		float near_dist =  m_Data.m_Scale.GetValue();
		maVector3d view_dir = target - pos;
		view_dir.Normalize();

		// spot lights have position at cone originm but g3dProjectedLights
		// have position on near plane, so have to adjust.
		maPoint3d new_pos = pos + view_dir*near_dist;

		// Check for meaningful difference before updating the proxy values.
		// This allows the render thread to shut down if there are no differences.
		if ( i_bCheckForDifference &&
			((m_pLightProxy->GetPosition() - new_pos).LengthSqr() < maConstants::c_fEpsilon) &&
			((m_pLightProxy->GetTarget() - target).LengthSqr() < maConstants::c_fEpsilon))
		{
			return; // make no changes
		}

		m_pLightProxy->SetPosition( new_pos );
		m_pLightProxy->SetTarget( target );
		m_pLightProxy->SetRange(m_ParentScale * m_Data.m_Range.GetValue() - near_dist);
		m_pLightProxy->SetScale(near_dist);

		// spot lights are never directional
		m_pLightProxy->SetIsDirectional(false);

		// setting angle and inner angle based on m_Angle and m_Penumbra
		// to create a light cone with falloff
		float total_angle = m_Data.m_Angle.GetValue() + m_Data.m_Penumbra.GetValue();
		maFunctions::Clamp(total_angle, 0.0f, 179.9f);
		m_pLightProxy->SetAngle(total_angle);
		m_pLightProxy->SetInnerAngle(m_Data.m_Angle.GetValue());

		// Aspect & Tilt are fixed for circular spot lights
	}
	else if (m_Data.m_LightType == prjltData::e_DirectionalLight)
	{
		// Directional lights use orientation property, 
		// but pass direction to light by using the position and target fields.
		// api3dScene will resize the directional light so that it 
		// lights the whole scene later
		maVector3d new_view(0,0,1.0f);
		m_Data.m_Orientation.GetValue().RotateVector( new_view );
		maPoint3d new_pos = m_Data.m_Position.GetValue();
		obj_target = new_pos + new_view; // for pitch and yaw below

		// Check for meaningful difference before updating the proxy values.
		// This allows the render thread to shut down if there are no differences.
		if ( i_bCheckForDifference &&
			((m_pLightProxy->GetPosition() - new_pos).LengthSqr() < maConstants::c_fEpsilon) &&
			((m_pLightProxy->GetTarget() - obj_target).LengthSqr() < maConstants::c_fEpsilon))
		{
			return; // make no changes
		}

		// Force directional lighting
		m_pLightProxy->SetIsDirectional(true);

		m_pLightProxy->SetPosition( new_pos );
		m_pLightProxy->SetTarget( obj_target );

		// Range and scale will be automatically resized to fit whole scene
		//m_pLightProxy->SetRange(m_ParentScale * m_Data.m_Range.GetValue());
		//m_pLightProxy->SetScale(m_Data.m_Scale.GetValue());

		// Aspect & Tilt are fixed for directional lights
	}
	else
	{
		// Projected light type...

		// Check for meaningful difference before updating the proxy values.
		// This allows the render thread to shut down if there are no differences.
		if ( i_bCheckForDifference &&
			((m_pLightProxy->GetPosition() - pos).LengthSqr() < maConstants::c_fEpsilon) &&
			((m_pLightProxy->GetTarget() - target).LengthSqr() < maConstants::c_fEpsilon))
		{
			return; // make no changes
		}

		m_pLightProxy->SetPosition( pos );
		m_pLightProxy->SetTarget( target );
		m_pLightProxy->SetRange(m_ParentScale * m_Data.m_Range.GetValue());
		m_pLightProxy->SetIsDirectional(m_Data.m_bDirectional.GetValue());
		if (m_Data.m_bConeLighting.GetValue())
		{
			// Cone lighting has angle as iner "hot spot" lighting angle
			m_pLightProxy->SetInnerAngle(m_Data.m_Angle.GetValue());
			float total_angle = m_Data.m_Angle.GetValue() + m_Data.m_Penumbra.GetValue();
			maFunctions::Clamp(total_angle, 0.0f, 179.9f);
			m_pLightProxy->SetAngle(total_angle);
		}
		else
		{
			// Otherwise, inner angle is set to maximum and the full 
			// rectangular light frustrum is used.
			m_pLightProxy->SetAngle(m_Data.m_Angle.GetValue());
			m_pLightProxy->SetInnerAngle(180.0f);
		}

		// The light scale can't be allowed to reach zero
		float light_scale = m_ParentScale * m_Data.m_Scale.GetValue();
		if (light_scale < c_MinimumScale) light_scale = c_MinimumScale;
		m_pLightProxy->SetScale(light_scale);

		m_pLightProxy->SetAspect(m_Data.m_Aspect.GetValue());
		m_pLightProxy->SetTilt(m_Data.m_Tilt.GetValue());
	}

	// Properties common to all light types
	m_pLightProxy->SetDepthBias(m_Data.m_DepthBias.GetValue() * 0.02f);

	// Let light update our camera for generating the depth map.
	// Use the proxy here to buffer the changes.
	m_pLightProxy->OrientCamera(*m_ShadowCameraProxy);

	// update 3d icons
	this->update_3d_icon();
	this->UpdateFalloffIcon();

	// Set the value of the pitch and yaw properties which are derived
	// from position and target
	//maVector3d view_dir = target - pos;	// this would be world space pitch & yaw
	maVector3d view_dir = obj_target - m_Data.m_Position.GetValue(); // this is object space pitch & yaw
	float pitch = 0, yaw = 0, distance = 0;
	convert_pitch_yaw(view_dir, pitch, yaw, distance);
	m_Pitch.SetValue(maConstants::c_fRadToAngle * pitch);
	m_Yaw.SetValue(maConstants::c_fRadToAngle * yaw);

	// Set our "LogAspect" user interface property
	m_LogAspect.SetValue( logf( m_Data.m_Aspect.GetValue() ) );

	// update position of light shaft
	// Is UpdateLightShaft() enough here, or do we need ConfirmLightShaft()?
	ConfirmLightShaft();
	UpdateLightShaft();
}

//--------------------------------------------------------------------
// Called from transformation manager once per frame, update light
//	position based on parent transformation matrix.
//--------------------------------------------------------------------
void prjltProjectedLightObject::UpdateParentTransform()
{
	// This function is called once per frame in order to avoid
	// multiple updates everytime any property of any parent transformation
	// node changes. Update light and icons based on parent transformation.

	// Give the properties the transformation matrix and let them compute
	// the object and world space as needed.
	maMatrix4x4 parent_matrix;
	this->GetParentMatrix(parent_matrix);
	m_Data.m_Position.SetTransformation(parent_matrix);
	m_Data.m_Target.SetTransformation(parent_matrix);

	// Extract out scale information (is there a better way to do this?)
	// in order to apply it to the range and scale properties of the light
	const float c_SqrtThree = 1.0f / ::sqrtf(3);
	maVector3d scale_vec(c_SqrtThree, c_SqrtThree, c_SqrtThree); // unit length
	parent_matrix.TransformDir(scale_vec);
	float new_parent_scale = scale_vec.Length();
	bool bChangedParentScale = (new_parent_scale != m_ParentScale);
	m_ParentScale = new_parent_scale;

	// Check for meaningful difference before updating the proxy values.
	// This allows the render thread to shut down if there are no differences.
	bool bCheckForDifferences = (!bChangedParentScale);
	this->transformation_changed(bCheckForDifferences);
	
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
void prjltProjectedLightObject::ApplyTransformation(const maMatrix4x4& i_Matrix)
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
bool prjltProjectedLightObject::HasIconPivotPoint(maPoint3d& o_Pivot) 
{ 
	maPoint3d position, target;
	get_world_positions(position, target);
	o_Pivot = position;
	return true; 
}

//--------------------------------------------------------------------
// Update icon that displays falloff range
//--------------------------------------------------------------------
void prjltProjectedLightObject::UpdateFalloffIcon()
{
	m_pRangeIcon->SetRenderable(this->ShouldShowFalloffIcon());

	maPoint3d position, target;
	get_world_positions(position, target);
	m_pRangeIcon->Update(position,
						 target - position,
						 m_FalloffRange.GetValue(), 
						 m_FalloffPercent.GetValue());
}

//--------------------------------------------------------------------
//	IsolationChanged - notification that this object's isolation
//	state has changed (fading on or out the light's intensity)
//--------------------------------------------------------------------
void prjltProjectedLightObject::IsolationChanged()
{
	// Combine intensity with the isolation alpha for fades in and out
	m_pLightProxy->SetIntensityFactor(m_Data.m_Intensity.GetValue() * this->GetIsolationAlpha());

	// Combine the isoalted and enabled states to determine if the light is "on"
	m_pLightProxy->SetEnable( m_Data.m_Enabled.GetValue() && this->GetIsolationVisible() );

	// Redo the renderable calculation that uses GetIsolationVisible()
	// by just calling it with the current value
	this->SetRenderable(m_bRenderable);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prjltProjectedLightObject::ReportMemory(gfFileTxt& i_File)
{
    std::ostringstream stm;

	if (m_pTexture)
	{
		stm << "  Texture  : " << m_pTexture->GetSize() << "KB\r\n";
	}

	if (m_pShadowMap)
	{
		stm << "  ShadowMap: " << m_pShadowMap->GetSize() << "KB\r\n";
	}

	if (m_pShaftTexture)
	{
		stm << "  Shaft Tex: " << m_pShaftTexture->GetSize() << "KB\r\n";
	}

	i_File.WriteLine(stm.str());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
matTexture* prjltProjectedLightObject::GetRampTexture()
{
	return m_pRampTexture;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
matTexture* prjltProjectedLightObject::GetTexture()
{
	return m_pTexture;
}

void prjltProjectedLightObject::HairChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	float ZNear = m_Data.m_Scale.GetValue();
	float ZFar = ZNear + m_Data.m_Range.GetValue();

	float Min = m_Data.m_HairMinBound.GetValue();
	float Max = m_Data.m_HairMaxBound.GetValue();

	if( Min < ZNear )
	{
		m_Data.m_HairMinBound.SetValueWithoutNotify( ZNear );
		Min = ZNear;
	}
	if( Max > ZFar )
	{
		m_Data.m_HairMaxBound.SetValueWithoutNotify( ZFar );
		Max = ZFar;
	}

	m_pLightProxy->SetHairMinBound( Min );
	m_pLightProxy->SetHairMaxBound( Max );

	if (i_bDirty)
		prjltDocumentChunk::ActiveDataChanged();
}

