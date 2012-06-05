/*****************************************************************************
**  chtrObject.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/Object/chtrObject.hpp"

//	Current System
#include "Systems/Character/Data/chtrDocumentChunk.hpp"
#include "Systems/Character/GUI/chtrDialogDataUtil.hpp"
#include "Systems/Character/Undo/chtrOperations.hpp"

//	Application
#include "Support/evmt/evmtEnvironmentMgr.hpp"
#include "Support/ltst/ltstLightSetMgr.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mnm/mnmThinkMgr.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"

// library
#include "Core/fs/fsFileUtil.hpp"
#include "Core/geo/geoRayIntersection.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"
#include "Graphics/smdl/smdlSubdivCharacter.hpp"
#include "Tool/api3d/api3dObjectEntity.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Tool/api3d/api3dSubdiv.hpp"

#include <boost/bind.hpp>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace
{
	enum JointDisplay
	{
		e_ModelOnly = 0,
		e_JointsOnly,
		e_JointsAndModel
	};
}

//----------------------------------------------------------------------------
// ownership for the api3dObject passes to this object
//----------------------------------------------------------------------------
chtrObject::chtrObject( api3dObjectEntity* i_pObject, 
						const fsLocator& i_Dir )
:	m_pCharacter(NULL),
	m_bLayerVisible(true),
	m_JointDisplay("Joint Display", e_ModelOnly)
{
	// Set up enumeration for joint display
	m_JointDisplay.SetEnumTag(e_ModelOnly, "Model Only");
	m_JointDisplay.SetEnumTag(e_JointsOnly, "Joints Only");
	m_JointDisplay.SetEnumTag(e_JointsAndModel, "Joints And Model");

	// If this is non-NULL, then we have a subdivision character
	m_pCharacter = dynamic_cast<smdlSubdivCharacter*>(i_pObject->Object());

	//m_p3DObject = pCharacterEnt;
	m_p3DObject = i_pObject;
	m_LocalBox = m_p3DObject->GetWorldBox();

	ltstLightSetMgr::AddObject(this, m_p3DObject);
	evmtEnvironmentMgr::AddObject(this, m_p3DObject);
	
	SetDirectory( i_Dir );
	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_Position), "Transform", "Position of the object");
	AddProperty( pPUII );
	prtyVector3dEditUpDownUIInfo *pPVEUDUII  = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_Orientation), "Transform", "Orientation of the object");
	pPVEUDUII->SetIncrement(0.5f, 0.5f, 0.5f);
	pPVEUDUII->SetDecimalPlaces(1);
	AddProperty( pPVEUDUII );		
	pPUII  = new prtyFloatEditUIInfo(&(m_Data.m_Scale), "Transform", "Scale of the object");
	AddProperty( pPUII );
	pPUII = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_PivotPoint), "Transform", "Pivot position in local space");
	AddProperty( pPUII );
	prtyTextBoxUIInfo* pTBUII;
	pTBUII = new prtyTextBoxUIInfo(&(m_Data.m_Name), "Asset", "Name of the object");
	pTBUII->SetMaxWidth( 100 );
	AddProperty( pTBUII );
	prtyFileChooserUIInfo* pFCUII = new prtyFileChooserUIInfo(&(m_Data.m_Filename), "Asset", "File Name of the object");
	pFCUII->SetInitialDirectory(i_Dir);
	AddProperty( pFCUII );
	// Visibility is the combination of multiple flags in the user interface.
	//AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bEditorVisible), "Editor", "Is the object visible in the editor") );
	pPUII = new prtyComboBoxUIInfo(&(m_JointDisplay), "Editor", "Display of joints");
	AddProperty( pPUII );

	// Register callbacks to update geometry
	// when properties change
	m_Data.m_Position.AddCallback(new prtyCallbackWrapper<chtrObject>(this, &chtrObject::PositionChanged));
	m_Data.m_Orientation.AddCallback(new prtyCallbackWrapper<chtrObject>(this, &chtrObject::OrientationChanged));
	m_Data.m_Scale.AddCallback(new prtyCallbackWrapper<chtrObject>(this, &chtrObject::ScaleChanged));
	m_Data.m_PivotPoint.AddCallback(new prtyCallbackWrapper<chtrObject>(this, &chtrObject::PivotChanged));
	m_Data.m_PivotCompensation.AddCallback(new prtyCallbackWrapper<chtrObject>(this, &chtrObject::PivotCompensationChanged));
	m_Data.m_Name.AddCallback(new prtyCallbackWrapper<chtrObject>(this, &chtrObject::NameChanged));
	m_Data.m_bVisible.AddCallback(new prtyCallbackWrapper<chtrObject>(this, &chtrObject::VisibleChanged));
	m_Data.m_Filename.AddCallback(new prtyCallbackWrapper<chtrObject>(this, &chtrObject::FilenameChanged));	
	
	// Properties just for the interface
	m_JointDisplay.AddCallback(new prtyCallbackWrapper<chtrObject>(this, &chtrObject::JointDisplayChanged));

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chtrObject::~chtrObject()
{
	//DBG_LOG0("In chtrObject destructor");

	//Note: m_pCharacter is not owned, just a view into this object's model

	if ( m_p3DObject )
	{
		ltstLightSetMgr::RemoveObject(this, m_p3DObject);
		evmtEnvironmentMgr::RemoveObject(this, m_p3DObject);

		api3dScene::RemoveObject(m_p3DObject);
		delete m_p3DObject;
	}
}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string chtrObject::GetPick3dName() const
{
	return this->GetName().GetString();
}

//--------------------------------------------------------------------
// Name property access
//--------------------------------------------------------------------
prtyName&	chtrObject::PropertyName()
{
	return m_Data.m_Name;
}
const prtyName&	chtrObject::GetPropertyName() const
{
	return m_Data.m_Name;
}

//--------------------------------------------------------------------
// Position property access
//--------------------------------------------------------------------
prtyPoint3d&	chtrObject::PropertyPosition()
{
	return m_Data.m_Position;
}
const prtyPoint3d&	chtrObject::GetPropertyPosition() const
{
	return m_Data.m_Position;
}

//--------------------------------------------------------------------
// Orientation property access
//--------------------------------------------------------------------
prtyRotation&	chtrObject::PropertyOrientation()
{
	return m_Data.m_Orientation;
}
const prtyRotation&	chtrObject::GetPropertyOrientation() const
{
	return m_Data.m_Orientation;
}

//--------------------------------------------------------------------
// Scale property access
//--------------------------------------------------------------------
prtyFloat&	chtrObject::PropertyScale()
{
	return m_Data.m_Scale;
}
const prtyFloat&	chtrObject::GetPropertyScale() const
{
	return m_Data.m_Scale;
}

//--------------------------------------------------------------------
// Visible property access
//--------------------------------------------------------------------
prtyBoolean&	chtrObject::PropertyVisible()
{
	return m_Data.m_bVisible;
}
const prtyBoolean&	chtrObject::GetPropertyVisible() const
{
	return m_Data.m_bVisible;
}

//----------------------------------------------------------------------------
//	GetWorldBox returns a box which completely encloses the chtrObject.
//----------------------------------------------------------------------------
const maAxisBox& chtrObject::GetWorldBox() const
{
	//const maAxisBox box = m_p3DObject->GetWorldBox();
	//DBG_LOG3( "POMin( %8.3f, %8.3f, %8.3f )", box.GetMinX(), box.GetMinY(), box.GetMinZ() );
	//DBG_LOG3( "  Max( %8.3f, %8.3f, %8.3f )", box.GetMaxX(), box.GetMaxY(), box.GetMaxZ() );

	return m_p3DObject->GetWorldBox();
}

//----------------------------------------------------------------------------
//	GetLocalBox returns a box which would enclose the chtrObject
//	if its transformations were identity
//----------------------------------------------------------------------------
const maAxisBox& chtrObject::GetLocalBox() const
{
	return m_LocalBox;
}

//--------------------------------------------------------------------
//	GetWorldPivot returns the point in world space that this
//		object will rotate around. Used to center rotation and
//		scale compasses.
//--------------------------------------------------------------------
//virtual 
maPoint3d chtrObject::GetWorldPivot() const
{
	// pivot point in world space is just local pivot plus position
	return (m_p3DObject->GetPivotPoint() + this->GetPosition());
}

//--------------------------------------------------------------------
// UpdateName() is called when the gui sets the name of the object,
//	derived classes can set dirty bits and do "undo" operations, etc.
// The default behavior calls SetName()
//--------------------------------------------------------------------
void chtrObject::UpdateName(const std::string& i_Name)
{
	chtrOperations::ChangeName( i_Name );
}

//--------------------------------------------------------------------
//	Name
//--------------------------------------------------------------------
void chtrObject::SetName(const nameString& i_Name)
{
    // Set name first, which may change the ID number
    nameObject::SetName(i_Name);

    // Make sure that the data matches our true name (including
    // ID number)
    m_Data.m_Name = this->GetName();
}

//--------------------------------------------------------------------
// UpdateFilename() is called when the gui sets the filename of the object,
//	derived classes can set dirty bits and do "undo" operations, etc.
// The default behavior calls SetFilename()
//--------------------------------------------------------------------
//virtual 
//void chtrObject::UpdateFilename(const itString& i_Filename)
//{
//	chtrOperations::ChangeFilename(m_Data.m_Filename.GetValue());
//}
void chtrObject::SetFilename(const itString& i_Filename)
{
	this->m_Data.m_Filename = i_Filename;
}
void chtrObject::SetFilenameWithoutNotify(const itString& i_Filename)
{
	this->m_Data.m_Filename.SetValueWithoutNotify( i_Filename );
}

//----------------------------------------------------------------------------
//	Position
//----------------------------------------------------------------------------
maPoint3d chtrObject::GetPosition() const
{
	return m_Data.m_Position.GetValue();
}
void chtrObject::UpdatePosition(const maPoint3d& i_Position, 
								bool i_bNewOperation)
{
	//chtrOperations::ChangePosition( i_Position );
	m_Data.m_Position.SetValue(i_Position, 
		i_bNewOperation ? prtyProperty::eNewUndo : prtyProperty::eContinueUndo);
}


//----------------------------------------------------------------------------
//	Orientation
//----------------------------------------------------------------------------
maRotation chtrObject::GetOrientation() const
{
	return m_Data.m_Orientation.GetQuaternion();
}
void chtrObject::UpdateOrientation(const maRotation& i_Orientation, 
										bool i_bNewOperation)
{
	//chtrOperations::ChangeOrientation( i_Orientation );
	m_Data.m_Orientation.SetQuaternion(i_Orientation, 
		i_bNewOperation ? prtyProperty::eNewUndo : prtyProperty::eContinueUndo);
}

//----------------------------------------------------------------------------
//	Scale
//----------------------------------------------------------------------------
maPoint3d chtrObject::GetScale() const
{
	return m_p3DObject->GetScale();
}
void chtrObject::UpdateScale(const maPoint3d& i_Scale, 
								bool i_bNewOperation)
{
	// Uniform Scale, just take off the X value
	m_Data.m_Scale.SetValue(i_Scale.m_X, 
		i_bNewOperation ? prtyProperty::eNewUndo : prtyProperty::eContinueUndo);
}

//----------------------------------------------------------------------------
//	RayPick returns true if the given ray intersects the chtrObject.
//	If it does, the t value is also returned in o_T.
//----------------------------------------------------------------------------
bool chtrObject::RayPick(	const maPoint3d& i_RayStart,
							const maPoint3d& i_RayEnd,
							float& o_T)
{
	if (!m_p3DObject->GetRenderable())
		return false;

	const maAxisBox &box = this->GetWorldBox();

	//DBG_LOG3( "RPMin( %8.3f, %8.3f, %8.3f )", box.GetMinX(), box.GetMinY(), box.GetMinZ() );
	//DBG_LOG3( "  Max( %8.3f, %8.3f, %8.3f )", box.GetMaxX(), box.GetMaxY(), box.GetMaxZ() );

	return geoRayIntersection::IntersectLineBBox( i_RayStart,
												(i_RayEnd - i_RayStart),
												box.GetBoxPoint(7),
												box.GetBoxPoint(0),
												o_T );
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
bool chtrObject::MatchPickCode(envType::UInt32 i_PickCode) const
{
	if (!m_p3DObject->GetRenderable())
		return false;

	return (m_p3DObject->ContainsPickCode(i_PickCode));
}

//--------------------------------------------------------------------
//  Changes visible state of character based on GUI
//--------------------------------------------------------------------
void  chtrObject::SetEditorVisible(bool i_bVisible)
{
	m_Data.m_bEditorVisible.SetValue(i_bVisible);

	// object is visible only if channel and gui and layer are all
	// are set visible == true;
	m_p3DObject->SetRenderable(m_Data.m_bVisible.GetValue() 
							&& m_Data.m_bEditorVisible.GetValue() 
							&& m_bLayerVisible);
}
bool chtrObject::GetEditorVisible() const
{
	return m_Data.m_bEditorVisible.GetValue();
}

//--------------------------------------------------------------------
//	LayerVisible represents if objects in the layer are visible
//--------------------------------------------------------------------
void chtrObject::SetLayerVisible(bool i_bVisible)
{
	m_bLayerVisible = i_bVisible;

	// object is visible only if channel and gui and layer are all
	// are set visible == true;
	m_p3DObject->SetRenderable(m_Data.m_bVisible.GetValue() 
							&& m_Data.m_bEditorVisible.GetValue() 
							&& i_bVisible);
}

//--------------------------------------------------------------------
//	Wireframe represents if the objects are rendered
//	in a wireframe style.
//--------------------------------------------------------------------
void chtrObject::SetWireframe(bool i_bWireframe)
{
	m_p3DObject->SetWireframe(i_bWireframe);
}

//--------------------------------------------------------------------
//	LowRes represents if the objects are rendered
//	using a low resolution model.
//--------------------------------------------------------------------
void chtrObject::SetLowRes(bool i_bLowRes)
{
	m_p3DObject->SetLowResolution(i_bLowRes);
}

//----------------------------------------------------------------------------
// Flags for how object can be modified
//----------------------------------------------------------------------------
mnmObject::RotateFlags	chtrObject::GetRotateFlags()
{
	return mnmObject::e_RotateAll;
}
mnmObject::ScaleFlags	chtrObject::GetScaleFlags()
{
	return mnmObject::e_ScaleUniform;
}
mnmObject::TranslateFlags	chtrObject::GetTranslateFlags()
{
	return mnmObject::e_TranslateAll;
}

//--------------------------------------------------------------------
//	entity
//--------------------------------------------------------------------
api3dObjectEntity * chtrObject::GetEntity()
{
	return this->m_p3DObject;
}

//--------------------------------------------------------------------
//	Get reference for given name.  The returned pointer is owned
//	by this object.  The object should be retained by the caller
//	to avoid repeated string searches.
//--------------------------------------------------------------------
api3dReference* chtrObject::GetReference(const char* i_Name)
{
	// See if object has reference
	api3dReference* pRef = m_p3DObject->GetReference(i_Name);
	if (pRef) return pRef;
	// if , return default refence to root
	return mnmObject::GetReference(i_Name);
}

//--------------------------------------------------------------------
// Get list of references for possible attachment within this object.
//--------------------------------------------------------------------
void chtrObject::GetReferenceList(std::vector<std::string> &o_List)
{
	m_p3DObject->GetReferenceList(o_List);
}

//--------------------------------------------------------------------
//	filename
//--------------------------------------------------------------------
//virtual
void chtrObject::GetFilename( itString& o_Filename ) const
{
	o_Filename = m_Data.m_Filename.GetValue();
}

//--------------------------------------------------------------------
//	Directory
//--------------------------------------------------------------------
//virtual 
void chtrObject::SetDirectory( const fsLocator& i_Dir )
{
	m_Directory = i_Dir;
}
//virtual 
void chtrObject::GetDirectory( fsLocator& o_Dir ) const
{
	o_Dir = m_Directory;
}

//--------------------------------------------------------------------
// Set subdivision level being used.
//--------------------------------------------------------------------
void chtrObject::SetSubdivLevel(int i_SubdivLevel)
{
	if (m_pCharacter)
	{
		m_pCharacter->SetCurrentSubdivLevel(i_SubdivLevel);
	}	
}
int chtrObject::GetSubdivLevel() const
{
	if (m_pCharacter)
	{
		return m_pCharacter->GetCurrentSubdivLevel();
	}	
	return 0;
}

//----------------------------------------------------------------------------
// Get parent object of the spline in order for selection to trace from
//	this spline back to the controlled object.
//----------------------------------------------------------------------------
//virtual 
pick3dPickObject* chtrObject::GetParentObject() const
{
	return m_pParent;
}
//virtual 
void chtrObject::SetParentObject(pick3dPickObject* i_pParent)
{
	m_pParent = i_pParent;
}

//--------------------------------------------------------------------
// Get values as a data structure
//--------------------------------------------------------------------
const chtrData& chtrObject::GetData() const
{
	return m_Data;
}

//--------------------------------------------------------------------
// Set from data structure
//--------------------------------------------------------------------
void chtrObject::SetData(const chtrData &i_Data)
{
	m_Data = i_Data;
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void chtrObject::PositionChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	const maPoint3d& position = m_Data.m_Position.GetValue();
	m_p3DObject->SetPosition( position );

	if (i_bDirty)
		chtrDocumentChunk::ActiveDataChanged();
}
void chtrObject::OrientationChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	const maRotation& orientation = m_Data.m_Orientation.GetQuaternion();
	m_p3DObject->SetOrientation( orientation );

	if (i_bDirty)
		chtrDocumentChunk::ActiveDataChanged();
}
void chtrObject::ScaleChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	m_p3DObject->SetUniformScale( m_Data.m_Scale.GetValue() );

	if (i_bDirty)
		chtrDocumentChunk::ActiveDataChanged();
}
void chtrObject::PivotChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	const maPoint3d& pivot = m_Data.m_PivotPoint.GetValue();
	if (i_bDirty)
	{
		const bool preserve_transformation = true;	// preserve transformation if coming from GUI
		m_p3DObject->SetPivotPoint( pivot, preserve_transformation );

		// When preserving transformation, the "pivot compensation" value will be updated.
		// So, we need to set that value into our data also
		m_Data.m_PivotCompensation = m_p3DObject->GetPivotCompensation();

		chtrDocumentChunk::ActiveDataChanged();
	}
	else
	{
		const bool dont_preserve_transformation = false;	// preserve transformation if coming from GUI
		m_p3DObject->SetPivotPoint( pivot, dont_preserve_transformation );
	}
}
void chtrObject::PivotCompensationChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	const maVector3d& vec = m_Data.m_PivotCompensation.GetValue();
	m_p3DObject->SetPivotCompensation( vec );

	if (i_bDirty)
		chtrDocumentChunk::ActiveDataChanged();
}
void chtrObject::NameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// The property's check will prevent an infinite
	// loop here.
	this->SetName( m_Data.m_Name.GetValue() );

	if (i_bDirty)
	{
		chtrDialogDataUtil::UpdateListDialog();
		chtrDocumentChunk::ActiveDataChanged();
	}
}
void chtrObject::VisibleChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// object is visible only if channel and gui and layer are all
	// are set visible == true;
	m_p3DObject->SetRenderable(m_Data.m_bVisible.GetValue() 
		&& m_Data.m_bEditorVisible.GetValue() 
		&& m_bLayerVisible);
}

void chtrObject::FilenameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	//UpdateFilename( m_Data.m_Filename.GetValue() );

	//if (i_bDirty)
	//{
	//	chtrDialogDataUtil::UpdateListDialog();
	//	chtrDocumentChunk::ActiveDataChanged();
	//}

	if (i_bDirty)
	{
		// Do the changed delayed so that the property controls aren't 
		// deleted while still in the callback
		mnmThinkMgr::CallFunctionDelayed( boost::bind(
						chtrOperations::ChangeFilename, this->GetName(), m_Data.m_Filename.GetValue()) );
	}
}


//--------------------------------------------------------------------
// This callback is for the display of joints
//--------------------------------------------------------------------
void chtrObject::JointDisplayChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (m_pCharacter)
	{
		switch (m_JointDisplay.GetValue())
		{
		case e_ModelOnly:
			m_pCharacter->SetJointDisplay(smdlSubdivCharacter::e_ModelOnly);
			break;
		case e_JointsOnly:
			m_pCharacter->SetJointDisplay(smdlSubdivCharacter::e_JointsOnly);
			break;
		case e_JointsAndModel:
			m_pCharacter->SetJointDisplay(smdlSubdivCharacter::e_JointsAndModel);
			break;
		}
	}
}
