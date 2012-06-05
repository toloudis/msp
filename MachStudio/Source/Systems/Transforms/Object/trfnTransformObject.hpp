/*****************************************************************************
**  trfnTransformObject.hpp
**
**      A trfnTransformObject is the property object for a transform
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef TRFN_TRANSFORMOBJECT_HPP
#error trfnTransformObject.hpp multiply included
#endif
#define TRFN_TRANSFORMOBJECT_HPP

#ifndef TRFN_DATA_HPP
#include "Systems/Transforms/Data/trfnData.hpp"
#endif
#ifndef MNM_OBJECT_HPP
#include "Support/mnm/mnmObject.hpp"
#endif
#ifndef CMM_NAMEDPROPERTYOBJECT_HPP
#include "Systems/Common/Object/cmmNamedPropertyObject.hpp"
#endif 
#ifndef XFRM_TRANSFORMNODECALLBACK_HPP
#include "Support/xfrm/xfrmTransformNodeCallback.hpp"
#endif 

#include <vector>

//============================================================================
//	forward references
//============================================================================
class fsResourceTrackerData;
class nameString;
class api3dObjectNode;
class gpxSceneObject;
class xfrmTransformGroup;

//============================================================================
//============================================================================
class trfnTransformObject : public mnmObject, 
							public cmmNamedPropertyObject,
							public xfrmTransformNodeCallback
{
public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		trfnTransformObject();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~trfnTransformObject();

	//============================================================================
	//	Overloads
	//============================================================================

		//--------------------------------------------------------------------
		// Get the name of the object.
		//--------------------------------------------------------------------
		virtual std::string GetDisplayName() const;

		//--------------------------------------------------------------------
		// Get parent object of this object in order to define relationships
		//	between icons and their affected objects.
		//--------------------------------------------------------------------
		//virtual sel3dObject* GetParentObject() const;
		//void SetParentObject(sel3dObject* i_pPO);

		//--------------------------------------------------------------------
		//  get a list of resources.  the resources will be appended to the
		//	passed in list.
		//--------------------------------------------------------------------
		virtual void GetResourceList( fsResourceTrackerData& io_List );

	//============================================================================
	//	Data
	//============================================================================

		//--------------------------------------------------------------------
		// Get values as a transform data structure
		//--------------------------------------------------------------------
		const trfnData& GetData() const;

		//--------------------------------------------------------------------
		// Set from transform data structure
		//--------------------------------------------------------------------
		void SetData(const trfnData &i_Data);


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
		// Orientation property access
		//--------------------------------------------------------------------
		prtyRotation&	PropertyOrientation();
		const prtyRotation&	GetPropertyOrientation() const;

		//--------------------------------------------------------------------
		// Scale property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyScale();
		const prtyFloat&	GetPropertyScale() const;

		//--------------------------------------------------------------------
		// Visible property access
		//--------------------------------------------------------------------
		prtyBoolean&	PropertyVisible();
		const prtyBoolean&	GetPropertyVisible() const;


	//============================================================================
	//	mnmObject implementation
	//============================================================================

		//--------------------------------------------------------------------
		//	GetWorldBox returns a box which completely encloses the
		//	chtrObject.
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
		// Change the pivot point of this object to the center 
		// of its bounding box
		//--------------------------------------------------------------------
		virtual void CenterPivot();

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
		// Flags for how object can be modified
		//--------------------------------------------------------------------
		virtual RotateFlags	GetRotateFlags();
		virtual ScaleFlags	GetScaleFlags();
		virtual TranslateFlags GetTranslateFlags();


	//============================================================================
	//	attributes
	//============================================================================

		//--------------------------------------------------------------------
		//  Changes visible state of node and its children based on GUI
		//--------------------------------------------------------------------
		void SetEditorVisible(bool i_bVisible);
		//bool GetEditorVisible() const;

		//--------------------------------------------------------------------
		// SetName
		//--------------------------------------------------------------------
		void SetName(const nameString& i_Name);

	private:
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
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void NameChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void PositionChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void OrientationChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void ScaleChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void PivotChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void PivotCompensationChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void InheritsTransformChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void VisibleChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void PickableChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void WireframeChanged(prtyProperty *i_pProperty, bool i_bDirty);

		api3dObjectNode *		m_p3DNode;
		gpxSceneObject*			m_p3DNodeProxy;

		mutable trfnData		m_Data;
		xfrmTransformGroup*		m_pTransform;

		sel3dObject*	m_pParent;
};


