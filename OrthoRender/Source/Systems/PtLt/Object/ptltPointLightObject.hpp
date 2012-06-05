/*****************************************************************************
**  ptltPointLightObject.hpp
**
**      A ptltPointLightObject is base class for objects that can be positioned
**  by manipulation modes.
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef PTLT_POINTLIGHTOBJECT_HPP
#error ptltPointLightObject.hpp multiply included
#endif
#define PTLT_POINTLIGHTOBJECT_HPP

#ifndef PTLT_DATA_HPP
#include "Systems/PtLt/Data/ptltData.hpp"
#endif

#ifndef NAME_OBJECT_HPP
#include "Core/name/nameObject.hpp"
#endif
#ifndef MNM_OBJECT_HPP
#include "Support/mnm/mnmObject.hpp"
#endif

#ifndef API3D_OBJECT_HPP
#include "Tool/api3d/api3dObject.hpp"
#endif
#ifndef API3D_SCALE_HPP
#include "Tool/api3d/api3dScale.hpp"
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
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
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


//============================================================================
//============================================================================
class ptltPointLightObject : 
	public mnmObject, 
	public nameObject, 
	public prtyObject,
	public cmmFalloffAspect,
	public api3dScaleInterest
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
		virtual std::string GetPick3dName() const;

		//--------------------------------------------------------------------
		//	GlobalScaleChanged - notification that global scale has changed.
		//--------------------------------------------------------------------
		virtual void GlobalScaleChanged( float i_Scale );


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


	//============================================================================
	//	3D icon/geometry
	//============================================================================

		//--------------------------------------------------------------------
		//	GetWorldBox returns a box which completely encloses the mnmObject.
		//--------------------------------------------------------------------
		virtual const maAxisBox& GetWorldBox() const;

		//--------------------------------------------------------------------
		//	GetLocalBox returns a box which would enclose the ptltPointLightObject
		//	if its transformations were identity
		//--------------------------------------------------------------------
		virtual const maAxisBox& GetLocalBox() const;

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

		//--------------------------------------------------------------------
		//	RayPick returns true if the given ray intersects the ptltPointLightObject.
		//	If it does, the t value is also returned in o_T.
		//--------------------------------------------------------------------
		virtual bool RayPick(	const maPoint3d& i_RayStart,
								const maPoint3d& i_RayEnd,
								float& o_T);

		//----------------------------------------------------------------------------
		// Find the object that matches the pick code from an earlier pick render.
		//----------------------------------------------------------------------------
		virtual bool MatchPickCode(envType::UInt32 i_PickCode) const;

		//--------------------------------------------------------------------
		//	Renderable sets whether the icon is visible
		//--------------------------------------------------------------------
		void SetRenderable(bool i_Renderable);

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
		// Get parent object of this object in order to define relationships
		//	between icons and their affected objects.
		//--------------------------------------------------------------------
		virtual pick3dPickObject* GetParentObject() const;
		void SetParentObject(pick3dPickObject* i_pPO);

		//--------------------------------------------------------------------
		// For polling if an manipulation operation is currently enabled
		//--------------------------------------------------------------------
		virtual bool IsOperationEnabled(Operations i_Operation, float i_Time);

		//--------------------------------------------------------------------
		//	Get reference for given name.  The returned pointer is owned
		//	by this object.  The object should be retained by the caller
		//	to avoid repeated string searches.
		//--------------------------------------------------------------------
		virtual api3dReference* GetReference(const char* i_Name);

		//--------------------------------------------------------------------
		// Get list of references for possible attachment within this object.
		//--------------------------------------------------------------------
		virtual void GetReferenceList(std::vector<std::string> &o_List);

	private:
		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void NameChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void TransformChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void LightChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void FalloffChanged(prtyProperty *i_pProperty, bool i_bDirty);

		//--------------------------------------------------------------------
		//	Callback from falloff aspect, update the falloff icon
		//--------------------------------------------------------------------
		virtual void UpdateFalloffIcon();

		api3dObject*		m_pObject;		// 3D icon/geometry info
		g3dPointLight*		m_pLight;		// actual light
		pick3dPickObject*	m_pParent;
		ptltRangeIcon*		m_pRangeIcon;

		maAxisBox			m_WorldBox;
		bool				m_bRenderable;
		bool				m_bLayerVisible;

		ptltData			m_Data;
};


