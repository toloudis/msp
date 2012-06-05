/*****************************************************************************
**  dirltScriptObject.cpp
**
**      A dirltScriptObject is a derived class for displaying a point
**	light's position.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "dirltScriptObject.hpp"
#include "dirltChannelColor.hpp"
#include "dirltChannelEnabled.hpp"
#include "dirltChannelLightPos.hpp"
#include "dirltOperations.hpp"

#include "api3dLightMgr.hpp"
#include "api3dObjectSimple.hpp"
#include "api3dScene.hpp"
#include "api3dShape.hpp"
#include "dbgLog.hpp"
#include "envSTLHelpers.hpp"
#include "g3dDirectionalLight.hpp"
#include "geoRayIntersection.hpp"
#include "tmlnCreator.hpp"
#include "tmlnTimelineMgr.hpp"


namespace
{
	const float l_cfDirScalar	= 20.0f;
	const float l_fConeRadius	= 0.5f;
	const float l_fConeHeight	= 1.0f;
	const float l_PickRadius	= 1.0f;	// pick larger than icon

	const maAxisBox l_SphereBox(-l_fConeRadius, l_fConeRadius,
								-l_fConeRadius, l_fConeRadius,
								-l_fConeRadius, l_fConeRadius);
	const maVector3d l_Zero(0,0,0);
	const maVector3d l_One(1,1,1);
	const maRotation l_NoRot;

//----------------------------------------------------------------------------
//	figure out the 3-D object's position and orientation based on a
//	direction vector.
//----------------------------------------------------------------------------
void calc_dirlight_position_orientation( const maVector3d& i_Dir, maPoint3d& o_Pos, maRotation& o_Orientation )
{
	o_Orientation.SetValue( maVector3d( 0.0f, -1.0f, 0.0f ), i_Dir );

	o_Pos.Set( i_Dir.GetX() * l_cfDirScalar, -i_Dir.GetY() * l_cfDirScalar, i_Dir.GetZ() * l_cfDirScalar );
}

api3dObject* create_line()
{
	maPoint3d	line_list[2];

	line_list[0].Set( 0.0f, 0.0f, 0.0f );
	line_list[1].Set( 0.0f, -1.0f * l_cfDirScalar, 0.0f );

	return api3dShape::CreateLineList( maFloatRGBA(1,0,0,1), &line_list[0], 2 );
}

}//eon


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dirltScriptObject::dirltScriptObject(const dirltScriptData &i_Data)
:	m_Data(i_Data),
	m_pPosChannel(0),
	m_pEnableChannel(0),
	m_pColorChannel(0),
	m_pIcon(0)
{
	m_pIcon = new dirltDirLightObject(i_Data.m_BaseData);
	m_pIcon->SetParentObject(this);

	this->SetName( i_Data.m_BaseData.m_Name.GetValue() );

	// Timline stuff
	//
	//	Note: when adding a new channel add it also to the get/set data at the ends.
	//
	tmlnTimelineMgr::AddObject(this,"Directional Light");
	m_pPosChannel = new dirltChannelLightPos("Position", m_pIcon);
	this->AddChannel(m_pPosChannel);
	m_pEnableChannel = new dirltChannelEnabled("Enabled", m_pIcon);
	this->AddChannel(m_pEnableChannel);
	m_pColorChannel = new dirltChannelColor("Color", m_pIcon);
	this->AddChannel(m_pColorChannel);

	// Set starting values from passed in data
	this->SetScriptData(i_Data);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dirltScriptObject::dirltScriptObject(g3dDirectionalLight* i_pLight)
:	m_pPosChannel(0),
	m_pEnableChannel(0),
	m_pColorChannel(0),
	m_pIcon(0)
{
	m_pIcon = new dirltDirLightObject(i_pLight);
	m_pIcon->SetParentObject(this);

	// Timline stuff
	tmlnTimelineMgr::AddObject(this, "Directional Light");
	m_pPosChannel = new dirltChannelLightPos("Position", m_pIcon);
	this->AddChannel(m_pPosChannel);
	m_pEnableChannel = new dirltChannelEnabled("Enabled", m_pIcon);
	this->AddChannel(m_pEnableChannel);
	m_pColorChannel = new dirltChannelColor("Color", m_pIcon);
	this->AddChannel(m_pColorChannel);

	// Set starting values from passed in data
	//this->SetScriptData(data);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dirltScriptObject::~dirltScriptObject()
{
	tmlnTimelineMgr::RemoveObject(this);

	delete m_pIcon;
	delete m_pPosChannel;
	delete m_pEnableChannel;
	delete m_pColorChannel;
}


//--------------------------------------------------------------------
// Get values as a light data structure
//--------------------------------------------------------------------
dirltScriptData dirltScriptObject::GetScriptData() const
{
	dirltScriptData data;

	data.m_BaseData	= this->GetBaseData();

	tmlnCreator::GetDriverInfo(this, data.m_Drivers);

	// get the channel info (order should match the "set")
	envSTLHelpers::DeleteContainer( data.m_Channels );
	data.m_Channels.push_back(m_pPosChannel->GetChannelInfo());
	data.m_Channels.push_back(m_pEnableChannel->GetChannelInfo());
	data.m_Channels.push_back(m_pColorChannel->GetChannelInfo());

	return data;
}

//--------------------------------------------------------------------
// Set from light data structure
//--------------------------------------------------------------------
void dirltScriptObject::SetScriptData(const dirltScriptData &i_Data)
{
	this->SetBaseData( i_Data.m_BaseData );

	// create drivers from info
	tmlnCreator::SetDriverInfo(this, i_Data.m_Drivers);

	// set the channels (order should match the "get")
	int count = i_Data.m_Channels.size();
	int i;
	for (i = 0; i < count; ++i)
	{
		switch (i)
		{
			case 0: m_pPosChannel->SetChannelInfo(i_Data.m_Channels[i]); break;
			case 1: m_pEnableChannel->SetChannelInfo(i_Data.m_Channels[i]); break;
			case 2: m_pColorChannel->SetChannelInfo(i_Data.m_Channels[i]); break;
		}
	}
}

//--------------------------------------------------------------------
// Get values as a light base data structure
//--------------------------------------------------------------------
dirltData dirltScriptObject::GetBaseData() const
{
	dirltData data	= m_pIcon->GetData();

	data.m_Color	= m_pColorChannel->GetOriginalColor();
	data.m_Enabled	= m_pEnableChannel->GetOriginalState();
	data.m_Position	= m_pPosChannel->GetOriginalPosition();

	return data;
}

//--------------------------------------------------------------------
// Set from light base data structure
//--------------------------------------------------------------------
void dirltScriptObject::SetBaseData(const dirltData &i_Data)
{
	// Set data into Icon/Pick object
	m_pIcon->SetData(i_Data);

	// Since the above line causes the Icon object to register
	// the name, the icon's name is the definitive name now,
	// not the one in the i_Data struct. By calling just the
	// nameObject::SetName function here, we avoid setting the
	// icon's name again
	nameObject::SetName(m_pIcon->GetName());

	m_pColorChannel->SetOriginalColor(i_Data.m_Color.GetValue());
	m_pEnableChannel->SetOriginalState(i_Data.m_Enabled.GetValue());
	m_pPosChannel->SetOriginalPosition(i_Data.m_Position.GetValue());
}

//--------------------------------------------------------------------
// Accessors to channels
//--------------------------------------------------------------------
tmlnChannelPosition& dirltScriptObject::PositionChannel()
{
	return (*m_pPosChannel);
}
tmlnChannelBoolean& dirltScriptObject::EnabledChannel()
{
	return (*m_pEnableChannel);
}
tmlnChannelColor& dirltScriptObject::ColorChannel()
{
	return (*m_pColorChannel);
}

//----------------------------------------------------------------------------
//	Position
//----------------------------------------------------------------------------
maPoint3d dirltScriptObject::GetPosition() const
{
	return m_pIcon->GetPosition();
}
void dirltScriptObject::SetPosition(const maPoint3d& i_Position)
{
	this->m_pIcon->SetPosition(i_Position);

	m_Data.m_BaseData.m_Position = i_Position;

	m_pPosChannel->SetOriginalPosition(i_Position);
}

//----------------------------------------------------------------------------
//	Orientation
//----------------------------------------------------------------------------
maRotation dirltScriptObject::GetOrientation() const
{
	return m_pIcon->GetOrientation();
}
void dirltScriptObject::SetOrientation(const maRotation& i_Orientation)
{
	m_pIcon->SetOrientation( i_Orientation );
}

//----------------------------------------------------------------------------
//	Scale
//----------------------------------------------------------------------------
maPoint3d dirltScriptObject::GetScale() const
{
	return l_One;
}
void dirltScriptObject::SetScale(const maPoint3d& i_Scale)
{
	// do nothing
}

//--------------------------------------------------------------------
//	Name
//--------------------------------------------------------------------
void dirltScriptObject::SetName(const nameString& i_Name)
{
    // Set name first, this will call nameMgr::RegisterName, and may set the
    // ID number. So, after this point, we need to use this->GetName() in order
    // to get the correct ID number
    nameObject::SetName(i_Name);

    // make sure that the Icon and the script object have the same 
    // nameString, including ID
    m_pIcon->SetName( this->GetName() );

    // if the script object has its own m_Data, then it needs to 
    // update it here also, because the ID may have changed.
    m_Data.m_BaseData.m_Name = this->GetName();
}

//----------------------------------------------------------------------------
//	RayPick returns true if the given ray intersects the dirltScriptObject.
//	If it does, the t value is also returned in o_T.
//----------------------------------------------------------------------------
bool dirltScriptObject::RayPick(	const maPoint3d& i_RayStart,
						const maPoint3d& i_RayEnd,
						float& o_T)
{
	return geoRayIntersection::IntersectLineSphere(	i_RayStart,
								i_RayEnd - i_RayStart,
								m_pIcon->GetPosition(),
								l_PickRadius,
								o_T);
}

//----------------------------------------------------------------------------
//	Renderable sets whether the dirltScriptObject can be selected.
//----------------------------------------------------------------------------
void dirltScriptObject::SetRenderable(bool i_Renderable)
{
	m_pIcon->SetRenderable( i_Renderable );

	this->ShowDriverIcons(i_Renderable);
}
bool dirltScriptObject::GetRenderable() const
{
	return m_pIcon->GetRenderable();
}

//--------------------------------------------------------------------
//	does this object own the lights it holds?
//--------------------------------------------------------------------
void dirltScriptObject::SetLightOwner( bool i_bOwner )
{
	m_pIcon->SetLightOwner( i_bOwner );
}
bool dirltScriptObject::GetLightOwner() const
{
	return m_pIcon->GetLightOwner();
}

//--------------------------------------------------------------------
// For polling if an manipulation operation is currently enabled
//--------------------------------------------------------------------
bool dirltScriptObject::IsOperationEnabled(cmpsManipObject::Operations i_Operation, float i_Time)
{
	switch (i_Operation)
	{
	case mnmObject::e_Translate:
		return (!m_pPosChannel->IsDriverActiveAtTime( i_Time ));
	case mnmObject::e_Rotate:
	case mnmObject::e_Scale:
		return true;
	}
	return true;
}

//--------------------------------------------------------------------
// This is called when a driver in this script object is changed,
//	or if a driver is added or deleted from this object.
//--------------------------------------------------------------------
void dirltScriptObject::NotifyDriverChanged()
{
	dirltOperations::ChangeDriverData(this->GetScriptData());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
dirltDirLightObject* dirltScriptObject::GetPickObject() const
{
	return m_pIcon;
}

