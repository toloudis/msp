/*****************************************************************************
**  envtScriptObject.cpp
**
**      A envtScriptObject is a derived class for displaying a point
**	light's position.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/Object/envtScriptObject.hpp"

#include "Systems/Environments/GUI/envtTextureList.hpp"
#include "Systems/Environments/Object/envtObjectMgr.hpp"
#include "Systems/Environments/Undo/envtOperations.hpp"
#include "Systems/Common/Object/cmmNamedScriptObjectReference.hpp"

#include "Support/evmt/evmtEnvironmentMgr.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
envtScriptObject::envtScriptObject(const envtScriptData &i_Data,
								   const nameString& i_Name)
:	m_pIcon(NULL)
{
	m_pIcon	= new envtEnvironmentObject();
	this->SetPropertyObject(*m_pIcon);

	//evmtEnvironmentMgr::CreateEnvironment(i_Name);

	// Timeline stuff
	tmlnTimelineMgr::AddObject(this,"Environment");

	//
	m_pDiffuseFactorChannel = new tmlnChannelFloatProperty(m_pIcon->PropertyDiffuseFactor());
	this->AddChannel(m_pDiffuseFactorChannel);
	m_pDiffuseAngleChannel = new tmlnChannelFloatProperty( m_pIcon->PropertyDiffuseAngle());
	this->AddChannel(m_pDiffuseAngleChannel);
	m_pSpecularFactorChannel = new tmlnChannelFloatProperty( m_pIcon->PropertySpecularFactor());
	this->AddChannel(m_pSpecularFactorChannel);
	m_pSpecularAngleChannel = new tmlnChannelFloatProperty( m_pIcon->PropertySpecularAngle());
	this->AddChannel(m_pSpecularAngleChannel);

	// Set starting values from passed in data
	this->SetScriptData(i_Data);
	this->SetName(i_Name);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
envtScriptObject::~envtScriptObject()
{
	// Unregister from Timeline
	tmlnTimelineMgr::RemoveObject(this);

	delete m_pIcon;
}


//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string envtScriptObject::GetDisplayName() const
{
	return m_pIcon->GetName().GetString();
}

//--------------------------------------------------------------------
//	CreateReferenceToSelf - create a reference that refers to the
//	given object. This reference should be able to refer to 
//  future instances of this object persistent through deletion
//	and recreation through undo.
//--------------------------------------------------------------------
shared_ptr<relObjectReference> envtScriptObject::CreateReferenceToSelf()
{
	shared_ptr<relObjectReference> named_reference(new cmmNamedScriptObjectReference<envtObjectMgr>(this->GetName()));
	return named_reference;
}

//--------------------------------------------------------------------
//  get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
//virtual 
void envtScriptObject::GetResourceList( fsResourceTrackerData& io_List )
{
	io_List.Add(nameString(this->GetDisplayName()), true);

	io_List.IncrementCurrentDepth();
	DBG_ASSERT(m_pIcon != NULL, "underlying object is null");

	io_List.Add( m_pIcon->GetData().m_DiffuseMapName.GetValue() );
	io_List.Add( m_pIcon->GetData().m_SpecularMapName.GetValue() );

	//	add all the driver resources as well
	tmlnScriptObject::GetResourceList( io_List );
	io_List.DecrementCurrentDepth();
}

//--------------------------------------------------------------------
// Get values as a light data structure
//--------------------------------------------------------------------
envtScriptData envtScriptObject::GetScriptData() const
{
	envtScriptData data(this->GetBaseData());

	// get custom property data
	this->GetCustomPropertyData(data.m_CustomProperties);

	// get driver info
	tmlnCreator::GetDriverInfo(this, data.m_Drivers);

	// get the channel info
	this->GetChannelSet().GetChannelInfo( data.m_ChannelInfo );

	return data;
}

//--------------------------------------------------------------------
// Set from light data structure
//--------------------------------------------------------------------
void envtScriptObject::SetScriptData( const envtScriptData &i_Data )
{
	this->SetBaseData( i_Data.m_BaseData );

	// set custom property data
	this->SetCustomPropertyData(i_Data.m_CustomProperties);

	// create drivers from info
	tmlnCreator::SetDriverInfo(this, i_Data.m_Drivers);

	// set the channels 
	this->ChannelSet().SetChannelInfo( i_Data.m_ChannelInfo );
}

//--------------------------------------------------------------------
// Get values as a light base data structure
//--------------------------------------------------------------------
envtData envtScriptObject::GetBaseData() const
{
	envtData data( m_pIcon->GetData() );

	data.m_DiffuseFactor = m_pDiffuseFactorChannel->GetOriginalValue();
	data.m_DiffuseAngle = m_pDiffuseAngleChannel->GetOriginalValue();
	data.m_SpecularAngle = m_pSpecularAngleChannel->GetOriginalValue();
	data.m_SpecularFactor = m_pSpecularFactorChannel->GetOriginalValue();

	return data;
}

//--------------------------------------------------------------------
// Set from light base data structure
//--------------------------------------------------------------------
void envtScriptObject::SetBaseData(const envtData &i_Data)
{
	// Set data into Icon/Pick object
	m_pIcon->SetData(i_Data);

	m_pDiffuseFactorChannel->SetOriginalValue(i_Data.m_DiffuseFactor.GetValue());
	m_pDiffuseAngleChannel->SetOriginalValue(i_Data.m_DiffuseAngle.GetValue());
	m_pSpecularAngleChannel->SetOriginalValue(i_Data.m_SpecularAngle.GetValue());
	m_pSpecularFactorChannel->SetOriginalValue(i_Data.m_SpecularFactor.GetValue());
}

//--------------------------------------------------------------------
// Accessors to channels
//--------------------------------------------------------------------
tmlnChannelFloat& envtScriptObject::DiffuseFactorChannel()
{
	return (*m_pDiffuseFactorChannel);
}
tmlnChannelFloat& envtScriptObject::DiffuseAngleChannel()
{
	return (*m_pDiffuseAngleChannel);
}
tmlnChannelFloat& envtScriptObject::SpecularFactorChannel()
{
	return (*m_pSpecularFactorChannel);
}
tmlnChannelFloat& envtScriptObject::SpecularAngleChannel()
{
	return (*m_pSpecularAngleChannel);
}

//--------------------------------------------------------------------
//	Name - passes functions on to pick object
//--------------------------------------------------------------------
void envtScriptObject::SetName(const nameString& i_Name)
{
    m_pIcon->SetName( i_Name );
}
const nameString& envtScriptObject::GetName() const
{
	return m_pIcon->GetName();
}

//--------------------------------------------------------------------
// This is called when a driver in this script object is changed,
//	or if a driver is added or deleted from this object.
//--------------------------------------------------------------------
void envtScriptObject::NotifyDriverChanged()
{
	envtOperations::ChangeDriverData(this->GetScriptData());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
envtEnvironmentObject*	envtScriptObject::GetPickObject() const
{
	return this->m_pIcon;
}

//--------------------------------------------------------------------
//	RayPick returns true if the given ray intersects the ptltScriptObject.
//	If it does, the t value is also returned in o_T.
//--------------------------------------------------------------------
void envtScriptObject::GetObjectsInEnvironment(std::vector<nameString>& o_Names) const
{
	for (int i = 0; i < m_pIcon->GetData().m_Objects.size(); i++)
	{
		o_Names.push_back(m_pIcon->GetData().m_Objects[i]);
	}
}
bool envtScriptObject::HasObject(const nameString& i_Name) const
{
	for (int i = 0; i < m_pIcon->GetData().m_Objects.size(); i++)
	{
		// do i need to use ExactMatch here?
		if (m_pIcon->GetData().m_Objects[i] == i_Name)
			return true;
	}
	return false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void envtScriptObject::ReportMemory(gfFileTxt& i_File)
{
	m_pIcon->ReportMemory(i_File);
}
