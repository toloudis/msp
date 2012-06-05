/*****************************************************************************
**  prjltProjectedLightObject.hpp
**
**      A prjltProjectedLightObject is a derived class for displaying a projected
**	light's position.
**
**	Extra Large Technology
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

#ifndef API3D_SCALE_HPP
#include "Tool/api3d/api3dScale.hpp"
#endif
#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif
#ifndef NAME_OBJECT_HPP
#include "Core/name/nameObject.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif
#ifndef PRTY_ENUM_HPP
#include "Core/prty/prtyEnum.hpp"
#endif
#ifndef CMM_FALLOFFASPECT_HPP
#include "Systems/Common/Object/cmmFalloffAspect.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class api3dObjectSimple;
class fsLocator;
class g3dProjectedLight;
class g3dSceneRenderer;
class g3dTargetRenderer;
class gfFileTxt;
class matMaterial;
class matTexture;
class prjltIconObject;
class pick3dPickObject;
class prjltRangeIcon;


//============================================================================
//============================================================================
class prjltProjectedLightObject : public mnmObject, 
									public nameObject, 
									public prtyObject,
									public cmmFalloffAspect,
									public api3dScaleInterest
{
	public:
		//------------------------------------------------------------------------
		// Set directory to find projected textures
		//------------------------------------------------------------------------
		static void SetTextureDir(const fsLocator &i_Dir);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prjltProjectedLightObject();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~prjltProjectedLightObject();

	//============================================================================
	//	Overloads
	//============================================================================

		//--------------------------------------------------------------------
		// Get the name of the object.
		//--------------------------------------------------------------------
		virtual std::string GetPick3dName() const;

		//--------------------------------------------------------------------
		//	GlobalScaleChanged - notification that global scale has changed.
		//--------------------------------------------------------------------
		virtual void GlobalScaleChanged( float i_Scale );


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
		// LightSize property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyLightSize();
		const prtyFloat&	GetPropertyLightSize() const;

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
		// EnableFur property access
		//--------------------------------------------------------------------
		prtyBoolean&	PropertyEnableFur();
		const prtyBoolean&	GetPropertyEnableFur() const;

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

	//============================================================================
	//	3D icon/geometry
	//============================================================================

		//--------------------------------------------------------------------
		//	GetWorldBox returns a box which completely encloses the
		//	prjltProjectedLightObject.
		//--------------------------------------------------------------------
		virtual const maAxisBox& GetWorldBox() const;

		//--------------------------------------------------------------------
		//	GetLocalBox returns a box which would enclose the prjltProjectedLightObject
		//	if its transformations were identity
		//--------------------------------------------------------------------
		virtual const maAxisBox& GetLocalBox() const;

		//--------------------------------------------------------------------
		//	GetWorldPivot returns the point in world space that this
		//		object will rotate around. Used to center rotation and
		//		scale compasses.
		//--------------------------------------------------------------------
		virtual maPoint3d GetWorldPivot() const;

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

		//--------------------------------------------------------------------
		//	RayPick returns true if the given ray intersects the prjltProjectedLightObject.
		//	If it does, the t value is also returned in o_T.
		//--------------------------------------------------------------------
		virtual bool RayPick(	const maPoint3d& i_RayStart,
								const maPoint3d& i_RayEnd,
								float& o_T);

		//----------------------------------------------------------------------------
		// Find the object that matches the pick code from an earlier pick render.
		//----------------------------------------------------------------------------
		virtual bool MatchPickCode(envType::UInt32 i_PickCode);

		//--------------------------------------------------------------------
		//	Renderable sets whether the icon is visible
		//--------------------------------------------------------------------
		virtual void SetRenderable(bool i_Renderable);

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
		// Get parent object of this object in order to define relationships
		//	between icons and their affected objects.
		//--------------------------------------------------------------------
		virtual pick3dPickObject* GetParentObject() const;
		virtual void SetParentObject(pick3dPickObject* i_pParent);

		//--------------------------------------------------------------------
		//	Get reference for given name.  The returned projecteder is owned
		//	by this object.  The object should be retained by the caller
		//	to avoid repeated string searches.
		//--------------------------------------------------------------------
		//api3dReference* GetReference(const char* i_Name);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void ReportMemory(gfFileTxt& i_File);
	private:

		//--------------------------------------------------------------------
		// Updates aspects of the projected light
		//--------------------------------------------------------------------
		void ConfirmTexture();
		void ConfirmDepthMap();
		void ConfirmLightShaft();
		void UpdateLightShaft();
		void ConfirmLightShaftTexture();

		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void NameChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void TextureChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void DepthMapChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void ColorChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void TransformChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void EnabledChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void ShadowSourceChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void LightChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void LightShaftChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void LightShaftTextureChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void PitchYawChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void ManipModeChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void LogAspectChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void IconDisplayChanged(prtyProperty *i_pProperty, bool i_bDirty);

		//--------------------------------------------------------------------
		// update world box based on manip mode
		//--------------------------------------------------------------------
		void update_world_box();

		//--------------------------------------------------------------------
		// update 3d icon with projected light values
		//--------------------------------------------------------------------
		void update_3d_icon();

		//--------------------------------------------------------------------
		//	Callback from falloff aspect, update the falloff icon
		//--------------------------------------------------------------------
		virtual void UpdateFalloffIcon();

		prjltData	m_Data;

		g3dProjectedLight* m_pLight;

		// 3D icon/geometry info
		prjltIconObject *m_pObject;
		maAxisBox m_WorldBox;
		bool m_bRenderable;
		bool m_bLayerVisible;

		// Two properties that are not animatable or saved to file,
		// these can be derived from position and target
		prtyFloat		m_Pitch;		// degrees pitch
		prtyFloat		m_Yaw;			// degrees yaw
		// this is a temporary mode for what is the center of the manipulation
		prtyEnum		m_ManipMode;
		// This represents the "log" of the aspect ratio, which
		// is more easy to control with a slider
		prtyFloat		m_LogAspect;
		// These two control the display of the icon
		prtyBoolean		m_ShowFrustrum;
		prtyBoolean		m_ShowTexture;

		// Textures for projection
		matTexture* m_pTexture;
		matTexture* m_pShadowMap;
		itString m_TextureNameLoaded;
		int m_DepthMapSize;

		// for rendering depth map for the projected light
		g3dTargetRenderer* m_pMapRenderer;
		g3dSceneRenderer* m_pDepthRenderer;
		camCamera m_ShadowCamera;

		// the 3d shape of the shaft of light
		api3dObjectSimple* m_pLightShaft;
		matMaterial* m_pLightShaftMat;
		matTexture* m_pShaftTexture;
		itString m_ShaftTextureNameLoaded;

		pick3dPickObject* m_pParent;

		// Falloff range icon
		prjltRangeIcon*		m_pRangeIcon;
};

