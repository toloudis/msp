/*****************************************************************************
**  envtScriptObject.cpp
**
**      A envtScriptObject is a derived class for displaying a point
**	light's position.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/Object/envtScriptObject.hpp"

#include "Systems/Environments/GUI/envtTextureList.hpp"
#include "Systems/Environments/Undo/envtOperations.hpp"

#include "Support/evmt/evmtEnvironmentMgr.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
envtScriptObject::envtScriptObject(const envtScriptData &i_Data,
								   const nameString& i_Name)
:	m_pIcon(NULL)
{
	m_pIcon	= new envtEnvironmentObject();
	m_pIcon->SetParentObject(this);

	//evmtEnvironmentMgr::CreateEnvironment(i_Name);

	// Timeline stuff
	tmlnTimelineMgr::AddObject(this,"Environment");

	//
	m_pDiffuseFactorChannel = new tmlnChannelFloatProperty("DiffuseFactor", m_pIcon->PropertyDiffuseFactor());
	this->AddChannel(m_pDiffuseFactorChannel);
	m_pDiffuseAngleChannel = new tmlnChannelFloatProperty("DiffuseAngle", m_pIcon->PropertyDiffuseAngle());
	this->AddChannel(m_pDiffuseAngleChannel);
	m_pSpecularFactorChannel = new tmlnChannelFloatProperty("SpecularFactor", m_pIcon->PropertySpecularFactor());
	this->AddChannel(m_pSpecularFactorChannel);
	m_pSpecularAngleChannel = new tmlnChannelFloatProperty("SpecularAngle", m_pIcon->PropertySpecularAngle());
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
std::string envtScriptObject::GetTmlnName() const
{
	return m_pIcon->GetName().GetString();
}

//--------------------------------------------------------------------
//  get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
//virtual 
void envtScriptObject::GetResourceList( fsResourceTrackerData& io_List )
{
	DBG_ASSERT0(m_pIcon != NULL, "underlying object is null");

	// let's make a best guess at a folder:
	// look up file paths in envtTextureList for initial dir.
	fsysFileList fileList;
	envtTextureList::BuildFileList(fileList);

	itString name = m_pIcon->GetData().m_DiffuseMapName.GetValue();
	if (name.GetLength() > 0)
	{
		fsLocator texDir;
		fileList.GetFilePath(name, texDir);
		texDir.Push(name);
		io_List.Add( texDir );
	}
	name = m_pIcon->GetData().m_SpecularMapName.GetValue();
	if (name.GetLength() > 0)
	{
		fsLocator texDir;
		fileList.GetFilePath(name, texDir);
		texDir.Push(name);
		io_List.Add( texDir );
	}

	//	add all the driver resources as well
	tmlnScriptObject::GetResourceList( io_List );
}

//--------------------------------------------------------------------
// Get values as a light data structure
//--------------------------------------------------------------------
envtScriptData envtScriptObject::GetScriptData() const
{
	envtScriptData data(this->GetBaseData());

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
void envtScriptObject::Add(const nameString& i_ObjectName)
{
	m_pIcon->Add(i_ObjectName);
}
void envtScriptObject::Remove(const nameString& i_ObjectName)
{
	m_pIcon->Remove(i_ObjectName);
}

void envtScriptObject::RefreshNames()
{
	m_pIcon->RefreshNames();
}
