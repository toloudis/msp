/*****************************************************************************
**  prjltProjectedLightObject.hpp
**
**      A prjltProjectedLightObject is a derived class for displaying a projected
**	light's position.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef PRJLT_PROJECTEDLIGHTOBJECT_HPP
#error prjltProjectedLightObject.hpp multiply included
#endif
#define PRJLT_PROJECTEDLIGHTOBJECT_HPP

#ifndef PRJLT_DATA_HPP
#include "Systems/PrjLt/Data/prjltData.hpp"
#endif
#ifndef MNM_OBJECT_HPP
#include "Support/mnm/mnmObject.hpp"
#endif
#ifndef ICN_ICONSCALE_HPP
#include "Tool/icn/icnIconScale.hpp"
#endif
#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif
#ifndef CMM_NAMEDPROPERTYOBJECT_HPP
#include "Systems/Common/Object/cmmNamedPropertyObject.hpp"
#endif 
#ifndef PRTY_ENUM_HPP
#include "Core/prty/prtyEnum.hpp"
#endif
#ifndef CMM_FALLOFFASPECT_HPP
#include "Systems/Common/Object/cmmFalloffAspect.hpp"
#endif
#ifndef RMP_DATA_HPP
#include "Support/rmp/rmpData.hpp"
#endif
#ifndef LTST_ISOLATABLE_HPP
#include "Support/ltst/ltstIsolatable.hpp"
#endif 
#ifndef XFRM_TRANSFORMNODECALLBACK_HPP
#include "Support/xfrm/xfrmTransformNodeCallback.hpp"
#endif 
#ifndef G3D_PROJECTEDLIGHT_HPP
#include "Graphics/G3d/g3dProjectedLight.hpp"
#endif 

#define NUM_OPACITY_MAPS 8

//============================================================================
//	forward references
//============================================================================
class api3dObjectSimple;
class fsLocator;
class g3dFragment;
class g3dProjectedLight;
class g3dSceneRenderer;
class g3dTargetRenderer;
class gfFileTxt;
class gpxCamera;
class gpxProjectedLight;
class matMaterial;
class matTexture;
class prjltIconObject;
class prjltRangeIcon;
class prtyRangedFloatUIInfo;
class sel3dObject;



//============================================================================
//============================================================================
class prjltProjectedLightObject : public mnmObject, 
									public cmmNamedPropertyObject,
									public cmmFalloffAspect,
									public icnIconScaleInterest,
									public ltstIsolatable,
									public xfrmTransformNodeCallback
{
	public:
		//------------------------------------------------------------------------
		// Set directory to find projected textures
		//------------------------------------------------------------------------
		static void SetTextureDir(const fsLocator &i_Dir);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prjltProjectedLightObject(prjltData::LightType i_LightType);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~prjltProjectedLightObject();

	//============================================================================
	//	Overloads
	//============================================================================

		//--------------------------------------------------------------------
		// Get the name of the object.
		//--------------------------------------------------------------------
		virtual std::string GetDisplayName() const;

		//--------------------------------------------------------------------
		//	UpdateIconScale - function that sets the icons scale.
		//--------------------------------------------------------------------
		virtual void UpdateIconScale( int i_IconLayerIndex, const camCamera& i_Camera, float i_Scale );

	//============================================================================
	//	Data
	//============================================================================

		//--------------------------------------------------------------------
		// Get values as a data structure
		//--------------------------------------------------------------------
		const prjltData& GetData() const;

		//--------------------------------------------------------------------
		// Set from data structure
		//--------------------------------------------------------------------
		void SetData(const prjltData &i_Data);

	//============================================================================
	//	properties
	//============================================================================

		//--------------------------------------------------------------------
		// Name property access
		//--------------------------------------------------------------------
		prtyName&	PropertyName();
		const prtyName&	GetPropertyName() const;

		//--------------------------------------------------------------------
		// FileName property access
		//--------------------------------------------------------------------
		prtyTextureFileName&	PropertyTextureFileName();
		const prtyTextureFileName&	GetPropertyTextureFileName() const;

		//--------------------------------------------------------------------
		// Position property access
		//--------------------------------------------------------------------
		prtyPoint3d&	PropertyPosition();
		const prtyPoint3d&	GetPropertyPosition() const;

		//--------------------------------------------------------------------
		// Target property access
		//--------------------------------------------------------------------
		prtyPoint3d&	PropertyTarget();
		const prtyPoint3d&	GetPropertyTarget() const;

		//--------------------------------------------------------------------
		// Orientation property access
		//--------------------------------------------------------------------
		prtyRotation&	PropertyOrientation();
		const prtyRotation&	GetPropertyOrientation() const;

		//--------------------------------------------------------------------
		// Color property access
		//--------------------------------------------------------------------
		prtyColor&	PropertyColor();
		const prtyColor&	GetPropertyColor() const;

		//--------------------------------------------------------------------
		// Enabled property access
		//--------------------------------------------------------------------
		prtyBoolean&	PropertyEnabled();
		const prtyBoolean&	GetPropertyEnabled() const;
		
		//--------------------------------------------------------------------
		// ShadowSource property access
		//--------------------------------------------------------------------
		prtyBoolean&	PropertyShadowSource();
		const prtyBoolean&	GetPropertyShadowSource() const;

		//--------------------------------------------------------------------
		// Tilt property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyTilt();
		const prtyFloat&	GetPropertyTilt() const;

		//--------------------------------------------------------------------
		// Range property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyRange();
		const prtyFloat&	GetPropertyRange() const;

		//--------------------------------------------------------------------
		// Intensity property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyIntensity();
		const prtyFloat&	GetPropertyIntensity() const;

		//--------------------------------------------------------------------
		// Angle property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyAngle();
		const prtyFloat&	GetPropertyAngle() const;

		//--------------------------------------------------------------------
		// Penumbra property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyPenumbra();
		const prtyFloat&	GetPropertyPenumbra() const;

		//--------------------------------------------------------------------
		// Scale property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyScale();
		const prtyFloat&	GetPropertyScale() const;

		//--------------------------------------------------------------------
		// Aspect property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyAspect();
		const prtyFloat&	GetPropertyAspect() const;

		//--------------------------------------------------------------------
		// ShadowIntensity property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyShadowIntensity();
		const prtyFloat&	GetPropertyShadowIntensity() const;

		//--------------------------------------------------------------------
		// DepthBias property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyDepthBias();
		const prtyFloat&	GetPropertyDepthBias() const;

		//--------------------------------------------------------------------
		// DepthMapSize property access
		//--------------------------------------------------------------------
		prtyInt32&	PropertyDepthMapSize();
		const prtyInt32&	GetPropertyDepthMapSize() const;

		//--------------------------------------------------------------------
		// ReflectiveShadowMapSize property access
		//--------------------------------------------------------------------
		prtyInt32&	PropertyRSMapSize();
		const prtyInt32&	GetPropertyRSMapSize() const;

		//--------------------------------------------------------------------
		// LightSize property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyLightSize();
		const prtyFloat&	GetPropertyLightSize() const;

		//--------------------------------------------------------------------
		// PCSSAdjust property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyPCSSAdjust();
		const prtyFloat&	GetPropertyPCSSAdjust() const;

		//--------------------------------------------------------------------
		// ShaftVisible property access
		//--------------------------------------------------------------------
		prtyBoolean&	PropertyShaftVisible();
		const prtyBoolean&	GetPropertyShaftVisible() const;

		//--------------------------------------------------------------------
		// ShaftAlpha property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyShaftAlpha();
		const prtyFloat&	GetPropertyShaftAlpha() const;

		//--------------------------------------------------------------------
		// ShaftDensity property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyShaftDensity();
		const prtyFloat&	GetPropertyShaftDensity() const;

		//--------------------------------------------------------------------
		// EnableDiffuse property access
		//--------------------------------------------------------------------
		prtyBoolean&	PropertyEnableDiffuse();
		const prtyBoolean&	GetPropertyEnableDiffuse() const;

		//--------------------------------------------------------------------
		// EnableSpecular property access
		//--------------------------------------------------------------------
		prtyBoolean&	PropertyEnableSpecular();
		const prtyBoolean&	GetPropertyEnableSpecular() const;

		//--------------------------------------------------------------------
		// AffectsGlow property access
		//--------------------------------------------------------------------
		prtyBoolean&	PropertyAffectsGlow();
		const prtyBoolean&	GetPropertyAffectsGlow() const;

		//--------------------------------------------------------------------
		// Falloff property access
		//--------------------------------------------------------------------
		prtyPoint3d&	PropertyFalloff();
		const prtyPoint3d&	GetPropertyFalloff() const;

		//--------------------------------------------------------------------
		// ShaftFalloffStart property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyShaftFalloffStart();
		const prtyFloat&	GetPropertyShaftFalloffStart() const;

		//--------------------------------------------------------------------
		// ShaftFalloffEnd property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyShaftFalloffEnd();
		const prtyFloat&	GetPropertyShaftFalloffEnd() const;

		//--------------------------------------------------------------------
		// ShowTexture property access
		//--------------------------------------------------------------------
		prtyBoolean&	PropertyShowTexture();
		const prtyBoolean&	GetPropertyShowTexture() const;

	//============================================================================
	//	3D icon/geometry
	//============================================================================

		//--------------------------------------------------------------------
		//	GetWorldBox returns a box which completely encloses the
		//	prjltProjectedLightObject.
		//--------------------------------------------------------------------
		virtual maAxisBox GetWorldBox(int i_IconLayerIndex = 0) const;

		//--------------------------------------------------------------------
		//	GetWorldPivot returns the point in world space that this
		//		object will rotate around. Used to center rotation and
		//		scale compasses.
		//--------------------------------------------------------------------
		virtual maPoint3d GetWorldPivot() const;
		
		//--------------------------------------------------------------------
		//  Get sum of matrices of all parents of this node. 
		//--------------------------------------------------------------------
		virtual void GetParentMatrix(maMatrix4x4 &o_Transformation) const;

		//--------------------------------------------------------------------
		//	Name
		//--------------------------------------------------------------------
		virtual void SetName(const nameString& i_Name);

		//--------------------------------------------------------------------
		//	Position
		//--------------------------------------------------------------------
		virtual maPoint3d GetPosition() const;

		//----------------------------------------------------------------------------
		//	UpdatePosition - compass interaction has altered the position of
		//	of this light
		//----------------------------------------------------------------------------
		virtual void UpdatePosition(const maPoint3d& i_Position, 
										bool i_bNewOperation);

		//--------------------------------------------------------------------
		//	Orientation
		//--------------------------------------------------------------------
		virtual maRotation GetOrientation() const;
		virtual void UpdateOrientation(const maRotation& i_Orientation, 
										bool i_bNewOperation);

		//--------------------------------------------------------------------
		//	Scale
		//--------------------------------------------------------------------
		virtual maPoint3d GetScale() const;
		virtual void UpdateScale(const maPoint3d& i_Scale, 
										bool i_bNewOperation);

		//----------------------------------------------------------------------------
		// Find the object that matches the pick code from an earlier pick render.
		//----------------------------------------------------------------------------
		virtual bool MatchPickCode(envType::UInt32 i_PickCode);

		//--------------------------------------------------------------------
		//	Renderable sets whether the icon is visible
		//--------------------------------------------------------------------
		virtual void SetRenderable(bool i_Renderable);

		//--------------------------------------------------------------------
		// Set object active state in the render layer
		//--------------------------------------------------------------------
		void SetActiveRenderLayer(bool i_bActive);

		//--------------------------------------------------------------------
		//  Changes visible state of light based on GUI
		//--------------------------------------------------------------------
		void SetEditorVisible(bool i_bVisible);
		bool GetEditorVisible() const;

		//--------------------------------------------------------------------
		//	LayerVisible represents if objects in the layer are visible
		//--------------------------------------------------------------------
		void SetLayerVisible(bool i_bVisible);

		//--------------------------------------------------------------------
		//	GPUPickable sets whether the icons should be rendered
		//	in pick renders when doing GPU picking
		//--------------------------------------------------------------------
		void SetGPUPickable(bool i_Pickable);

		//--------------------------------------------------------------------
		// Flags for how object can be modified
		//--------------------------------------------------------------------
		virtual RotateFlags	GetRotateFlags();
		virtual ScaleFlags	GetScaleFlags();
		virtual TranslateFlags GetTranslateFlags();

		//--------------------------------------------------------------------
		// Return this light's range icon
		//--------------------------------------------------------------------
		virtual prjltRangeIcon* GetRangeIcon();

		//--------------------------------------------------------------------
		// Get parent object of this object in order to define relationships
		//	between icons and their affected objects.
		//--------------------------------------------------------------------
		//virtual sel3dObject* GetParentObject() const;
		//virtual void SetParentObject(sel3dObject* i_pParent);

		//--------------------------------------------------------------------
		//	Get reference for given name.  The returned projecteder is owned
		//	by this object.  The object should be retained by the caller
		//	to avoid repeated string searches.
		//--------------------------------------------------------------------
		//api3dReference* GetReference(const char* i_Name);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void ReportMemory(gfFileTxt& i_File);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void SelectObject();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void DeSelectObject();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		g3dProjectedLight* GetLight();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		matTexture* GetRampTexture();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		matTexture* GetTexture();


	private:

		//--------------------------------------------------------------------
		// Updates aspects of the projected light
		//--------------------------------------------------------------------
		void ConfirmTexture();
		void ConfirmDepthMap();
		void ConfirmOpacityMap();
		void ConfirmLightShaft();
		void ConfirmReflectiveMap();
		void UpdateLightShaft();
		void ConfirmLightShaftTexture();
		void RampChangedFromUI(bool i_bDirty);
		void RampChangedFromData(prtyProperty *i_pProperty, bool i_bDirty);
		void RampChanged(bool i_bDirty);
		void NotifyRampUI();
		void UpdateRampData(prtyTextureFileName& io_Texture, const rmpData& i_RampData);
		void ConfirmRampTexture(int i_texSize);
		void ShaftRampChangedFromUI(bool i_bDirty);
		void ShaftRampChangedFromData(prtyProperty *i_pProperty, bool i_bDirty);
		void ShaftRampChanged(bool i_bDirty);
		void NotifyShaftRampUI();
		void ConfirmShaftRampTexture(int i_texSize);

		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void NameChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void TextureChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void DepthMapChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void OpacityMapChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void ColorChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void TransformChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void AngleMeaningChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void EnabledChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void ShadowSourceChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void GISourceChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void LightChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void LightShaftChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void LightShaftTextureChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void PitchYawChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void ManipModeChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void LogAspectChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void IconDisplayChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void RampEnabledChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void RampTriggerChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void HairChanged(prtyProperty *i_pProperty, bool i_bDirty);

		//--------------------------------------------------------------------
		// update 3d icon with projected light values
		//--------------------------------------------------------------------
		void update_3d_icon();

		//--------------------------------------------------------------------
		// compute world position of light using parent transformations
		//--------------------------------------------------------------------
		void get_world_positions(maPoint3d &o_Position, maPoint3d &o_Target) const;

		//--------------------------------------------------------------------
		// common code when a property related to position of light is changed
		// If i_bCheckForDifference==true, only submit changes to proxy if the
		// target or position changes.
		//--------------------------------------------------------------------
		void transformation_changed(bool i_bCheckForDifference = false);

		//--------------------------------------------------------------------
		// Called from transformation manager once per frame, update light
		//	position based on parent transformation matrix.
		//--------------------------------------------------------------------
		virtual void UpdateParentTransform();
		
		//--------------------------------------------------------------------
		// Called from transformation manager when the parenting of this 
		// object changes in a way that we need to alter our values to
		// saty in the same world position.
		//--------------------------------------------------------------------
		virtual void ApplyTransformation(const maMatrix4x4& i_Matrix);		
		
		//--------------------------------------------------------------------
		// Return true if this object has a pivot point based on its icon.
		// If so, return pivot point in the o_Pivot argument.
		//--------------------------------------------------------------------
		virtual bool HasIconPivotPoint(maPoint3d& o_Pivot);

		//--------------------------------------------------------------------
		//	Callback from falloff aspect, update the falloff icon
		//--------------------------------------------------------------------
		virtual void UpdateFalloffIcon();

		//--------------------------------------------------------------------
		//	IsolationChanged - notification that this object's isolation
		//	state has changed (fading on or out the light's intensity)
		//--------------------------------------------------------------------
		virtual void IsolationChanged();

		//--------------------------------------------------------------------
		// return the ramp data of the texture control
		//--------------------------------------------------------------------
		rmpData GetRampData(prtyTextureFileName& io_Texture, const rmpData& i_RampData);

	private:
		prjltData	m_Data;

		g3dProjectedLight* m_pProjectedLight;
		gpxProjectedLight* m_pLightProxy;

		// 3D icon/geometry info
		prjltIconObject *m_pObject;
		bool m_bRenderable;
		bool m_bLayerVisible;
		bool m_bLayerPickable;
		float m_ParentScale;	// scale transformation coming from parents

		// Two properties that are not animatable or saved to file,
		// these can be derived from position and target
		prtyFloat		m_Pitch;		// degrees pitch
		prtyFloat		m_Yaw;			// degrees yaw
		// this is a temporary mode for what is the center of the manipulation
		prtyEnum		m_ManipMode;
		// This represents the "log" of the aspect ratio, which
		// is more easy to control with a slider
		prtyFloat		m_LogAspect;
		// These three control the display of the icon
		prtyBoolean		m_ShowFrustrum;
		prtyBoolean		m_ShowTexture;
		prtyFloat		m_TexturePosition;

		// UI Controls that are disabled by some settings
		prtyRangedFloatUIInfo*	m_pAngleControl;
		prtyRangedFloatUIInfo*	m_pPenumbraControl;

		// Textures for projection
		matTexture* m_pTexture;
		matTexture* m_pShadowMap;
		fsLocator m_TextureNameLoaded;
		int m_DepthMapSize;

		// for rendering depth map for the projected light
		g3dTargetRenderer* m_pMapRenderer;
		g3dSceneRenderer* m_pDepthRenderer;
		camCamera m_ShadowCamera;
		gpxCamera* m_ShadowCameraProxy;

		//Hair properties
		int m_HairShadowSize;
		HAIR_SHADOW_TYPE m_HairShadowType;
		matTexture* m_pOpacityShadowMap[ NUM_OPACITY_MAPS ];
		matTexture* m_pOpacityVolume;
		g3dTargetRenderer* m_pOpacityMapRenderer;
		g3dSceneRenderer* m_pOpacityRenderer;

		// the 3d shape of the shaft of light
		api3dObjectSimple* m_pLightShaft;
		matMaterial* m_pLightShaftMat;
		matTexture* m_pShaftTexture;
		fsLocator m_ShaftTextureNameLoaded;

		sel3dObject* m_pParent;

		// Falloff range icon
		prjltRangeIcon*		m_pRangeIcon;

		// Ramp Texture variables
		matTexture* m_pRampTexture;	// target texture the ramp is rendering to
		matTexture* m_pShaftRampTexture;	// target texture the ramp is rendering to for shaft
		int m_RampTextureSize;
		int m_ShaftRampTextureSize;

		// Textures for GI map
		/*matTexture* m_pReflectiveMap;
		g3dSceneRenderer* m_pRSMRenderer;
		g3dTargetRenderer* m_pRSMTargetRenderer;
		int m_ReflectiveMapSize;*/

};

