/*****************************************************************************
**  prjltProjectedLightObject.cpp
**
**      A prjltProjectedLightObject is a derived class for displaying a projected
**	light's position.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/PrjLt/Object/prjltProjectedLightObject.hpp"

#include "Systems/PrjLt/GUI/prjltDialogDataUtil.hpp"
#include "Systems/PrjLt/Data/prjltDocumentChunk.hpp"
#include "Systems/PrjLt/Object/prjltIconObject.hpp"
#include "Systems/PrjLt/Undo/prjltOperations.hpp"
#include "Systems/PrjLt/Object/prjltRangeIcon.hpp"
#include "Systems/PrjLt/GUI/prjltTextureList.hpp"

#include "Features/LoadPrefs/LoadPrefsMgr.hpp"
#include "Support/ltst/ltstLightSetMgr.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mnm/mnmReferencePosChannel.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"

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
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"
#include "Tool/api3d/api3dLightMgr.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Tool/api3d/api3dTargetRendererMgr.hpp"
#include "Graphics/eff/effLightGlowData.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#include "Graphics/g3d/g3dTargetRenderer.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/sc/scObject.hpp"
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"

#include <assert.h>
#include <sstream>

//============================================================================
//============================================================================
namespace
{
	const float l_SphereRadius = 0.4f;
	const float l_CircleRadius = 0.25f;
	const float l_BoxRadius = 2 * l_SphereRadius;
	const float l_PickRadius = 1.0f;	// pick larger than icon
	const float c_MinScale = 0.1f;
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
prjltProjectedLightObject::prjltProjectedLightObject()
:	m_WorldBox(l_SphereBox), 
	m_pTexture(NULL), 
	m_pShadowMap(NULL), 
	m_pDepthRenderer(NULL), 
	m_pMapRenderer(NULL),
	m_pLightShaft(NULL),
	m_pLightShaftMat(NULL),
	m_pShaftTexture(NULL),
    m_bRenderable(true),
	m_bLayerVisible(true),
	m_Pitch("Pitch", 0.0f),
	m_Yaw("Yaw", 0.0f),
	m_ManipMode("Manip Mode", e_Light),
	m_LogAspect("Aspect (log)", 0.0f),
	m_ShowFrustrum("Show Frustrum", false),
	m_ShowTexture("Show Texture", false)
{
	// Set up property ranges
	m_Pitch.SetMaximum(90.0f);
	m_Pitch.SetMinimum(-90.0f);
	m_Yaw.SetMaximum(180.0f);
	m_Yaw.SetMinimum(-180.0f);

	// Set up enumeration for manip mode
	m_ManipMode.SetEnumTag(e_Light,"Light");
	m_ManipMode.SetEnumTag(e_Target,"Target");
	m_ManipMode.SetEnumTag(e_Together,"Together");

	// Create g3d light
	m_pLight = api3dLightMgr::CreateProjectedLight();
	m_pLight->SetCastsShadow(true);

	// simple renderer for shadow maps (put into api3d somehow?)
	m_pDepthRenderer = g3dSceneRendererCreate::CreateDepthMapRenderer();

	// Create 3D icon
	m_pObject = new prjltIconObject();
	//m_pObject = api3dShape::CreateSphere(maFloatRGBA(1,1,1,1), l_SphereRadius, 8, 8);
	//m_pObject->SetPosition(i_Data.m_Position);

	// falloff icon
	m_pRangeIcon = new prjltRangeIcon();

	ltstLightSetMgr::AddLight(this, m_pLight);

	api3dScale::RegisterScaleInterest(this);

	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyTextBoxUIInfo(&(m_Data.m_Name), "Asset", "Name of the object");
	AddProperty( pPUII );
	prtyFileChooserUIInfo* pFCI;
	pFCI = new prtyFileChooserUIInfo(&(m_Data.m_TextureFilename), "Asset", "File Name of the texture");
	fsLocator dir;
	dir.Push( gfPaths::GetPath( mnmPaths::e_DataStock ) );
	dir.Push("Effects");
	dir.Push("General");
	dir.Push("Textures");
	pFCI->SetInitialDirectory(dir);
	AddProperty( pFCI );
	prtyComboBoxUIInfo* pCBUII;
	pCBUII  = new prtyComboBoxUIInfo(&(m_Data.m_DepthMapSize), "Shadow Quality", "Depth Map Size");
	pCBUII->AddItem(std::string("256"), 0);
	pCBUII->AddItem(std::string("512"), 1);
	pCBUII->AddItem(std::string("1024"), 2);
	pCBUII->AddItem(std::string("2048"), 3);
	pCBUII->AddItem(std::string("4096"), 4);
	pCBUII->AddItem(std::string("8192"), 5);
	//pCBUII->SetReadOnly(true);
	AddProperty( pCBUII );
	pPUII  = new prtyColorRGBAEditUIInfo(&(m_Data.m_Color), "Light", "Color of the object");
	AddProperty( pPUII );
	pPUII = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_Position), "Transform", "Position of the object");
	AddProperty( pPUII );
	// There shouldn't be an orientation field in projected light
	//pPUII  = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_Orientation), "category2", "Orientation of the object");
	//AddProperty( pPUII );
	pPUII  = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_Target), "Transform", "Target of the object");
	AddProperty( pPUII );
	prtyRangedFloatUIInfo* pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Angle), "Light", "Angle of the light´s view");
	pRFUII->SetMinimum(0.0f); //1.0f);
	pRFUII->SetMaximum(179.0f);
	pRFUII->SetDecimalPlaces(2);
	pRFUII->SetNumTicks(100);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Scale), "Transform", "Scale of the light");
	pRFUII->SetMinimum(0.0f); //0.1f);
	pRFUII->SetMaximum(api3dScale::Scale(1000.0f));
	pRFUII->SetDecimalPlaces(0);
	pRFUII->SetNumTicks(100);
	pRFUII->SetExponent(3);
	AddProperty( pRFUII );
	//pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Aspect), "Light", "Aspect of the light´s view region");
	pRFUII  = new prtyRangedFloatUIInfo(&(m_LogAspect), "Light", "Aspect (log) of the light´s view region");
	pRFUII->SetMinimum(-4.0f);
	pRFUII->SetMaximum(4.0f);
	pRFUII->SetDecimalPlaces(2);
	pRFUII->SetNumTicks(100);
	AddProperty( pRFUII );
	pPUII = new prtyComboBoxUIInfo(&(m_ManipMode), "Light Angles", "Manipulations center on light or target");
	AddProperty( pPUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Pitch), "Light Angles", "Pitch angle of light direction");
	pRFUII->SetMinimum(-89.9f);
	pRFUII->SetMaximum(89.9f);
	pRFUII->SetDecimalPlaces(1);
	pRFUII->SetNumTicks(180);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Yaw), "Light Angles", "Yaw angle of light direction");
	pRFUII->SetMinimum(-180.0f);
	pRFUII->SetMaximum(180.0f);
	pRFUII->SetDecimalPlaces(1);
	pRFUII->SetNumTicks(90);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Tilt), "Light Angles", "Tilt angle around light direction");
	pRFUII->SetMinimum(-180.0f);
	pRFUII->SetMaximum(180.0f);
	pRFUII->SetDecimalPlaces(1);
	pRFUII->SetNumTicks(90);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Range), "Light", "Range of the object");
	pRFUII->SetMinimum(0.0f); //0.1f);
	pRFUII->SetMaximum(api3dScale::Scale(3000.0f));
	pRFUII->SetDecimalPlaces(0);
	pRFUII->SetNumTicks(100);
	pRFUII->SetExponent(3);
	AddProperty( pRFUII );
	// Editor visible is set from other part of GUI
	//AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bEditorVisible), "Editor", "Is the object visible in the editor") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_Enabled), "Asset", "Enabled object") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_ShadowSource), "Shadow Quality", "Is this a Shadow Source") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_ShadowQuality), "Shadow Quality", "Shadow quality level") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bDiffuseEnabled), "Light", "Enable Diffuse") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bSpecularEnabled), "Light", "Enable Specular") );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_DepthBias), "Shadow Quality", "Depth Bias for shadow map");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(0.1f);
	pRFUII->SetDecimalPlaces(5);
	pRFUII->SetNumTicks(100);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_LightSize), "Shadow Quality", "Light Size of the object");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(api3dScale::Scale(0.010f));
	pRFUII->SetDecimalPlaces(5);
	pRFUII->SetNumTicks(50);
	AddProperty( pRFUII );
	pPUII  = new prtyFloatEditUIInfo(&(m_Data.m_SceneScale), "Shadow Quality", "Scene Scale of the object");
	AddProperty( pPUII );
	pPUII  = new prtyColorRGBAEditUIInfo(&(m_Data.m_ShadowColor), "Shadow Quality", "Color of the shadows");
	AddProperty( pPUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_ShadowIntensity), "Shadow Quality", "How dark is the shadow");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	AddProperty( pRFUII );
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
	pFCI = new prtyFileChooserUIInfo(&(m_Data.m_ShaftTextureFilename), "Light Shaft", "File Name of the texture");
	fsLocator shaftDir;
	shaftDir.Push( gfPaths::GetPath( mnmPaths::e_DataStock ) );
	shaftDir.Push("Effects");
	shaftDir.Push("General");
	shaftDir.Push("Textures");
	pFCI->SetInitialDirectory(shaftDir);
	AddProperty( pFCI );

	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bAffectsFur), "Light", "Does the light affect fur") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bAffectsGlow), "Light", "Does the light affect glow") );

	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Intensity), "Light", "Brightness multiplier (>1 for HDR only)");
	pRFUII->SetMinimum(1.0f);
	pRFUII->SetMaximum(500.0f);
	pRFUII->SetNumTicks(1000);
	AddProperty( pRFUII );

	// Checkboxes for control of icon display
	AddProperty( new prtyCheckBoxUIInfo(&(m_ShowFrustrum), "Icon Display", "Display frustrum of light") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_ShowTexture), "Icon Display", "Show texture in icon") );

	// Falloff group
	cmmFalloffAspect::RegisterProperties( this, &(m_Data.m_Falloff));

	// Register callbacks to update particle generator and icons
	// when properties change
	m_Data.m_Name.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::NameChanged));
	m_Data.m_TextureFilename.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::TextureChanged));
	m_Data.m_DepthMapSize.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::DepthMapChanged));
	m_Data.m_Color.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::ColorChanged));
	m_Data.m_Intensity.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::ColorChanged));
	m_Data.m_Position.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::TransformChanged));
	m_Data.m_Scale.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::TransformChanged));
	m_Data.m_Target.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::TransformChanged));
	m_Data.m_Angle.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::TransformChanged));
	m_Data.m_Aspect.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::TransformChanged));
	m_Data.m_Tilt.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::TransformChanged));

	m_Data.m_Enabled.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::EnabledChanged));
	m_Data.m_ShadowSource.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::ShadowSourceChanged));
	m_Data.m_Falloff.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightChanged));
	m_Data.m_Range.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::TransformChanged));
	m_Data.m_ShadowQuality.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightChanged));
	m_Data.m_bDiffuseEnabled.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightChanged));
	m_Data.m_bSpecularEnabled.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightChanged));
	m_Data.m_LightSize.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightChanged));
	m_Data.m_SceneScale.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightChanged));
	m_Data.m_ShadowIntensity.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightChanged));
	m_Data.m_ShadowColor.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightChanged));
	m_Data.m_DepthBias.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::TransformChanged));
	m_Data.m_bAffectsFur.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightChanged));
	m_Data.m_bAffectsGlow.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightChanged));

	m_Data.m_bShaftVisible.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightShaftChanged));
	m_Data.m_ShaftAlpha.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightShaftChanged));
	m_Data.m_ShaftDensity.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightShaftChanged));
	m_Data.m_ShaftDistFalloffStart.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightShaftChanged));
	m_Data.m_ShaftDistFalloffEnd.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightShaftChanged));
	m_Data.m_ShaftTextureFilename.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LightShaftTextureChanged));

	// Properties just for the interface
	m_Yaw.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::PitchYawChanged));
	m_Pitch.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::PitchYawChanged));
	m_ManipMode.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::ManipModeChanged));
	m_LogAspect.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::LogAspectChanged));
	m_ShowFrustrum.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::IconDisplayChanged));
	m_ShowTexture.AddCallback(new prtyCallbackWrapper<prjltProjectedLightObject>(this, &prjltProjectedLightObject::IconDisplayChanged));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prjltProjectedLightObject::~prjltProjectedLightObject()
{
	ltstLightSetMgr::RemoveLight(this, m_pLight);

	api3dScale::UnRegisterScaleInterest(this);

	delete m_pRangeIcon;

	delete m_pObject;
	api3dLightMgr::DestroyLight(m_pLight);

	//	release textures
	if (m_pTexture)
		matTextureMgr::ReleaseTexture(m_pTexture);
	if (m_pShadowMap)
		matTextureMgr::ReleaseTexture(m_pShadowMap);
	if (m_pMapRenderer)
	{
		api3dTargetRendererMgr::RemoveTargetRenderer(m_pMapRenderer);
		delete m_pMapRenderer;
	}
	delete m_pDepthRenderer;

	if (m_pShaftTexture)
		matTextureMgr::ReleaseTexture(m_pShaftTexture);
	if (m_pLightShaft)
	{
		api3dScene::RemoveObject(m_pLightShaft);
		delete m_pLightShaft;
		//delete m_pLightShaftMat;
	}
}


//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string prjltProjectedLightObject::GetPick3dName() const
{
	return GetName().GetString();
}

//--------------------------------------------------------------------
//	GlobalScaleChanged - notification that global scale has changed.
//--------------------------------------------------------------------
//virtual 
void prjltProjectedLightObject::GlobalScaleChanged( float i_Scale )
{
	update_3d_icon();
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
	//m_pLight->SetCastsShadow(i_Data.m_ShadowSource);

	this->ConfirmDepthMap();
	this->ConfirmLightShaft();

	m_Data = i_Data;

	// Get initial value of falloff type and then let user change it after this
	cmmFalloffAspect::InitializeFalloffType();
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
// EnableFur property access
//--------------------------------------------------------------------
prtyBoolean&	prjltProjectedLightObject::PropertyEnableFur()
{
	return m_Data.m_bAffectsFur;
}
const prtyBoolean&	prjltProjectedLightObject::GetPropertyEnableFur() const
{
	return m_Data.m_bAffectsFur;
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
prtyFloat&	prjltProjectedLightObject::PropertyShaftFalloffEnd()
{
	return m_Data.m_ShaftDistFalloffEnd;
}
const prtyFloat&	prjltProjectedLightObject::GetPropertyShaftFalloffEnd() const
{
	return m_Data.m_ShaftDistFalloffEnd;
}

//----------------------------------------------------------------------------
//	GetWorldBox returns a box which completely encloses the prjltProjectedLightObject.
//----------------------------------------------------------------------------
const maAxisBox& prjltProjectedLightObject::GetWorldBox() const
{
	return m_WorldBox;
}

//----------------------------------------------------------------------------
//	GetLocalBox returns a box which would enclose the prjltProjectedLightObject
//	if its transformations were identity
//----------------------------------------------------------------------------
const maAxisBox& prjltProjectedLightObject::GetLocalBox() const
{
	return l_SphereBox;
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
	if (m_ManipMode.GetValue() == e_Target)
		return m_Data.m_Target.GetValue();
	else
		return m_Data.m_Position.GetValue();
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
	switch (m_ManipMode.GetValue())
	{
	case e_Light:
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
maRotation prjltProjectedLightObject::GetOrientation() const
{
	//return l_NoRot;

	return maRotation(-m_Pitch.GetValue() * maConstants::c_fAngleToRad, 
					  m_Yaw.GetValue() * maConstants::c_fAngleToRad, 
					  0);	// don't do tilt here
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
maPoint3d prjltProjectedLightObject::GetScale() const
{
	float scale = m_pLight->GetScale();
	return maPoint3d(scale, scale, scale);
	//return l_One;
}

void prjltProjectedLightObject::UpdateScale(const maPoint3d& i_Scale, 
										bool i_bNewOperation)
{
	m_Data.m_Scale.SetValue((i_Scale.m_X > c_MinScale) ? i_Scale.m_X : c_MinScale, 
		i_bNewOperation ? prtyProperty::eNewUndo : prtyProperty::eContinueUndo);
}


//----------------------------------------------------------------------------
//	RayPick returns true if the given ray intersects the prjltProjectedLightObject.
//	If it does, the t value is also returned in o_T.
//----------------------------------------------------------------------------
bool prjltProjectedLightObject::RayPick(const maPoint3d& i_RayStart,
										const maPoint3d& i_RayEnd,
										float& o_T)
{
	if (!m_pObject->GetRenderable())
		return false;

	maVector3d ray_dir = i_RayEnd - i_RayStart;

	// Test pick ray against light position and against target
	float light_t  = -1.0f;
	bool bPickLight = geoRayIntersection::IntersectLineSphere(	
								i_RayStart,
								ray_dir,
								m_Data.m_Position.GetValue(),
								api3dScale::Scale(l_PickRadius),
								light_t);
	float tgt_t  = -1.0f;
	bool bPickTarget = geoRayIntersection::IntersectLineSphere(	
								i_RayStart,
								ray_dir,
								m_Data.m_Target.GetValue(),
								api3dScale::Scale(l_PickRadius),
								tgt_t);

	// Also add in a circle pick for the circle icon on the projected
	// light's plane
	maVector3d plane_normal = m_Data.m_Target.GetValue() - m_Data.m_Position.GetValue();
	plane_normal.Normalize();
	float circle_t  = -1.0f;
	if (geoRayIntersection::ProjectLineToPlane(i_RayStart, 
								ray_dir, 
								m_Data.m_Position.GetValue(), 
								plane_normal, 
								circle_t))
	{
		// See if this pick was better than the simple
		// small sphere pick
		if (circle_t > 0.0f && 
			(!bPickLight || circle_t < light_t))
		{
			// If so, then see if the intersection point is within
			// the circle's radius
			maPoint3d int_pt = i_RayStart + ray_dir * circle_t;
			float dist_sqr = (m_Data.m_Position.GetValue() - int_pt).LengthSqr();
			float circ_rad = l_CircleRadius * m_Data.m_Scale.GetValue();
			if (dist_sqr < circ_rad*circ_rad)
			{
				// Alter the variables so that it looks like we picked
				// the light's sphere. This will set up the manip mode correctly
				bPickLight = true;
				light_t = circle_t;
			}
		}
	}

	// If both were picked, choose closest
	if (bPickLight && bPickTarget)
	{
		if (tgt_t < light_t)
			bPickLight = false;
		else
			bPickTarget = false;
	}

	// Set the manip mode based on where the click was.
	// Note: no way to choose "together" this way, 
	// maybe we should use midpoint of ray as another pick point?
	if (bPickLight)
	{
		this->m_ManipMode.SetValue(e_Light);
		o_T = light_t;
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
		&& m_bLayerVisible;
	m_pObject->SetRenderable( bRenderable );
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
	m_pObject->SetPickable(i_Pickable);
}

//----------------------------------------------------------------------------
// Flags for how object can be modified
//----------------------------------------------------------------------------
mnmObject::RotateFlags	prjltProjectedLightObject::GetRotateFlags()
{
	return mnmObject::e_RotateXY;
	//return mnmObject::e_RotateNone;
}
mnmObject::ScaleFlags	prjltProjectedLightObject::GetScaleFlags()
{
	return mnmObject::e_ScaleUniform;
}
mnmObject::TranslateFlags	prjltProjectedLightObject::GetTranslateFlags()
{
	return mnmObject::e_TranslateAll;
}


//----------------------------------------------------------------------------
// Get parent object of the spline in order for selection to trace from
//	this spline back to the controlled object.
//----------------------------------------------------------------------------
//virtual 
pick3dPickObject* prjltProjectedLightObject::GetParentObject() const
{
	return m_pParent;
}
//virtual 
void prjltProjectedLightObject::SetParentObject(pick3dPickObject* i_pParent)
{
	m_pParent = i_pParent;
}
		
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
		if (m_pTexture)
		{
			matTextureMgr::ReleaseTexture(m_pTexture);
			m_pTexture = NULL;
		}

		if (m_Data.m_TextureFilename.GetValue().GetLength() > 0)
		{

			//	get the file path
			//
			fsLocator tex_loc;
			fsysFileList file_list;
			prjltTextureList::BuildFileList(file_list);
			file_list.GetFilePath(m_Data.m_TextureFilename.GetValue(), tex_loc);
			tex_loc.Push(m_Data.m_TextureFilename.GetValue());

			// load texture to project (have to make sure this isn't a mip-map
			// texture, or else you get a line at the light's plane.)
			//
			// FIX - the texture filename coming in may be ".dds", but the
			//	texture loader will search for other extensions as well.  The
			//	result is that the filename stored in data fields (and the resource
			//	tracker) may have the wrong extension.  This is a problem when 
			//	trying to replace a file in the resource tracker. [rjk]
			//
			m_pTexture = matTextureMgr::LoadTexture(tex_loc, false);
			//m_pTexture = matTextureMgrDX9::LoadPlainTexture(tex_loc, 0,0);

			//	replace the filename in the resource tracker
			//
			fsResourceTracker::ReplaceFile(m_TextureNameLoaded, tex_loc);
		}

		//	set the name
		m_TextureNameLoaded = m_Data.m_TextureFilename.GetValue();

		// Assign texture to projected light
		//
		m_pLight->SetTexture(m_pTexture);
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

		if (LoadPrefsMgr::Data().m_bNoDepthMaps.GetValue() || !m_Data.m_Enabled.GetValue())
		{
			// Instead of dealing with NULL pointers, just set the
			// depth map to a very small size when they are turned off.
			new_depth_map_size = 4;
		}
		else if (LoadPrefsMgr::Data().m_DepthMapReduce.GetValue() > 0)
		{
			// Reduce size of depth maps
			new_depth_map_size >>= LoadPrefsMgr::Data().m_DepthMapReduce.GetValue();
			if (new_depth_map_size < 4)
				new_depth_map_size = 4;
		}

		// Create depth map for rendering shadows
		if (!m_pShadowMap || (new_depth_map_size != m_DepthMapSize))
		{
			if (m_pShadowMap)
				matTextureMgr::ReleaseTexture(m_pShadowMap);

			// make the render target texture
			try
			{
				m_pShadowMap = matTextureMgr::CreateRenderTargetTexture( new_depth_map_size, new_depth_map_size, true );
			}
			catch (const g2dOutOfVideoMemoryX& )
			{
				guiMessageBox::Show("Not enough video mem to create shadow map texture. Switching shadows off for this light.", "Error", guiMessageBox::e_OKOnly);
				m_pShadowMap = NULL;
				m_Data.m_ShadowSource.SetValue(false);
			}


			m_DepthMapSize = new_depth_map_size;

			// Assign textures to projected light
			m_pLight->SetShadowMap(m_pShadowMap);

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
		m_pLight->SetShadowMap(NULL);
	}
}
//--------------------------------------------------------------------
// Creates/destroys light shaft object depending on 
//	properties in the data structure
//--------------------------------------------------------------------
void prjltProjectedLightObject::ConfirmLightShaft()
{
	if (m_Data.m_bShaftVisible.GetValue())
	{
		// Do we always need to delete the old light shaft? 
		if (m_pLightShaft)
		{
			// clean up the old one
			api3dScene::RemoveObject(m_pLightShaft);
			delete m_pLightShaft;
			m_pLightShaft = NULL;
			//delete m_pLightShaftMat; // material is deleted with api3dobject
		}

		if (m_Data.m_Enabled.GetValue())
		{
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

			m_pLightShaftMat = m_pLightShaft->Material();
			m_pLightShaftMat->ForceTransparency(true);
			m_pLightShaftMat->SetAdditive(true);

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
			// clean up the old one
			api3dScene::RemoveObject(m_pLightShaft);
			delete m_pLightShaft;
			m_pLightShaft = NULL;
			//delete m_pLightShaftMat; // material is deleted with api3dobject
		}
	}
}
//--------------------------------------------------------------------
// Updates position of light shaft
//--------------------------------------------------------------------
void prjltProjectedLightObject::UpdateLightShaft()
{
	if ((!m_pLightShaft) || (!m_Data.m_bShaftVisible.GetValue()))
		return;

	// cone's origin is at its base, which is the target pos of our light
	m_pLightShaft->SetPosition(m_pLight->GetPosition() + m_pLight->GetDirection()*m_Data.m_Range.GetValue());
	maRotation rot;
	maVector3d rotvec = (m_pLight->GetPosition()-m_pLight->GetTarget());
	rotvec.Normalize();
	rot.SetValue(maVector3d(0,1,0), rotvec);
	m_pLightShaft->SetOrientation(rot);

//	matShaderEffect* eff = m_pLightShaftMat->GetShaderEffect();
//	eff->SetTechnique(matShaderEffect::e_Default);

	effLightGlowData* pData = dynamic_cast<effLightGlowData*>(m_pLightShaftMat->GetEffectData());
	DBG_ASSERT0(pData != NULL, "prjlt Light Shaft not using effLightGlow");
	// set up texture in different func
	pData->m_TextureDiffuse = m_pShaftTexture;
	pData->m_EdgeFuzzCutoff = m_Data.m_ShaftDensity.GetValue();
	pData->m_DistFalloffStart = m_Data.m_ShaftDistFalloffStart.GetValue() + m_Data.m_Scale.GetValue();
	pData->m_DistFalloffEnd = m_Data.m_ShaftDistFalloffEnd.GetValue() + m_Data.m_Scale.GetValue();
	pData->m_GlowAlpha = m_Data.m_ShaftAlpha.GetValue();
	pData->m_pShadowMap = m_pShadowMap;

	maVector3d pos = m_pLight->GetPosition();
	maVector3d dir = m_pLight->GetDirection();

	pData->proj_light_info.m_Position.Set(pos.m_X, pos.m_Y, pos.m_Z, 1.0f);
	pData->proj_light_info.m_TextureMatrix = m_pLight->GetTotalMatrix();
	pData->proj_light_info.m_TextureMatrix.Transpose(); // needed?
	pData->proj_light_info.m_LightSize = m_pLight->GetLightSize();
	pData->proj_light_info.m_SceneScale = m_pLight->GetSceneScale();
	pData->proj_light_info.m_ShadowIntensity = m_pLight->GetShadowIntensity();
	pData->proj_light_info.m_ShadowColor = m_pLight->GetShadowColor();
	pData->light_info.m_Position.Set(pos.m_X, pos.m_Y, pos.m_Z, 1.0f);
	pData->light_info.m_ConeInfo.Set(-dir.m_X, -dir.m_Y, -dir.m_Z, 
		cosf(0.5f * m_pLight->GetAngle() * maConstants::c_fAngleToRad));
	pData->light_info.m_Attenuation.Set(m_pLight->GetFalloff0(), m_pLight->GetFalloff1(), m_pLight->GetFalloff2());
	maFloatRGBA light_color = m_pLight->GetIntensity();
//	if (g3dPrefs::CurrentPrefs().m_RendererType == g3dSceneRendererCreate::e_HDR)
	light_color *= m_pLight->GetIntensityFactor();
	light_color.SetAlpha(1.0f); // make sure light doesn't change alpha of color
	if (m_pLight->IsDiffuseEnabled())
	{
		pData->light_info.m_Diffuse = light_color;
	}
	else pData->light_info.m_Diffuse.Set(0,0,0,1);
	if (m_pLight->IsSpecularEnabled())
	{
		pData->light_info.m_Specular = light_color;
	}
	else pData->light_info.m_Specular.Set(0,0,0,1);
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
		if (m_pShaftTexture)
		{
			matTextureMgr::ReleaseTexture(m_pShaftTexture);
			m_pShaftTexture = NULL;
		}

		if (m_Data.m_ShaftTextureFilename.GetValue().GetLength() > 0)
		{

			//	get the file path
			//
			fsLocator tex_loc;
			fsysFileList file_list;
			prjltTextureList::BuildFileList(file_list);
			file_list.GetFilePath(m_Data.m_ShaftTextureFilename.GetValue(), tex_loc);
			tex_loc.Push(m_Data.m_ShaftTextureFilename.GetValue());

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
		m_ShaftTextureNameLoaded = m_Data.m_ShaftTextureFilename.GetValue();

		// MUST call UpdateLightShaft() to install the texture properly!!!!
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
	this->ConfirmTexture();

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

void prjltProjectedLightObject::TransformChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// This callback is triggered from changes to the properties 
	// that require a call to "OrientCamera":
	// Position, Target, Scale, Aspect, Range, Angle
	//
	// Since this changes the transformation updates are made to
	// the world box and 3d icon also.

	maPoint3d pos		= m_Data.m_Position.GetValue();
	maPoint3d target	= m_Data.m_Target.GetValue();

	// pass values to light
	m_pLight->SetPosition( pos );
	m_pLight->SetTarget( target );
	m_pLight->SetRange(m_Data.m_Range.GetValue());
	m_pLight->SetAngle(m_Data.m_Angle.GetValue());
	m_pLight->SetScale(m_Data.m_Scale.GetValue());
	m_pLight->SetAspect(m_Data.m_Aspect.GetValue());
	m_pLight->SetTilt(m_Data.m_Tilt.GetValue());
	m_pLight->SetDepthBias(m_Data.m_DepthBias.GetValue());

	// let light update our camera for generating the
	// depth map
	m_pLight->OrientCamera(m_ShadowCamera);

	// update 3d icons
	this->update_3d_icon();
	this->UpdateFalloffIcon();

	// Set the value of the pitch and yaw properties which are derived
	// from position and target
	maVector3d view_dir = target - pos;
	float pitch = 0, yaw = 0, distance = 0;
	convert_pitch_yaw(view_dir, pitch, yaw, distance);
	m_Pitch.SetValue(maConstants::c_fRadToAngle * pitch);
	m_Yaw.SetValue(maConstants::c_fRadToAngle * yaw);

	// update world box	
	this->update_world_box();

	// Set our "LogAspect" user interface property
	m_LogAspect.SetValue( logf( m_Data.m_Aspect.GetValue() ) );

	// update position of light shaft
	// Is UpdateLightShaft() enough here, or do we need ConfirmLightShaft()?
	ConfirmLightShaft();
	UpdateLightShaft();

	if (i_bDirty)
		prjltDocumentChunk::ActiveDataChanged();
}

void prjltProjectedLightObject::ColorChanged(prtyProperty *i_pProperty, bool i_bDirty)
{		
	// color is reflected in the light, the 3d icon and the light shaft
	m_pLight->SetIntensity(m_Data.m_Color.GetValue());
	m_pLight->SetIntensityFactor(m_Data.m_Intensity.GetValue());

	m_pObject->SetColor(m_Data.m_Color.GetValue());

	this->UpdateLightShaft();

	if (i_bDirty)
		prjltDocumentChunk::ActiveDataChanged();
}

void prjltProjectedLightObject::EnabledChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// Enable light
	m_pLight->SetEnable( m_Data.m_Enabled.GetValue() );

	// These confirm calls will create/delete the related objects.
	this->ConfirmDepthMap();
	this->ConfirmLightShaft();
	this->UpdateLightShaft();

	if (i_bDirty)
		prjltDocumentChunk::ActiveDataChanged();
}

void prjltProjectedLightObject::ShadowSourceChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	//DBG_LOG1("Light casts shadow: %s", m_Data.m_ShadowSource ? "True" : "False");
//	m_pLight->SetCastsShadow(m_Data.m_ShadowSource);

	this->ConfirmDepthMap();
	this->UpdateLightShaft();

	if (i_bDirty)
		prjltDocumentChunk::ActiveDataChanged();
}

void prjltProjectedLightObject::LightChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// Set properties of light that are only reflected in the light itself,
	// not in the 3d icon or in the depth map
	//
	m_pLight->SetFalloff0(m_Data.m_Falloff.GetValue()[0]);
	m_pLight->SetFalloff1(m_Data.m_Falloff.GetValue()[1]);
	m_pLight->SetFalloff2(m_Data.m_Falloff.GetValue()[2]);

	m_pLight->SetDiffuseEnabled( m_Data.m_bDiffuseEnabled.GetValue() );
	m_pLight->SetSpecularEnabled( m_Data.m_bSpecularEnabled.GetValue() );
	m_pLight->SetAffectsFur(m_Data.m_bAffectsFur.GetValue());
	m_pLight->SetAffectsGlow(m_Data.m_bAffectsGlow.GetValue());

	m_pLight->SetLightSize(m_Data.m_LightSize.GetValue());
	m_pLight->SetSceneScale(m_Data.m_SceneScale.GetValue());
	m_pLight->SetShadowQuality(m_Data.m_ShadowQuality.GetValue());
	m_pLight->SetShadowIntensity(m_Data.m_ShadowIntensity.GetValue());
	m_pLight->SetShadowColor(m_Data.m_ShadowColor.GetValue());

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
	this->ConfirmLightShaftTexture();
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
void prjltProjectedLightObject::ManipModeChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	update_world_box();
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

//--------------------------------------------------------------------
// update world box based on manip mode
//--------------------------------------------------------------------
void prjltProjectedLightObject::update_world_box()
{
	m_WorldBox = l_SphereBox;
	
	if (m_ManipMode.GetValue() == e_Target)
	{
		m_WorldBox.Translate(m_Data.m_Target.GetValue());

		// Make bounding box larger when icon is getting larger
		// because the global scale is getting bigger
		m_WorldBox.Swell(0.5f * api3dScale::GetGlobalScale());
	}
	else
	{
		m_WorldBox.Translate(m_Data.m_Position.GetValue());

		// Make bounding box larger when icon is getting larger
		// because the scale is getting bigger
		m_WorldBox.Swell(0.5f * m_Data.m_Scale.GetValue());
	}
}

//--------------------------------------------------------------------
// update 3d icon with projected light values
//--------------------------------------------------------------------
void prjltProjectedLightObject::update_3d_icon()
{
	if (!m_pObject) return;

	// If we are not showing the frustrum, then shorten the range to 
	// a shorter value.
	const float c_ShortFrustrum = 1.0f;
	float range = (m_ShowFrustrum.GetValue()) ? 
		m_Data.m_Range.GetValue() : api3dScale::Scale(c_ShortFrustrum);

	matTexture* pTexture = (m_ShowTexture.GetValue()) ? m_pTexture : NULL;

	// update 3d icon
	m_pObject->Update( m_Data.m_Position.GetValue(), 
		m_Data.m_Target.GetValue(), m_Data.m_Scale.GetValue(),
		m_Data.m_Tilt.GetValue(), m_Data.m_Angle.GetValue(), 
		m_Data.m_Aspect.GetValue(), range, pTexture);
}

//--------------------------------------------------------------------
// Update icon that displays falloff range
//--------------------------------------------------------------------
void prjltProjectedLightObject::UpdateFalloffIcon()
{
	m_pRangeIcon->SetRenderable(this->ShouldShowFalloffIcon());

	maPoint3d position = m_Data.m_Position.GetValue();
	maPoint3d target = m_Data.m_Target.GetValue();
	m_pRangeIcon->Update(position,
						 target - position,
						 m_FalloffRange.GetValue(), 
						 m_FalloffPercent.GetValue());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prjltProjectedLightObject::ReportMemory(gfFileTxt& i_File)
{
    std::ostringstream stm;

	if (m_pTexture)
	{
		stm << "  Texture  : " << m_pTexture->GetSize()/1024 << "KB\r\n";
	}

	if (m_pShadowMap)
	{
		stm << "  ShadowMap: " << m_pShadowMap->GetSize()/1024 << "KB\r\n";
	}

	if (m_pShaftTexture)
	{
		stm << "  Shaft Tex: " << m_pShaftTexture->GetSize()/1024 << "KB\r\n";
	}

	i_File.WriteLine(stm.str());
}
