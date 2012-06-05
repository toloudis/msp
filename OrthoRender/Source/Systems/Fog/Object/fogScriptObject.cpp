/*****************************************************************************
**  fogScriptObject.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Fog/Object/fogScriptObject.hpp"

#include "Systems/Fog/Object/fogFogObject.hpp"
#include "Systems/Fog/Data/fogDocumentChunk.hpp"

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
fogScriptObject::fogScriptObject( )
: m_pIcon(NULL)
{
	m_pIcon = new fogFogObject;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
fogScriptObject::~fogScriptObject()
{
	delete m_pIcon;
}

//--------------------------------------------------------------------
//  get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
void fogScriptObject::GetResourceList( fsResourceTrackerData& io_List )
{
}

//============================================================================
//	Data
//============================================================================


//--------------------------------------------------------------------
// Get values as a data structure
//--------------------------------------------------------------------
fogScriptData fogScriptObject::GetScriptData() const
{
	fogScriptData data(this->GetBaseData());

	// get driver info
//	tmlnCreator::GetDriverInfo(this, data.m_Drivers);

	// get the channel info
//	this->GetChannelSet().GetChannelInfo( data.m_ChannelInfo );

	return data;
}

//--------------------------------------------------------------------
// Set from data structure
//--------------------------------------------------------------------
void fogScriptObject::SetScriptData(const fogScriptData &i_Data)
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
void fogScriptObject::SetBaseData(const fogFogData &i_Data)
{
	// Set data into Icon/Pick object
	m_pIcon->SetData(i_Data);
}

//--------------------------------------------------------------------
// Get values as a light base data structure
//--------------------------------------------------------------------
fogFogData fogScriptObject::GetBaseData() const
{
	fogFogData data( m_pIcon->GetData() );
	return data;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
fogFogObject* fogScriptObject::GetPickObject() const
{
	return m_pIcon;
}

//--------------------------------------------------------------------
//	Name - passes functions on to pick object
//--------------------------------------------------------------------
void fogScriptObject::SetName(const nameString& i_Name)
{
    m_pIcon->SetName( i_Name );
}
const nameString& fogScriptObject::GetName() const
{
	return m_pIcon->GetName();
}

