/*****************************************************************************
**  propPropObject.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Props/Object/propPropObject.hpp"

#include "Systems/Props/GUI/propDialogDataUtil.hpp"
#include "Systems/Props/Data/propDocumentChunk.hpp"
#include "Systems/Props/Undo/propOperations.hpp"

#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"
#include "Support/evmt/evmtEnvironmentMgr.hpp"
#include "Support/ltst/ltstLightSetMgr.hpp"
#include "Support/rlyr/rlyrRenderLayerMgr.hpp"

// library
#include "Tool/api3d/api3dObjectEntity.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Core/geo/geoRayIntersection.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"
#include "Graphics/smdl/smdlSubdivCharacter.hpp"


//----------------------------------------------------------------------------
// ownership for the api3dObject passes to this object
//----------------------------------------------------------------------------
propPropObject::propPropObject( api3dObject* i_pObject, fsLocator& i_Dir )
:	m_pCharacter(NULL),
	m_bLayerVisible(true)
{
	api3dObjectEntity * pPropEnt = dynamic_cast<api3dObjectEntity *>(i_pObject);
	m_p3DObject = pPropEnt;
	DBG_ASSERT0( pPropEnt != 0, "Object doesn't cast ti api3dObjectEntity");
	m_LocalBox = pPropEnt->GetWorldBox();

	// If this is non-NULL, then we have a subdivision character
	m_pCharacter = dynamic_cast<smdlSubdivCharacter*>(pPropEnt->Object());

	ltstLightSetMgr::AddObject(this, m_p3DObject);
	evmtEnvironmentMgr::AddObject(this, m_p3DObject);
	rlyrRenderLayerMgr::AddObject(this, m_p3DObject);
	SetDirectory( i_Dir );

	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyTextBoxUIInfo(&(m_Data.m_Name), "Asset", "Name of the object");
	AddProperty( pPUII );
	pPUII = new prtyTextBoxUIInfo(&(m_Data.m_Filename), "Asset", "File Name of the object");
	pPUII->SetReadOnly(true);
	AddProperty( pPUII );

	pPUII = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_Position), "Transform", "Position of the object");
	AddProperty( pPUII );
	prtyVector3dEditUpDownUIInfo* pPVEUDUII  = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_Orientation), "Transform", "Orientation of the object");
	pPVEUDUII->SetIncrement(0.5f, 0.5f, 0.5f);
	pPVEUDUII->SetDecimalPlaces(1);
	AddProperty( pPVEUDUII );	
	pPUII  = new prtyFloatEditUIInfo(&(m_Data.m_Scale), "Transform", "Scale of the object");
	AddProperty( pPUII );
	pPUII = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_PivotPoint), "Transform", "Pivot position in local space");
	AddProperty( pPUII );

	// Register callbacks to update geometry
	// when properties change
	m_Data.m_Position.AddCallback(new prtyCallbackWrapper<propPropObject>(this, &propPropObject::PositionChanged));
	m_Data.m_Orientation.AddCallback(new prtyCallbackWrapper<propPropObject>(this, &propPropObject::OrientationChanged));
	m_Data.m_Scale.AddCallback(new prtyCallbackWrapper<propPropObject>(this, &propPropObject::ScaleChanged));
	m_Data.m_PivotPoint.AddCallback(new prtyCallbackWrapper<propPropObject>(this, &propPropObject::PivotChanged));
	m_Data.m_PivotCompensation.AddCallback(new prtyCallbackWrapper<propPropObject>(this, &propPropObject::PivotCompensationChanged));
	m_Data.m_Name.AddCallback(new prtyCallbackWrapper<propPropObject>(this, &propPropObject::NameChanged));
	m_Data.m_bVisible.AddCallback(new prtyCallbackWrapper<propPropObject>(this, &propPropObject::VisibleChanged));

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
propPropObject::~propPropObject()
{
	//Note: m_pCharacter is not owned, just a view into this object's model

	if ( m_p3DObject )
	{
		ltstLightSetMgr::RemoveObject(this, m_p3DObject);
		evmtEnvironmentMgr::RemoveObject(this, m_p3DObject);
		rlyrRenderLayerMgr::RemoveObject(this, m_p3DObject);
		api3dScene::RemoveObject(m_p3DObject);
		delete m_p3DObject;
	}
}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string propPropObject::GetPick3dName() const
{
	return GetName().GetString();
}


//--------------------------------------------------------------------
// Get values as a data structure
//--------------------------------------------------------------------
const propData& propPropObject::GetData() const
{
	return m_Data;
}

//--------------------------------------------------------------------
// Set from data structure
//--------------------------------------------------------------------
void propPropObject::SetData(const propData &i_Data)
{
	m_Data = i_Data;
}

//--------------------------------------------------------------------
// Name property access
//--------------------------------------------------------------------
prtyName&	propPropObject::PropertyName()
{
	return m_Data.m_Name;
}
const prtyName&	propPropObject::GetPropertyName() const
{
	return m_Data.m_Name;
}

//--------------------------------------------------------------------
// Position property access
//--------------------------------------------------------------------
prtyPoint3d&	propPropObject::PropertyPosition()
{
	return m_Data.m_Position;
}
const prtyPoint3d&	propPropObject::GetPropertyPosition() const
{
	return m_Data.m_Position;
}

//--------------------------------------------------------------------
// Orientation property access
//--------------------------------------------------------------------
prtyRotation&	propPropObject::PropertyOrientation()
{
	return m_Data.m_Orientation;
}
const prtyRotation&	propPropObject::GetPropertyOrientation() const
{
	return m_Data.m_Orientation;
}

//--------------------------------------------------------------------
// Scale property access
//--------------------------------------------------------------------
prtyFloat&	propPropObject::PropertyScale()
{
	return m_Data.m_Scale;
}
const prtyFloat&	propPropObject::GetPropertyScale() const
{
	return m_Data.m_Scale;
}

//--------------------------------------------------------------------
// Visible property access
//--------------------------------------------------------------------
prtyBoolean&	propPropObject::PropertyVisible()
{
	return m_Data.m_bVisible;
}
const prtyBoolean&	propPropObject::GetPropertyVisible() const
{
	return m_Data.m_bVisible;
}

//----------------------------------------------------------------------------
//	GetWorldBox returns a box which completely encloses the propPropObject.
//----------------------------------------------------------------------------
const maAxisBox& propPropObject::GetWorldBox() const
{
	//const maAxisBox box = m_p3DObject->GetWorldBox();
	//DBG_LOG3( "POMin( %8.3f, %8.3f, %8.3f )", box.GetMinX(), box.GetMinY(), box.GetMinZ() );
	//DBG_LOG3( "  Max( %8.3f, %8.3f, %8.3f )", box.GetMaxX(), box.GetMaxY(), box.GetMaxZ() );

	return m_p3DObject->GetWorldBox();
}

//----------------------------------------------------------------------------
//	GetLocalBox returns a box which would enclose the propPropObject
//	if its transformations were identity
//----------------------------------------------------------------------------
const maAxisBox& propPropObject::GetLocalBox() const
{
	return m_LocalBox;
}

//--------------------------------------------------------------------
//	GetWorldPivot returns the point in world space that this
//		object will rotate around. Used to center rotation and
//		scale compasses.
//--------------------------------------------------------------------
//virtual 
maPoint3d propPropObject::GetWorldPivot() const
{
	// pivot point in world space is just local pivot plus position
	return (m_p3DObject->GetPivotPoint() + this->GetPosition());
}

//--------------------------------------------------------------------
//	Name
//--------------------------------------------------------------------
void propPropObject::SetName(const nameString& i_Name)
{
    // Set name first, which may change the ID number
    nameObject::SetName(i_Name);

    // Make sure that the data matches our true name (including
    // ID number)
    m_Data.m_Name = this->GetName();
}

//----------------------------------------------------------------------------
//	Position
//----------------------------------------------------------------------------
maPoint3d propPropObject::GetPosition() const
{
	return m_Data.m_Position.GetValue();
}
void propPropObject::UpdatePosition(const maPoint3d& i_Position, 
										bool i_bNewOperation)
{
	//propOperations::ChangePosition( i_Position );
	m_Data.m_Position.SetValue(i_Position, 
		i_bNewOperation ? prtyProperty::eNewUndo : prtyProperty::eContinueUndo);
}


//----------------------------------------------------------------------------
//	Orientation
//----------------------------------------------------------------------------
maRotation propPropObject::GetOrientation() const
{
	return m_Data.m_Orientation.GetQuaternion();
}
void propPropObject::UpdateOrientation(const maRotation& i_Orientation, 
										bool i_bNewOperation)
{
	//propOperations::ChangeOrientation( i_Orientation );
	m_Data.m_Orientation.SetQuaternion(i_Orientation, 
		i_bNewOperation ? prtyProperty::eNewUndo : prtyProperty::eContinueUndo);
}

//----------------------------------------------------------------------------
//	Scale
//----------------------------------------------------------------------------
maPoint3d propPropObject::GetScale() const
{
	return m_p3DObject->GetScale();

}
void propPropObject::UpdateScale(const maPoint3d& i_Scale, 
										bool i_bNewOperation)
{
	// Uniform Scale, just take off the X value
	m_Data.m_Scale.SetValue(i_Scale.m_X, 
		i_bNewOperation ? prtyProperty::eNewUndo : prtyProperty::eContinueUndo);
}

//----------------------------------------------------------------------------
//	RayPick returns true if the given ray intersects the propPropObject.
//	If it does, the t value is also returned in o_T.
//----------------------------------------------------------------------------
bool propPropObject::RayPick(	const maPoint3d& i_RayStart,
								const maPoint3d& i_RayEnd,
								float& o_T )
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
bool propPropObject::MatchPickCode(envType::UInt32 i_PickCode) const
{
	if (!m_p3DObject->GetRenderable())
		return false;

	return (m_p3DObject->ContainsPickCode(i_PickCode));
}

//--------------------------------------------------------------------
//  Changes visible state of prop based on GUI
//--------------------------------------------------------------------
void  propPropObject::SetEditorVisible(bool i_bVisible)
{
	m_Data.m_bEditorVisible = i_bVisible;

	// object is visible only if channel and gui and layer are all
	// are set visible == true;
	m_p3DObject->SetRenderable(m_Data.m_bVisible.GetValue() 
		&& m_Data.m_bEditorVisible.GetValue() 
		&& m_bLayerVisible);
}
bool propPropObject::GetEditorVisible() const
{
	return m_Data.m_bEditorVisible.GetValue();
}

//--------------------------------------------------------------------
//	LayerVisible represents if objects in the layer are visible
//--------------------------------------------------------------------
void propPropObject::SetLayerVisible(bool i_bVisible)
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
void propPropObject::SetWireframe(bool i_bWireframe)
{
	m_p3DObject->SetWireframe(i_bWireframe);
}

//--------------------------------------------------------------------
//	LowRes represents if the objects are rendered
//	using a low resolution model.
//--------------------------------------------------------------------
void propPropObject::SetLowRes(bool i_bLowRes)
{
	m_p3DObject->SetLowResolution(i_bLowRes);
}

//----------------------------------------------------------------------------
// Flags for how object can be modified
//----------------------------------------------------------------------------
mnmObject::RotateFlags	propPropObject::GetRotateFlags()
{
	return mnmObject::e_RotateAll;
}
mnmObject::ScaleFlags	propPropObject::GetScaleFlags()
{
	return mnmObject::e_ScaleUniform;
}
mnmObject::TranslateFlags	propPropObject::GetTranslateFlags()
{
	return mnmObject::e_TranslateAll;
}

//--------------------------------------------------------------------
//	entity
//--------------------------------------------------------------------
api3dObjectEntity * propPropObject::GetEntity()
{
	return this->m_p3DObject;
}

//--------------------------------------------------------------------
//	Get reference for given name.  The returned pointer is owned
//	by this object.  The object should be retained by the caller
//	to avoid repeated string searches.
//--------------------------------------------------------------------
api3dReference* propPropObject::GetReference(const char* i_Name)
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
void propPropObject::GetReferenceList(std::vector<std::string> &o_List)
{
	m_p3DObject->GetReferenceList(o_List);
}
//--------------------------------------------------------------------
//	filename
//--------------------------------------------------------------------
//virtual
void propPropObject::GetFilename( itString& o_Filename ) const
{
	o_Filename = m_Data.m_Filename.GetValue();
}
void propPropObject::SetFilename(const itString& i_Filename)
{
	this->m_Data.m_Filename = i_Filename;
}

//--------------------------------------------------------------------
//	Directory
//--------------------------------------------------------------------
//virtual 
void propPropObject::SetDirectory( const fsLocator& i_Dir )
{
	m_Directory = i_Dir;
}
//virtual 
void propPropObject::GetDirectory( fsLocator& o_Dir ) const
{
	o_Dir = m_Directory;
}

//--------------------------------------------------------------------
// Set subdivision level being used.
//--------------------------------------------------------------------
void propPropObject::SetSubdivLevel(int i_SubdivLevel)
{
	if (m_pCharacter)
	{
		m_pCharacter->SetCurrentSubdivLevel(i_SubdivLevel);
	}	
}
int propPropObject::GetSubdivLevel() const
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
pick3dPickObject* propPropObject::GetParentObject() const
{
	return m_pParent;
}
//virtual 
void propPropObject::SetParentObject(pick3dPickObject* i_pParent)
{
	m_pParent = i_pParent;
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void propPropObject::PositionChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	const maPoint3d& position = m_Data.m_Position.GetValue();
	m_p3DObject->SetPosition( position );

	if (i_bDirty)
		propDocumentChunk::ActiveDataChanged();
}
void propPropObject::OrientationChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	const maRotation& orientation = m_Data.m_Orientation.GetQuaternion();
	m_p3DObject->SetOrientation( orientation );

	if (i_bDirty)
		propDocumentChunk::ActiveDataChanged();
}
void propPropObject::ScaleChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	m_p3DObject->SetUniformScale( m_Data.m_Scale.GetValue() );

	if (i_bDirty)
		propDocumentChunk::ActiveDataChanged();
}
void propPropObject::PivotChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	const maPoint3d& pivot = m_Data.m_PivotPoint.GetValue();
	if (i_bDirty)
	{
		const bool preserve_transformation = true;	// preserve transformation if coming from GUI
		m_p3DObject->SetPivotPoint( pivot, preserve_transformation );

		// When preserving transformation, the "pivot compensation" value will be updated.
		// So, we need to set that value into our data also
		m_Data.m_PivotCompensation = m_p3DObject->GetPivotCompensation();

		propDocumentChunk::ActiveDataChanged();
	}
	else
	{
		const bool dont_preserve_transformation = false;	// preserve transformation if coming from GUI
		m_p3DObject->SetPivotPoint( pivot, dont_preserve_transformation );
	}
}
void propPropObject::PivotCompensationChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	const maVector3d& vec = m_Data.m_PivotCompensation.GetValue();
	m_p3DObject->SetPivotCompensation( vec );

	if (i_bDirty)
		propDocumentChunk::ActiveDataChanged();
}
void propPropObject::NameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// The property's check will prevent an infinite
	// loop here.
	this->SetName( m_Data.m_Name.GetValue() );

	if (i_bDirty)
	{
		propDialogDataUtil::UpdateListDialog();
		propDocumentChunk::ActiveDataChanged();
	}
}
void propPropObject::VisibleChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// object is visible only if channel and gui and layer are all
	// are set visible == true;
	m_p3DObject->SetRenderable(m_Data.m_bVisible.GetValue() 
		&& m_Data.m_bEditorVisible.GetValue() 
		&& m_bLayerVisible);
}

