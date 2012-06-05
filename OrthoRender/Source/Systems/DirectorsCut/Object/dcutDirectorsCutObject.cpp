/*****************************************************************************
**  dcutDirectorsCutObject.cpp
**
**      A dcutDirectorsCutObject is a derived class for tracking the edit cuts
**	between cameras.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/DirectorsCut/Object/dcutDirectorsCutObject.hpp"

#include "Systems/DirectorsCut/Data/dcutDocumentChunk.hpp"
#include "Systems/DirectorsCut/Undo/dcutOperations.hpp"

#include "Support/cams/camsDirectorsCutMgr.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"


namespace
{
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dcutDirectorsCutObject::dcutDirectorsCutObject()
: m_CameraIndex(-1)
{
	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyTextBoxUIInfo(&(m_Data.m_Name), "Asset", "Name of the object");
	AddProperty( pPUII );
	pPUII = new prtyTextBoxUIInfo(&(m_Data.m_Description), "Asset", "Description of the object");
	AddProperty( pPUII );

	// Register callbacks to update particle generator and icons
	// when properties change
	m_Data.m_Name.AddCallback(new prtyCallbackWrapper<dcutDirectorsCutObject>(this, &dcutDirectorsCutObject::NameChanged));
	m_Data.m_Description.AddCallback(new prtyCallbackWrapper<dcutDirectorsCutObject>(this, &dcutDirectorsCutObject::DescriptionChanged));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dcutDirectorsCutObject::~dcutDirectorsCutObject()
{
}

//--------------------------------------------------------------------
//  Return index for which camera to use at this moment 
//  for this director's cut.
//--------------------------------------------------------------------
//virtual 
int dcutDirectorsCutObject::GetCameraIndex()
{
	return m_CameraIndex;
}

//--------------------------------------------------------------------
//  Set the index of the camera to use. This is set from the
//	drivers to animate the cuts.
//--------------------------------------------------------------------
void dcutDirectorsCutObject::SetCameraIndex(int i_Index)
{
	m_CameraIndex = i_Index;
}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string dcutDirectorsCutObject::GetPick3dName() const
{
	return GetName().GetString();
}


//--------------------------------------------------------------------
// Get values as a camera data structure
//--------------------------------------------------------------------
const dcutCueData& dcutDirectorsCutObject::GetData() const
{
	return m_Data;
}

//--------------------------------------------------------------------
// Set from camera data structure
//--------------------------------------------------------------------
void dcutDirectorsCutObject::SetData(const dcutCueData &i_Data)
{
	m_Data = i_Data;
}


//--------------------------------------------------------------------
// Name property access
//--------------------------------------------------------------------
prtyName&	dcutDirectorsCutObject::PropertyName()
{
	return m_Data.m_Name;
}
const prtyName&	dcutDirectorsCutObject::GetPropertyName() const
{
	return m_Data.m_Name;
}

//--------------------------------------------------------------------
// Description property access
//--------------------------------------------------------------------
const prtyText&	dcutDirectorsCutObject::GetPropertyDescription() const
{
	return m_Data.m_Description;
}

//--------------------------------------------------------------------
//	Name
//--------------------------------------------------------------------
void dcutDirectorsCutObject::SetName(const nameString& i_Name)
{
    // Set name first, which may change the ID number
    nameObject::SetName(i_Name);

    // Make sure that the data matches our true name (including
    // ID number)
    m_Data.m_Name.SetValue( this->GetName() );
}

//----------------------------------------------------------------------------
// Get parent object of the spline in order for selection to trace from
//	this spline back to the controlled object.
//----------------------------------------------------------------------------
//virtual 
pick3dPickObject* dcutDirectorsCutObject::GetParentObject() const
{
	return m_pParent;
}
//virtual 
void dcutDirectorsCutObject::SetParentObject(pick3dPickObject* i_pParent)
{
	m_pParent = i_pParent;
}


//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void dcutDirectorsCutObject::NameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// Use our old name to get the index
	int index = camsDirectorsCutMgr::GetIndexForName(this->GetName());

	// The property's check will prevent an infinite
	// loop here.
	this->SetName( m_Data.m_Name.GetValue() );

	// Notify that we have a new name
	if (index >= 0)
		camsDirectorsCutMgr::SetDirectorsCutName(index, this->GetName() );

	if (i_bDirty)
	{
		dcutDocumentChunk::ActiveDataChanged();

		dcutDialogUtil::UpdateListDialog();
	}
}
void dcutDirectorsCutObject::DescriptionChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// Notify that we have a new description
	int index = camsDirectorsCutMgr::GetIndexForName(this->GetName());
	if (index >= 0)
		camsDirectorsCutMgr::SetDirectorsCutDescription(index, m_Data.m_Description.GetValue());

	if (i_bDirty)
	{
		dcutDialogUtil::UpdateListDialog();

		dcutDocumentChunk::ActiveDataChanged();
	}
}
