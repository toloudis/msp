/*****************************************************************************
**  ptltPointLightObject.hpp
**
**      A ptltPointLightObject is base class for objects that can be positioned
**  by manipulation modes.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef PTLT_POINTLIGHTOBJECT_HPP
#error ptltPointLightObject.hpp multiply included
#endif
#define PTLT_POINTLIGHTOBJECT_HPP

#ifndef PTLT_DATA_HPP
#include "Systems/PtLt/Data/ptltData.hpp"
#endif

#ifndef CMM_NAMEDPROPERTYOBJECT_HPP
#include "Systems/Common/Object/cmmNamedPropertyObject.hpp"
#endif 
#ifndef MNM_OBJECT_HPP
#include "Support/mnm/mnmObject.hpp"
#endif

#ifndef API3D_OBJECT_HPP
#include "Tool/api3d/api3dObject.hpp"
#endif
#ifndef ICN_ICONSCALE_HPP
#include "Tool/icn/icnIconScale.hpp"
#endif
#ifndef G3D_POINTLIGHT_HPP
#include "Graphics/g3d/g3dPointLight.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif
#ifndef CMM_FALLOFFASPECT_HPP
#include "Systems/Common/Object/cmmFalloffAspect.hpp"
#endif
#ifndef LTST_ISOLATABLE_HPP
#include "Support/ltst/ltstIsolatable.hpp"
#endif 
#ifndef XFRM_TRANSFORMNODECALLBACK_HPP
#include "Support/xfrm/xfrmTransformNodeCallback.hpp"
#endif 


#include <vector>


//============================================================================
//	forward references
//============================================================================
class api3dReference;
class nameString;
class prtyVector3dEditUpDownUIInfo;
class prtyNumericUpDownUIInfo;
class ptltRangeIcon;
class gpxPointLight;
class icnIconSet;
class gpxIconSet;

//============================================================================
//============================================================================
class ptltPointLightObject : 
	public mnmObject, 
	public cmmNamedPropertyObject,
	public cmmFalloffAspect,
	public icnIconScaleInterest,
	public ltstIsolatable,
	public xfrmTransformNodeCallback
{
public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		ptltPointLightObject();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~ptltPointLightObject();

	//============================================================================
	//	Overloads
	//============================================================================

		//--------------------------------------------------------------------
		// Get the name of the object.
		//--------------------------------------------------------------------
		virtual std::string GetDisplayName() const;

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
		// Get values as a light  data structure
		//--------------------------------------------------------------------
		const ptltData& GetData() const;

		//--------------------------------------------------------------------
		// Set from light  data structure
		//--------------------------------------------------------------------
		void SetData(const ptltData &i_Data);


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
		// ShadowSource property access
		//--------------------------------------------------------------------
		prtyBoolean&	PropertyShadowSource();
		const prtyBoolean&	GetPropertyShadowSource() const;

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


	//============================================================================
	//	3D icon/geometry
	//============================================================================

		//--------------------------------------------------------------------
		//	GetWorldBox returns a box which completely encloses the mnmObject.
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
		// SetName
		//--------------------------------------------------------------------
		void SetName(const nameString& i_Name);

		//--------------------------------------------------------------------
		//	Position
		//--------------------------------------------------------------------
		virtual maPoint3d GetPosition() const;
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
		virtual bool MatchPickCode(envType::UInt32 i_PickCode) const;

		//--------------------------------------------------------------------
		//	Renderable sets whether the icon is visible
		//--------------------------------------------------------------------
		void SetRenderable(bool i_Renderable);

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
		//--------------------------------------------------------------------
		g3dPointLight* GetLight();

		//--------------------------------------------------------------------
		// Return this light's range icon
		//--------------------------------------------------------------------
		ptltRangeIcon* GetRangeIcon();

		//--------------------------------------------------------------------
		// Get parent object of this object in order to define relationships
		//	between icons and their affected objects.
		//--------------------------------------------------------------------
		//virtual sel3dObject* GetParentObject() const;
		//void SetParentObject(sel3dObject* i_pPO);

		//--------------------------------------------------------------------
		// For polling if an manipulation operation is currently enabled
		//--------------------------------------------------------------------
		virtual bool IsOperationEnabled(Operations i_Operation, const maTime& i_Time);


	private:
		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void NameChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void TransformChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void LightChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void FalloffChanged(prtyProperty *i_pProperty, bool i_bDirty);

		//--------------------------------------------------------------------
		//	IsolationChanged - notification that this object's isolation
		//	state has changed (fading on or out the light's intensity)
		//--------------------------------------------------------------------
		virtual void IsolationChanged();

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
		// compute world position of light using parent transformations
		//--------------------------------------------------------------------
		maPoint3d get_world_position() const;

		icnIconSet*			m_pObject;		// 3D icon/geometry info	
		gpxIconSet*			m_pObjectProxy;	// thread-safe proxy to object	
		g3dPointLight*		m_pPointLight;	// actual light
		gpxPointLight*		m_pLightProxy;	// thread-safe proxy to light		
		sel3dObject*		m_pParent;
		ptltRangeIcon*		m_pRangeIcon;

		//maAxisBox			m_WorldBox;
		bool				m_bRenderable;
		bool				m_bLayerVisible;
		bool				m_bLayerPickable;

		ptltData			m_Data;
};


