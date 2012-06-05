/*****************************************************************************
**  aoScriptObject.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/AmbientOcclusion/Object/aoScriptObject.hpp"

//#include "Systems/AmbientOcclusion/Object/aoAOObject.hpp"
#include "Systems/AmbientOcclusion/Data/aoDocumentChunk.hpp"

#include "Support/grps/grpsGroupMgr.hpp"
#include "Support/lyer/lyerLayerMgr.hpp"
#include "Support/mnm/mnmReferencePosChannel.hpp"
#include "Support/tmln/tmlnChannelBoolean.hpp"
#include "Support/tmln/tmlnChannelColor.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelOrientation.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"

// library
#include "Tool/api3d/api3dBillboard.hpp"
#include "Tool/api3d/api3dObjectEntity.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Core/geo/geoRayIntersection.hpp"



//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
aoScriptObject::aoScriptObject( )
: m_pIcon(NULL)
{
	m_pIcon = new aoAOObject;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
aoScriptObject::~aoScriptObject()
{
	delete m_pIcon;
}

//--------------------------------------------------------------------
//  get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
void aoScriptObject::GetResourceList( fsResourceTrackerData& io_List )
{
}

//============================================================================
//	Data
//============================================================================


//--------------------------------------------------------------------
// Get values as a data structure
//--------------------------------------------------------------------
aoScriptData aoScriptObject::GetScriptData() const
{
	aoScriptData data(this->GetBaseData());

	// get driver info
//	tmlnCreator::GetDriverInfo(this, data.m_Drivers);

	// get the channel info
//	this->GetChannelSet().GetChannelInfo( data.m_ChannelInfo );

	return data;
}

//--------------------------------------------------------------------
// Set from data structure
//--------------------------------------------------------------------
void aoScriptObject::SetScriptData(const aoScriptData &i_Data)
{
	this->SetBaseData( i_Data.m_BaseData );

	// create drivers from info
//	tmlnCreator::SetDriverInfo(this, i_Data.m_Drivers);

	// set the channels 
//	this->ChannelSet().SetChannelInfo( i_Data.m_ChannelInfo );
}

//--------------------------------------------------------------------
// Set from light base data structure
//--------------------------------------------------------------------
void aoScriptObject::SetBaseData(const aoAOData &i_Data)
{
	// Set data into Icon/Pick object
	m_pIcon->SetData(i_Data);
}

//--------------------------------------------------------------------
// Get values as a light base data structure
//--------------------------------------------------------------------
aoAOData aoScriptObject::GetBaseData() const
{
	aoAOData data( m_pIcon->GetData() );
	return data;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
aoAOObject* aoScriptObject::GetPickObject() const
{
	return m_pIcon;
}

//--------------------------------------------------------------------
//	Name - passes functions on to pick object
//--------------------------------------------------------------------
void aoScriptObject::SetName(const nameString& i_Name)
{
    m_pIcon->SetName( i_Name );
}
const nameString& aoScriptObject::GetName() const
{
	return m_pIcon->GetName();
}

