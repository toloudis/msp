/*****************************************************************************
**  chtrObject.hpp
**
**      A chtrObject is a derived class for displaying an object's position.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef CHTR_OBJECT_HPP
#error chtrObject.hpp multiply included
#endif
#define CHTR_OBJECT_HPP

#ifndef CHTR_DATA_HPP
#include "Systems/Character/Data/chtrData.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
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
#ifndef PRTY_ENUM_HPP
#include "Core/prty/prtyEnum.hpp"
#endif
#ifndef XFRM_TRANSFORMNODECALLBACK_HPP
#include "Support/xfrm/xfrmTransformNodeCallback.hpp"
#endif 


//============================================================================
//	forward references
//============================================================================
class api3dObject;
class api3dObjectEntity;
class smdlSubdivCharacter;
class gpxCharacter;
class gpxSceneObject;


//============================================================================
//============================================================================
class chtrObject :	public mnmObject, 
					public cmmNamedPropertyObject,
					public xfrmTransformNodeCallback
{
	public:
		//--------------------------------------------------------------------
		// ownership for the api3dObjectEntity passes to this object
		//--------------------------------------------------------------------
		chtrObject(api3dObjectEntity* i_pObject,
				   const fsLocator &i_ModelFile);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~chtrObject();

	//============================================================================
	//	Overloads
	//============================================================================

		//--------------------------------------------------------------------
		// Get the name of the object.
		//--------------------------------------------------------------------
		virtual std::string GetDisplayName() const;

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
	//	members
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
		// UpdateName() is called when the gui sets the name of the object,
		//	derived classes can set dirty bits and do "undo" operations, etc.
		// The default behavior calls SetName()
		//--------------------------------------------------------------------
		virtual void UpdateName(const std::string& i_Name);
		void SetName(const nameString& i_Name);

		//--------------------------------------------------------------------
		// UpdateFilename() is called when the gui sets the filename of the object,
		//	derived classes can set dirty bits and do "undo" operations, etc.
		// The default behavior calls SetFilename()
		//--------------------------------------------------------------------
		//virtual void UpdateFilename(const itString& i_Filename);
		//void SetFilename(const itString& i_Filename);
		//void SetFilenameWithoutNotify(const itString& i_Filename);

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
		//	Set/GetVisible comes from channel controlling visibility 
		//	of geometry
		//--------------------------------------------------------------------
		//void SetVisible(bool i_bVisible);
		//bool GetVisible() const;

		//--------------------------------------------------------------------
		// Set object active state in the render layer
		//--------------------------------------------------------------------
		void SetActiveRenderLayer(bool i_bActive);

		//--------------------------------------------------------------------
		//  Changes visible state of character based on GUI
		//--------------------------------------------------------------------
		void  SetEditorVisible(bool i_bVisible);
		bool GetEditorVisible() const;

		//--------------------------------------------------------------------
		//	LayerVisible represents if objects in the layer are visible
		//--------------------------------------------------------------------
		void SetLayerVisible(bool i_bVisible);

		//--------------------------------------------------------------------
		//	Wireframe represents if the objects are rendered
		//	in a wireframe style.
		//--------------------------------------------------------------------
		void SetWireframe(bool i_bWireframe);

		//--------------------------------------------------------------------
		//	LowRes represents if the objects are rendered
		//	using a low resolution model.
		//--------------------------------------------------------------------
		void SetLowRes(bool i_bLowRes);

		//--------------------------------------------------------------------
		// Flags for how object can be modified
		//--------------------------------------------------------------------
		virtual RotateFlags	GetRotateFlags();
		virtual ScaleFlags	GetScaleFlags();
		virtual TranslateFlags GetTranslateFlags();

		//--------------------------------------------------------------------
		//	entity
		//--------------------------------------------------------------------
		api3dObjectEntity * GetEntity();

		//--------------------------------------------------------------------
		//	Get reference for given name.  The returned pointer is owned
		//	by this object.  The object should be retained by the caller
		//	to avoid repeated string searches.
		//--------------------------------------------------------------------
		api3dReference* GetReference(const char* i_Name);

		//--------------------------------------------------------------------
		// Get list of references for possible attachment within this object.
		//--------------------------------------------------------------------
		void GetReferenceList(std::vector<std::string> &o_List);

		//--------------------------------------------------------------------
		//	filename
		//--------------------------------------------------------------------
		//void GetFilename( itString& o_Filename ) const;

		//--------------------------------------------------------------------
		//	Directory
		//--------------------------------------------------------------------
		//void SetDirectory( const fsLocator& i_Dir );
		//void GetDirectory( fsLocator& o_Dir ) const;

		//--------------------------------------------------------------------
		// Set subdivision level being used.
		//--------------------------------------------------------------------
		void SetSubdivLevel(int i_SubdivLevel);
		int GetSubdivLevel() const; 

		//--------------------------------------------------------------------
		// Get parent object of this object in order to define relationships
		//	between icons and their affected objects.
		//--------------------------------------------------------------------
		//virtual sel3dObject* GetParentObject() const;
		//virtual void SetParentObject(sel3dObject* i_pParent);

	//============================================================================
	//	Data
	//============================================================================

		//--------------------------------------------------------------------
		// Get values as a data structure
		//--------------------------------------------------------------------
		const chtrData& GetData() const;

		//--------------------------------------------------------------------
		// Set from data structure
		//--------------------------------------------------------------------
		void SetData(const chtrData &i_Data);

		//--------------------------------------------------------------------
		// GetSubdivCharacter()
		//--------------------------------------------------------------------
		smdlSubdivCharacter* GetSubdivCharacter();

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
		void PositionChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void OrientationChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void ScaleChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void PivotChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void PivotCompensationChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void NameChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void VisibleChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void FilenameChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void JointDisplayChanged(prtyProperty *i_pProperty, bool i_bDirty);

	private:
		api3dObjectEntity *		m_p3DObject;
		gpxSceneObject*			m_p3DObjectProxy;
		smdlSubdivCharacter*	m_pCharacter;
		gpxCharacter*			m_pCharacterProxy;
		fsLocator				m_ModelFileLoaded;

		//sel3dObject* m_pParent;

		chtrData	m_Data;
		maAxisBox	m_LocalBox;
		//fsLocator	m_Directory;
		bool		m_bLayerVisible;

		// this is a temporary property for how to display joints
		prtyEnum		m_JointDisplay;
};

