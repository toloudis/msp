/*****************************************************************************
**  chtrObject.hpp
**
**      A chtrObject is a derived class for displaying an object's position.
**
**	Extra Large Technology
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
#ifndef NAME_OBJECT_HPP
#include "Core/name/nameObject.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif
#ifndef PRTY_ENUM_HPP
#include "Core/prty/prtyEnum.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class api3dObject;
class api3dObjectEntity;
class smdlSubdivCharacter;


//============================================================================
//============================================================================
class chtrObject : public mnmObject, public nameObject, public prtyObject
{
	public:
		//--------------------------------------------------------------------
		// ownership for the api3dObjectEntity passes to this object
		//--------------------------------------------------------------------
		chtrObject(api3dObjectEntity* i_pObject, 
				   const fsLocator& i_Dir );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~chtrObject();

	//============================================================================
	//	Overloads
	//============================================================================

		//--------------------------------------------------------------------
		// Get the name of the object.
		//--------------------------------------------------------------------
		virtual std::string GetPick3dName() const;

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
		virtual const maAxisBox& GetWorldBox() const;

		//--------------------------------------------------------------------
		//	GetLocalBox returns a box which would enclose the chtrObject
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
		void SetFilename(const itString& i_Filename);
		void SetFilenameWithoutNotify(const itString& i_Filename);

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
		//	RayPick returns true if the given ray intersects the chtrObject.
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
		//	Set/GetVisible comes from channel controlling visibility 
		//	of geometry
		//--------------------------------------------------------------------
		void SetVisible(bool i_bVisible);
		bool GetVisible() const;

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
		void GetFilename( itString& o_Filename ) const;

		//--------------------------------------------------------------------
		//	Directory
		//--------------------------------------------------------------------
		void SetDirectory( const fsLocator& i_Dir );
		void GetDirectory( fsLocator& o_Dir ) const;

		//--------------------------------------------------------------------
		// Set subdivision level being used.
		//--------------------------------------------------------------------
		void SetSubdivLevel(int i_SubdivLevel);
		int GetSubdivLevel() const; 

		//--------------------------------------------------------------------
		// Get parent object of this object in order to define relationships
		//	between icons and their affected objects.
		//--------------------------------------------------------------------
		virtual pick3dPickObject* GetParentObject() const;
		virtual void SetParentObject(pick3dPickObject* i_pParent);

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

	private:
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
		smdlSubdivCharacter*	m_pCharacter;

		pick3dPickObject* m_pParent;

		chtrData	m_Data;
		maAxisBox	m_LocalBox;
		fsLocator	m_Directory;
		bool		m_bLayerVisible;

		// this is a temporary property for how to display joints
		prtyEnum		m_JointDisplay;
};

