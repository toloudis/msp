/*****************************************************************************
**  lsetScriptObject.cpp
**
**      A lsetScriptObject is a derived class for displaying a point
**	light's position.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/Object/lsetScriptObject.hpp"

#include "Systems/LightSets/Data/lsetDocumentChunk.hpp"

#include "Support/ltst/ltstLightSetMgr.hpp"
#include "Support/tmln/tmlnChannelColor.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
lsetScriptObject::lsetScriptObject(const lsetScriptData &i_Data,
								   const nameString& i_Name)
:	m_pIcon(NULL)
{
	m_pIcon	= new lsetLightSetObject(i_Name);
	m_pIcon->SetParentObject(this);

	// Timeline stuff
	tmlnTimelineMgr::AddObject(this,"LightSet");

	//
	m_pAmbientLightChannel = new tmlnChannelColorProperty("AmbientLight", m_pIcon->PropertyAmbientLight());
	this->AddChannel(m_pAmbientLightChannel);

	// Set starting values from passed in data
	this->SetScriptData(i_Data);
	this->SetName(i_Name);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
lsetScriptObject::~lsetScriptObject()
{
	// Unregister from Timeline
	tmlnTimelineMgr::RemoveObject(this);

	delete m_pIcon;
}


//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string lsetScriptObject::GetTmlnName() const
{
	return m_pIcon->GetName().GetString();
}

//--------------------------------------------------------------------
// Get values as a light data structure
//--------------------------------------------------------------------
lsetScriptData lsetScriptObject::GetScriptData() const
{
	lsetScriptData data(this->GetBaseData());

	// get driver info
	tmlnCreator::GetDriverInfo(this, data.m_Drivers);

	// get the channel info
	this->GetChannelSet().GetChannelInfo( data.m_ChannelInfo );

	return data;
}

//--------------------------------------------------------------------
// Set from light data structure
//--------------------------------------------------------------------
void lsetScriptObject::SetScriptData( const lsetScriptData &i_Data )
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
lsetData lsetScriptObject::GetBaseData() const
{
	lsetData data( m_pIcon->GetData() );

	data.m_AmbientLight = m_pAmbientLightChannel->GetOriginalColor();

	return data;
}

//--------------------------------------------------------------------
// Set from light base data structure
//--------------------------------------------------------------------
void lsetScriptObject::SetBaseData(const lsetData &i_Data)
{
	// Set data into Icon/Pick object
	m_pIcon->SetData(i_Data);

	m_pAmbientLightChannel->SetOriginalColor(i_Data.m_AmbientLight.GetValue());
}

//--------------------------------------------------------------------
// Accessors to channels
//--------------------------------------------------------------------
tmlnChannelColor& lsetScriptObject::AmbientLightChannel()
{
	return (*m_pAmbientLightChannel);
}

//--------------------------------------------------------------------
//	Name - passes functions on to pick object
//--------------------------------------------------------------------
void lsetScriptObject::SetName(const nameString& i_Name)
{
    m_pIcon->SetName( i_Name );
}
const nameString& lsetScriptObject::GetName() const
{
	return m_pIcon->GetName();
}


//--------------------------------------------------------------------
// This is called when a driver in this script object is changed,
//	or if a driver is added or deleted from this object.
//--------------------------------------------------------------------
void lsetScriptObject::NotifyDriverChanged()
{
	lsetDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
lsetLightSetObject*	lsetScriptObject::GetPickObject() const
{
	return this->m_pIcon;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//void lsetScriptObject::GetObjectsInLightSet(std::vector<nameString>& o_Names) const
//{
//	for (int i = 0; i < m_pIcon->GetData().m_Objects.size(); i++)
//	{
//		o_Names.push_back(m_pIcon->GetData().m_Objects[i]);
//	}
//}
//bool lsetScriptObject::HasObject(const nameString& i_Name) const
//{
//	for (int i = 0; i < m_pIcon->GetData().m_Objects.size(); i++)
//	{
//		// do i need to use ExactMatch here?
//		if (m_pIcon->GetData().m_Objects[i] == i_Name)
//			return true;
//	}
//	return false;
//}
//void lsetScriptObject::Add(const nameString& i_ObjectName)
//{
//	m_pIcon->Add(i_ObjectName);
//}
//void lsetScriptObject::Remove(const nameString& i_ObjectName)
//{
//	m_pIcon->Remove(i_ObjectName);
//}
//
//void lsetScriptObject::RefreshNames()
//{
//	m_pIcon->RefreshNames();
//}
