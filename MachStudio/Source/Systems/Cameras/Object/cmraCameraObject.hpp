/*****************************************************************************
**	cmraCameraObject.hpp
**
**	A cmraCameraObject is a derived class for recording and displaying
**	a camera's position.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef CMRA_CAMERAOBJECT_HPP
#error cmraCameraObject.hpp multiply included
#endif
#define CMRA_CAMERAOBJECT_HPP

#ifndef CMRA_DATA_HPP
#include "Systems/Cameras/Data/cmraData.hpp"
#endif
#ifndef ICN_ICONSCALE_HPP
#include "Tool/icn/icnIconScale.hpp"
#endif
#ifndef MNM_OBJECT_HPP
#include "Support/mnm/mnmObject.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif
#ifndef CMM_NAMEDPROPERTYOBJECT_HPP
#include "Systems/Common/Object/cmmNamedPropertyObject.hpp"
#endif 
#ifndef GPX_CAMERA_HPP
#include "Tool/gpx/gpxCamera.hpp"
#endif 
#ifndef XFRM_TRANSFORMNODECALLBACK_HPP
#include "Support/xfrm/xfrmTransformNodeCallback.hpp"
#endif 

#include <string>


//============================================================================
//	forward references
//============================================================================
class api3dObject;
class api3dObjectSimple;
class camCamera;
class cmraIconObject;
class prtyNumericUpDownUIInfo;
class prtyComboBoxUIInfo;
class prtyRangedFloatUIInfo;
class prtyFloatEditUIInfo;
class gpxSceneObject;

//============================================================================
//============================================================================
class cmraCameraObject : public mnmObject, 
						 public cmmNamedPropertyObject,
						 public gpxCamera::CameraChangedCallback,
						 public icnIconScaleInterest,
						 public xfrmTransformNodeCallback
{
	public:
		//--------------------------------------------------------------------
		/// Constructor - set perspective or orthographic camera here.
		///	If i_bUseEditorCamera is true, this object attaches to the
		///	cam3dMgr editor camera.
		//--------------------------------------------------------------------
		cmraCameraObject(bool i_bOrthographic,
						 bool i_bUseEditorCamera = false);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~cmraCameraObject();

	//============================================================================
	///	Overloads
	//============================================================================

		//--------------------------------------------------------------------
		/// Get the name of the object.
		//--------------------------------------------------------------------
		virtual std::string GetDisplayName() const;

		//--------------------------------------------------------------------
		//	CreateReferenceToSelf - create a reference that refers to the
		//	given object. This reference should be able to refer to 
		//  future instances of this object persistent through deletion
		//	and recreation through undo.
		//--------------------------------------------------------------------
		virtual shared_ptr<relObjectReference> CreateReferenceToSelf();

		//--------------------------------------------------------------------
		//	GlobalScaleChanged - notification that global scale has changed.
		//--------------------------------------------------------------------
		//virtual void GlobalScaleChanged( float i_Scale );

		//--------------------------------------------------------------------
		//	UpdateIconScale - function that sets the icons scale.
		//--------------------------------------------------------------------
		virtual void UpdateIconScale( int i_IconLayerIndex, const camCamera& i_Camera, float i_Scale );

	//============================================================================
	//	Data
	//============================================================================

		//--------------------------------------------------------------------
		/// Get values as a camera data structure
		//--------------------------------------------------------------------
		const cmraCameraData& GetData() const;

		//--------------------------------------------------------------------
		// Set from camera data structure
		//--------------------------------------------------------------------
		void SetData(const cmraCameraData &i_Data);

		//--------------------------------------------------------------------
		// CleanupPassBuffers()
		//--------------------------------------------------------------------
		void CleanupPassBuffers();

	//============================================================================
	//	properties
	//============================================================================

		//--------------------------------------------------------------------
		// Name property access
		//--------------------------------------------------------------------
		prtyName&	PropertyName();
		const prtyName&	GetPropertyName() const;

		//--------------------------------------------------------------------
		// Description property access
		//--------------------------------------------------------------------
		prtyText&	PropertyDescription();
		const prtyText&	GetPropertyDescription() const;

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
		// FieldOfView property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyFieldOfView();
		const prtyFloat&	GetPropertyFieldOfView() const;

		//--------------------------------------------------------------------
		// OrthoWidth property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyOrthoWidth();
		const prtyFloat&	GetPropertyOrthoWidth() const;

		//--------------------------------------------------------------------
		// Tilt property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyTilt();
		const prtyFloat&	GetPropertyTilt() const;

		//--------------------------------------------------------------------
		// Focus distance property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyNearFocusDistance();
		const prtyFloat&	GetPropertyNearFocusDistance() const;

		//--------------------------------------------------------------------
		// Focus distance property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyFarFocusDistance();
		const prtyFloat&	GetPropertyFarFocusDistance() const;
		
		//--------------------------------------------------------------------
		// Focus distance property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyFarBlurDistance();
		const prtyFloat&	GetPropertyFarBlurDistance() const;

		//--------------------------------------------------------------------
		// Focus distance property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyNearBlurDistance();
		const prtyFloat&	GetPropertyNearBlurDistance() const;

		//--------------------------------------------------------------------
		// NearClip property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyNearClip();
		const prtyFloat&	GetPropertyNearClip() const;

		//--------------------------------------------------------------------
		// FarClip property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyFarClip();
		const prtyFloat&	GetPropertyFarClip() const;

		//--------------------------------------------------------------------
		// DOFMaxFarBlur property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyDOFMaxFarBlur();
		const prtyFloat&	GetPropertyDOFMaxFarBlur() const;

		//--------------------------------------------------------------------
		// MaxCoC property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyMaxCoC();
		const prtyFloat&	GetPropertyMaxCoC() const;

		//--------------------------------------------------------------------
		// Focal Length property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyFocalLength();
		const prtyFloat&	GetPropertyFocalLength() const;

		//--------------------------------------------------------------------
		// Horizontal Aperture property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyHorizontalAperture();
		const prtyFloat&	GetPropertyHorizontalAperture() const;

		//--------------------------------------------------------------------
		// FStop property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyFStop();
		const prtyFloat&	GetPropertyFStop() const;

		//--------------------------------------------------------------------
		// Focal Distance property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyFocalDistance();
		const prtyFloat&	GetPropertyFocalDistance() const;

		//--------------------------------------------------------------------
		// CoC property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyCoC();
		const prtyFloat&	GetPropertyCoC() const;

		//--------------------------------------------------------------------
		// HDRMiddleGray property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyHDRMiddleGray();
		const prtyFloat&	GetPropertyHDRMiddleGray() const;

		//--------------------------------------------------------------------
		// HDRBloomScale property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyHDRBloomScale();
		const prtyFloat&	GetPropertyHDRBloomScale() const;

		//--------------------------------------------------------------------
		// HDRStarScale property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyHDRStarScale();
		const prtyFloat&	GetPropertyHDRStarScale() const;

		//--------------------------------------------------------------------
		// HDRBrightPassThresh property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyHDRBrightPassThresh();
		const prtyFloat&	GetPropertyHDRBrightPassThresh() const;

		//--------------------------------------------------------------------
		// HDRBrightPassOffset property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyHDRBrightPassOffset();
		const prtyFloat&	GetPropertyHDRBrightPassOffset() const;

		//--------------------------------------------------------------------
		// HDRWhiteCutoff property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyHDRWhiteCutoff();
		const prtyFloat&	GetPropertyHDRWhiteCutoff() const;

		//--------------------------------------------------------------------
		// HDRSceneLuminance property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyHDRSceneLuminance();
		const prtyFloat&	GetPropertyHDRSceneLuminance() const;

		//--------------------------------------------------------------------
		// StereoFD property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyStereoFD();
		const prtyFloat&	GetPropertyStereoFD() const;

		//--------------------------------------------------------------------
		// StereoIOD property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyStereoIOD();
		const prtyFloat&	GetPropertyStereoIOD() const;

		//--------------------------------------------------------------------
		// PropertyTextureFileNameAO property access
		//--------------------------------------------------------------------
		prtyTextureFileName&	PropertyTextureFileNameAO();
		const prtyTextureFileName&	GetPropertyTextureFileNameAO() const;

		//--------------------------------------------------------------------
		// PropertyIntensityAO property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyIntensityAO();
		const prtyFloat&	GetPropertyIntensityAO() const;

		//--------------------------------------------------------------------
		// PropertyBlendOpAO property access
		//--------------------------------------------------------------------
		prtyInt32&	PropertyBlendOpAO();
		const prtyInt32&	GetPropertyBlendOpAO() const;

		//--------------------------------------------------------------------
		// PropertyTextureFileNameGI property access
		//--------------------------------------------------------------------
		prtyTextureFileName&	PropertyTextureFileNameGI();
		const prtyTextureFileName&	GetPropertyTextureFileNameGI() const;

		//--------------------------------------------------------------------
		// PropertyIntensityGI property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyIntensityGI();
		const prtyFloat&	GetPropertyIntensityGI() const;

		//--------------------------------------------------------------------
		// PropertyBlendOpGI property access
		//--------------------------------------------------------------------
		prtyInt32&	PropertyBlendOpGI();
		const prtyInt32&	GetPropertyBlendOpGI() const;

		//--------------------------------------------------------------------
		// FileName property access
		//--------------------------------------------------------------------
		prtyTextureFileName&	PropertyTextureFileNameRefl();
		const prtyTextureFileName&	GetPropertyTextureFileNameRefl() const;

		//--------------------------------------------------------------------
		// PropertyIntensityRefl property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyIntensityRefl();
		const prtyFloat&	GetPropertyIntensityRefl() const;

		//--------------------------------------------------------------------
		// PropertyBlendOpRefl property access
		//--------------------------------------------------------------------
		prtyInt32&	PropertyBlendOpRefl();
		const prtyInt32&	GetPropertyBlendOpRefl() const;

		//--------------------------------------------------------------------
		// PropertyTextureFileNameShadowMask property access
		//--------------------------------------------------------------------
		prtyTextureFileName&	PropertyTextureFileNameShadowMask();
		const prtyTextureFileName&	GetPropertyTextureFileNameShadowMask() const;

		//--------------------------------------------------------------------
		// PropertyIntensityShadowMask property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyIntensityShadowMask();
		const prtyFloat&	GetPropertyIntensityShadowMask() const;

		//--------------------------------------------------------------------
		// PropertyBlendOpShadowMask property access
		//--------------------------------------------------------------------
		prtyInt32&	PropertyBlendOpShadowMask();
		const prtyInt32&	GetPropertyBlendOpShadowMask() const;

		//--------------------------------------------------------------------
		// PropertyTextureFileNameBeauty property access
		//--------------------------------------------------------------------
		prtyTextureFileName&	PropertyTextureFileNameBeauty();
		const prtyTextureFileName&	GetPropertyTextureFileNameBeauty() const;

		//--------------------------------------------------------------------
		// PropertyIntensityBeauty property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyIntensityBeauty();
		const prtyFloat&	GetPropertyIntensityBeauty() const;

		//--------------------------------------------------------------------
		// PropertyBlendOpBeauty property access
		//--------------------------------------------------------------------
		prtyInt32&	PropertyBlendOpBeauty();
		const prtyInt32&	GetPropertyBlendOpBeauty() const;

	//============================================================================
	//	3D icon/geometry
	//============================================================================

		//--------------------------------------------------------------------
		//	UpdateIcon() - update the graphical icon using the current
		//	cmraCameraObject settings.
		//--------------------------------------------------------------------
		void UpdateIcon();

		//--------------------------------------------------------------------
		//	GetWorldBox returns a box which completely encloses the
		//	cmraCameraObject.
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
		//	of this camera
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
		//	Renderable sets whether the cmraCameraObject can be selected.
		//--------------------------------------------------------------------
		void SetRenderable(bool i_Renderable);
		
		//--------------------------------------------------------------------
		// Set object active state in the render layer
		//--------------------------------------------------------------------
		void SetActiveRenderLayer(bool i_bActive);
		
		//--------------------------------------------------------------------
		//  Changes visible state of camera based on GUI
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
		// For polling if an manipulation operation is currently enabled
		//--------------------------------------------------------------------
		//virtual bool IsOperationEnabled(Operations i_Operation, const maTime& i_Time);

		//--------------------------------------------------------------------
		// Get parent object of this object in order to define relationships
		//	between icons and their affected objects.
		//--------------------------------------------------------------------
		//virtual sel3dObject* GetParentObject() const;
		//virtual void SetParentObject(sel3dObject* i_pParent);
		
		//--------------------------------------------------------------------
		// Accessor to camera
		//--------------------------------------------------------------------
		const camCamera& GetCamera() const;
		camCamera& Camera();
		shared_ptr<camCamera> GetCameraPtr();
		shared_ptr<gpxCamera> GetCameraProxyPtr();

	private:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		api3dObjectSimple* create_line();

		//----------------------------------------------------------------------------
		/// Setup enabled state for controls based on EnableALP setting
		//----------------------------------------------------------------------------
		void enable_lens_controls();

		//----------------------------------------------------------------------------
		/// These functions set the camCamera properties depending on the lens
		/// properties
		//----------------------------------------------------------------------------
		bool check_divideby_zero_error(float& f, float&s, float&n, float& c, float& H, float& H2);
		void set_camera_fov();
		void set_camera_dof();

		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void NameChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void TransformChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void FOVChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void OrthoWidthChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void DescriptionChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void EnableALPChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void FilmGateChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void ApertureChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void LensChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void DOFChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void FocalLengthChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void CameraChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void CameraPassBuffersChanged(prtyProperty *i_pProperty, bool i_bDirty);
		matTexture* CameraPassBuffersTexChanged(matTexture* o_Mat, const prtyTextureFileName& i_Loc);
		void CameraPassBuffersAOTexChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void CameraPassBuffersGITexChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void CameraPassBuffersReflTexChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void CameraPassBuffersShadowMaskTexChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void CameraPassBuffersBeautyTexChanged(prtyProperty *i_pProperty, bool i_bDirty);
		//void AspectRatioChanged(prtyProperty *i_pProperty, bool i_bDirty);
		//void MatchAspectChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void PitchYawChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void ManipModeChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void ToggleStereoChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void StereoAngleChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void StereoFDChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void StereoFilterColorChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void StereoTypeChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void StereoIODChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void StereoProjectionChanged(prtyProperty *i_pProperty, bool i_bDirty);

		//--------------------------------------------------------------------
		// This callback is for when the manipulator changes the camera.
		// Set the new values from the camera into the position and
		// target properties.
		//--------------------------------------------------------------------
		virtual void CameraChanged(const gpxCamera*);

		//--------------------------------------------------------------------
		// compute world position of light using parent transformations
		//--------------------------------------------------------------------
		void get_world_positions(maPoint3d &o_Position, maPoint3d &o_Target) const;

		//--------------------------------------------------------------------
		// common code when a property related to position of light is changed
		//--------------------------------------------------------------------
		void transformation_changed();

		//--------------------------------------------------------------------
		// Called from transformation manager once per frame, update camera
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

	private:
		cmraCameraData	m_Data;

		// Two properties that are not animatable or saved to file,
		// these can be derived from position and target
		prtyFloat		m_Pitch;		// degrees pitch
		prtyFloat		m_Yaw;			// degrees yaw
		// this is a temporary mode for what is the center of the
		//	manipulation
		prtyEnum		m_ManipMode;

		// Film Gate enumeration is a convenience for setting aperture
		// from a table, but is not part of the inherent data of the camera itself
		prtyEnum		m_FilmGate;

		// Checkbox enables/disables aspect ratio control
		//prtyNumericUpDownUIInfo* m_pAspectRatioControl;

		// EnableALP Checkbox enables/disables other property UI Infos
		prtyNumericUpDownUIInfo*	m_pFOVControl;
		prtyComboBoxUIInfo*			m_pFilmGateControl;
		prtyRangedFloatUIInfo*		m_pFocalLengthControl;
		prtyRangedFloatUIInfo*		m_pFStopControl;
		prtyNumericUpDownUIInfo*	m_pFocusDistanceControl;
		prtyNumericUpDownUIInfo*	m_pCoCControl;
		prtyFloatEditUIInfo*		m_pHorizontalApertureControl;

		prtyNumericUpDownUIInfo*	m_pNearBlurDistance;
		prtyNumericUpDownUIInfo*	m_pNearFocalDistance;
		prtyNumericUpDownUIInfo*	m_pFarFocalDistance;
		prtyNumericUpDownUIInfo*	m_pFarBlurDistance;
		prtyNumericUpDownUIInfo*	m_pMaxCoC;

		// 3D icon/geometry info
		cmraIconObject*	m_pObject;
		api3dObject*	m_pTargetObject;
		gpxSceneObject* m_pTargetObjectProxy;
		sel3dObject*	m_pParent;

		bool m_bRenderable, m_bLayerVisible, m_bIsValid;
		bool				m_bLayerPickable;


		camPassBuffersData	m_PassBuffersData;

		// Camera being controlled
		shared_ptr<camCamera>	m_Camera;
		shared_ptr<gpxCamera>	m_CameraProxy;
};

