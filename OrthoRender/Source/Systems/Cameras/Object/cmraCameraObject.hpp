/*****************************************************************************
**  cmraCameraObject.hpp
**
**      A cmraCameraObject is a derived class for recording and displaying
**	a camera's position.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef CMRA_CAMERAOBJECT_HPP
#error cmraCameraObject.hpp multiply included
#endif
#define CMRA_CAMERAOBJECT_HPP

#ifndef CMRA_DATA_HPP
#include "Systems/Cameras/Data/cmraData.hpp"
#endif
#ifndef API3D_SCALE_HPP
#include "Tool/api3d/api3dScale.hpp"
#endif
#ifndef MNM_OBJECT_HPP
#include "Support/mnm/mnmObject.hpp"
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
#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif

#include <string>


//============================================================================
//	forward references
//============================================================================
class api3dObject;
class cmraIconObject;
class prtyNumericUpDownUIInfo;

//============================================================================
//============================================================================
class cmraCameraObject : public mnmObject, 
						 public nameObject, 
						 public prtyObject,
						 public camCamera::CameraChangedCallback,
						 public api3dScaleInterest
{
	public:
		//--------------------------------------------------------------------
		// Constructor - set perspective or orthographic camera here.
		//	If i_bUseEditorCamera is true, this object attaches to the
		//	cam3dMgr editor camera.
		//--------------------------------------------------------------------
		cmraCameraObject(bool i_bOrthographic,
						 bool i_bUseEditorCamera = false);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~cmraCameraObject();

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
		// Get values as a camera data structure
		//--------------------------------------------------------------------
		const cmraCameraData& GetData() const;

		//--------------------------------------------------------------------
		// Set from camera data structure
		//--------------------------------------------------------------------
		void SetData(const cmraCameraData &i_Data);

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
		virtual const maAxisBox& GetWorldBox() const;

		//--------------------------------------------------------------------
		//	GetLocalBox returns a box which would enclose the cmraCameraObject
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

		//--------------------------------------------------------------------
		//	RayPick returns true if the given ray intersects the cmraCameraObject.
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
		//	Renderable sets whether the cmraCameraObject can be selected.
		//--------------------------------------------------------------------
		void SetRenderable(bool i_Renderable);

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
		//virtual bool IsOperationEnabled(Operations i_Operation, float i_Time);

		//--------------------------------------------------------------------
		// Get parent object of this object in order to define relationships
		//	between icons and their affected objects.
		//--------------------------------------------------------------------
		virtual pick3dPickObject* GetParentObject() const;
		virtual void SetParentObject(pick3dPickObject* i_pParent);
		
		//--------------------------------------------------------------------
		// Accessor to camera
		//--------------------------------------------------------------------
		const camCamera& GetCamera() const;
		camCamera& Camera();
		shared_ptr<camCamera> GetCameraPtr();

	private:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		api3dObject* create_line();

		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void NameChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void TransformChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void FOVChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void OrthoWidthChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void DescriptionChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void DOFChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void CameraChanged(prtyProperty *i_pProperty, bool i_bDirty);
		//void AspectRatioChanged(prtyProperty *i_pProperty, bool i_bDirty);
		//void MatchAspectChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void PitchYawChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void ManipModeChanged(prtyProperty *i_pProperty, bool i_bDirty);

		//--------------------------------------------------------------------
		// This callback is for when the manipulator changes the camera.
		// Set the new values from the camera into the position and
		// target properties.
		//--------------------------------------------------------------------
		virtual void CameraChanged(const camCamera*);

		//--------------------------------------------------------------------
		// update world box based on manip mode
		//--------------------------------------------------------------------
		void update_world_box();

	private:
		cmraCameraData	m_Data;

		// Two properties that are not animatable or saved to file,
		// these can be derived from position and target
		prtyFloat		m_Pitch;		// degrees pitch
		prtyFloat		m_Yaw;			// degrees yaw
		// this is a temporary mode for what is the center of the
		//	manipulation
		prtyEnum		m_ManipMode;

		// Checkbox enables/disables aspect ratio control
		//prtyNumericUpDownUIInfo* m_pAspectRatioControl;

		// 3D icon/geometry info
		cmraIconObject*	m_pObject;
		api3dObject*	m_pTargetObject;
		pick3dPickObject* m_pParent;

		maAxisBox m_WorldBox;
		bool m_bRenderable, m_bLayerVisible;

		// Camera being controlled
		shared_ptr<camCamera>	m_Camera;
};

