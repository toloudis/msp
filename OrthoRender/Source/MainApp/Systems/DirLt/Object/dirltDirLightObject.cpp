/*****************************************************************************
**  dirltDirLightObject.cpp
**
**      A dirltDirLightObject is a derived class for displaying a point
**	light's position.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "dirltDirLightObject.hpp"

#include "dirltOperations.hpp"

#include "api3dLightMgr.hpp"
#include "api3dObjectSimple.hpp"
#include "api3dScene.hpp"
#include "api3dShape.hpp"
#include "dbgLog.hpp"
#include "g3dDirectionalLight.hpp"
#include "geoRayIntersection.hpp"
#include "ltstLightSetMgr.hpp"
#include "tmlnCreator.hpp"
#include "tmlnTimelineMgr.hpp"
#include "prtyCheckBoxUIInfo.hpp"
#include "prtyColorRGBEditUIInfo.hpp"
#include "prtyTextBoxUIInfo.hpp"
#include "prtyVector3dEditUIInfo.hpp"
#include "prtyVector3dEditUpDownUIInfo.hpp"


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
	o_Orientation.SetValue( maVector3d( 0.0f, 1.0f, 0.0f ), i_Dir );

	o_Pos.Set( i_Dir.GetX() * l_cfDirScalar, i_Dir.GetY() * l_cfDirScalar, i_Dir.GetZ() * l_cfDirScalar );
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
dirltDirLightObject::dirltDirLightObject(const dirltData &i_Data)
:	m_Data(i_Data),
	m_WorldBox(l_SphereBox),
	m_bLightOwner( true )
{
	this->SetName( i_Data.m_Name.GetValue() );

	// Create g3d light
	m_pLight = api3dLightMgr::CreateDirectionalLight();

	// Create 3D icon and add it.
	//
	m_pObject = api3dShape::CreateCone( maFloatRGBA(1,0,0,1), l_fConeRadius, l_fConeHeight, 8 );
	api3dScene::AddObject(m_pObject);
	m_pObjectLine = create_line();
	api3dScene::AddObject(m_pObjectLine);

	m_Direction = i_Data.m_Direction.GetValue();

	SetObjectPositionAndOrientation();

	// Set starting values from passed in data
	this->SetData(i_Data);

	ltstLightSetMgr::AddLight(this, m_pLight);

	// Register the properties so they can be displayed to the user
	//
	RegisterProperties();
}
	
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dirltDirLightObject::dirltDirLightObject(g3dDirectionalLight* i_pLight)
:	m_WorldBox(l_SphereBox),
	m_bLightOwner(false)
{
	m_pLight = i_pLight;

	// Create 3D icon
	m_pObject = api3dShape::CreateCone( maFloatRGBA(1,0,0,1), l_fConeRadius, l_fConeHeight, 8 );

	dirltData data = this->GetData();
	m_Direction = data.m_Direction.GetValue();
	m_Position	= data.m_Position.GetValue();

	SetObjectPositionAndOrientation();

	api3dScene::AddObject(m_pObject);

	// Set starting values from passed in data
	this->SetData(data);

	ltstLightSetMgr::AddLight(this, m_pLight);

	// Register the properties so they can be displayed to the user
	//
	RegisterProperties();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dirltDirLightObject::~dirltDirLightObject()
{
	ltstLightSetMgr::RemoveLight(this, m_pLight);

	api3dScene::RemoveObject(m_pObject);
	delete m_pObject;
	api3dScene::RemoveObject(m_pObjectLine);
	delete m_pObjectLine;

	if ( m_bLightOwner )
	{
		api3dLightMgr::DestroyLight(m_pLight);
	}
}


//--------------------------------------------------------------------
// Get values as a light data structure
//--------------------------------------------------------------------
dirltData dirltDirLightObject::GetData() const
{
	dirltData data = m_Data;

	if ( m_pLight )
	{
		data.m_Direction	= m_pLight->GetDirection();
		data.m_ShadowSource = m_pLight->GetCastsShadow();
	}

	return data;
}

//--------------------------------------------------------------------
// Set from light data structure
//--------------------------------------------------------------------
void dirltDirLightObject::SetData(const dirltData &i_Data)
{
	m_Data = i_Data;

	// SetName happens after the m_Data set because
	// it may alter the m_Data field with the new ID
	SetName( i_Data.m_Name.GetValue() );

	m_Direction = i_Data.m_Direction.GetValue();

	DBG_LOG3("direction (%6.2f, %6.2f, %6.2f)", m_Direction.GetX(), m_Direction.GetY(), m_Direction.GetZ() );

	SetObjectPositionAndOrientation();

	if ( m_pLight )
	{
		m_pLight->SetIntensity(i_Data.m_Color.GetValue());
		m_pLight->SetCastsShadow(i_Data.m_ShadowSource.GetValue());
		m_pLight->SetEnable( i_Data.m_Enabled.GetValue() );
		m_pLight->SetDirection( i_Data.m_Direction.GetValue() );
		m_pLight->SetDiffuseEnabled( i_Data.m_bDiffuseEnabled.GetValue() );
		m_pLight->SetSpecularEnabled( i_Data.m_bSpecularEnabled.GetValue() );
	}
}

//----------------------------------------------------------------------------
//	GetWorldBox returns a box which completely encloses the dirltDirLightObject.
//----------------------------------------------------------------------------
const maAxisBox& dirltDirLightObject::GetWorldBox() const
{
	return m_WorldBox;
}

//----------------------------------------------------------------------------
//	GetLocalBox returns a box which would enclose the dirltDirLightObject
//	if its transformations were identity
//----------------------------------------------------------------------------
const maAxisBox& dirltDirLightObject::GetLocalBox() const
{
	return l_SphereBox;
}

//--------------------------------------------------------------------
// UpdateName() is called when the gui sets the name of the object,
//	derived classes can set dirty bits and do "undo" operations, etc.
// The default behavior calls SetName()
//--------------------------------------------------------------------
void dirltDirLightObject::UpdateName(const std::string& i_Name)
{
	dirltOperations::ChangeName( i_Name );
}


//--------------------------------------------------------------------
//	Name
//--------------------------------------------------------------------
void dirltDirLightObject::SetName(const nameString& i_Name)
{
    // Set name first, which may change the ID number
    nameObject::SetName(i_Name);

    // Make sure that the data matches our true name (including
    // ID number)
    m_Data.m_Name.SetValue(this->GetName());

	// Update LightSetMgr
	ltstLightSetMgr::LightRenamed(this);
}

//----------------------------------------------------------------------------
//	Position
//----------------------------------------------------------------------------
maPoint3d dirltDirLightObject::GetPosition() const
{
	return m_Position;
}
void dirltDirLightObject::SetPosition(const maPoint3d& i_Position)
{
	m_Position = i_Position;
	m_pObject->SetPosition(i_Position);
	m_pObjectLine->SetPosition(i_Position);
	m_WorldBox = l_SphereBox;
	m_WorldBox.Translate(i_Position);
}

//----------------------------------------------------------------------------
//	UpdatePosition - compass interaction has altered the position of
//	of this light
//----------------------------------------------------------------------------
void dirltDirLightObject::UpdatePosition(const maPoint3d& i_Position)
{
	dirltOperations::ChangePosition( i_Position );
}

//----------------------------------------------------------------------------
//	Orientation
//----------------------------------------------------------------------------
maRotation dirltDirLightObject::GetOrientation() const
{
	return m_Orientation;
}
void dirltDirLightObject::UpdateOrientation(const maRotation& i_Orientation)
{
	dirltOperations::ChangeOrientation( i_Orientation );
}

//virtual 
void dirltDirLightObject::SetOrientation(const maRotation& i_Orientation)
{
	m_Orientation = i_Orientation;

	m_pObject->SetOrientation( i_Orientation );
	m_pObjectLine->SetOrientation( i_Orientation );
}


//----------------------------------------------------------------------------
//	Scale
//----------------------------------------------------------------------------
maPoint3d dirltDirLightObject::GetScale() const
{
	return l_One;
}
void dirltDirLightObject::UpdateScale(const maPoint3d& i_Scale)
{
	// do nothing
}
//virtual 
void dirltDirLightObject::SetScale(const maPoint3d& i_Scale)
{
	// do nothing
}


//--------------------------------------------------------------------
//	Intensity
//--------------------------------------------------------------------
//virtual 
maFloatRGBA dirltDirLightObject::GetIntensity() const
{
	return m_Data.m_Color.GetValue();
}
//virtual 
void dirltDirLightObject::SetIntensity(const maFloatRGBA& i_Intensity)
{
	m_Data.m_Color.SetValue(i_Intensity);

	m_pLight->SetIntensity( i_Intensity );
}

//--------------------------------------------------------------------
//	Enabled
//--------------------------------------------------------------------
//virtual 
bool dirltDirLightObject::GetEnabled() const
{
	return m_pLight->IsEnabled();
}
//virtual 
void dirltDirLightObject::SetEnabled(const bool& i_Enabled)
{
	m_pLight->SetEnable(i_Enabled);
}

//----------------------------------------------------------------------------
//	RayPick returns true if the given ray intersects the dirltDirLightObject.
//	If it does, the t value is also returned in o_T.
//----------------------------------------------------------------------------
bool dirltDirLightObject::RayPick(	const maPoint3d& i_RayStart,
									const maPoint3d& i_RayEnd,
									float& o_T)
{
	return geoRayIntersection::IntersectLineSphere(	i_RayStart,
								i_RayEnd - i_RayStart,
								m_Position,
								l_PickRadius,
								o_T);
}

//----------------------------------------------------------------------------
//	Renderable sets whether the dirltDirLightObject can be selected.
//----------------------------------------------------------------------------
void dirltDirLightObject::SetRenderable(bool i_Renderable)
{
	m_pObject->SetRenderable(i_Renderable);
	m_pObjectLine->SetRenderable(i_Renderable);
//	this->ShowDriverIcons(i_Renderable);
}
bool dirltDirLightObject::GetRenderable() const
{
	return m_pObject->GetRenderable();
}

//----------------------------------------------------------------------------
//	GetDefaultTerrainOffset is the desired offset from
//	the terrain for this object.  This can be altered
//	by the user during placement
//----------------------------------------------------------------------------
float dirltDirLightObject::GetDefaultTerrainOffset() const
{
	return 0.0f;
}

//--------------------------------------------------------------------
//	does this object own the lights it holds?
//--------------------------------------------------------------------
void dirltDirLightObject::SetLightOwner( bool i_bOwner )
{
	m_bLightOwner = i_bOwner;
}
bool dirltDirLightObject::GetLightOwner() const
{
	return m_bLightOwner;
}

//----------------------------------------------------------------------------
// Flags for how object can be modified
//----------------------------------------------------------------------------
mnmObject::RotateFlags	dirltDirLightObject::GetRotateFlags()
{
	return (mnmObject::e_RotateXY);
}
mnmObject::ScaleFlags	dirltDirLightObject::GetScaleFlags()
{
	return mnmObject::e_ScaleNone;
}
mnmObject::TranslateFlags	dirltDirLightObject::GetTranslateFlags()
{
	return mnmObject::e_TranslateNone;
}

//----------------------------------------------------------------------------
// Get parent object of the spline in order for selection to trace from
//	this spline back to the controlled object.
//----------------------------------------------------------------------------
//virtual 
pick3dPickObject* dirltDirLightObject::GetParentObject() const
{
	return m_pParent;
}
//virtual 
void dirltDirLightObject::SetParentObject(pick3dPickObject* i_pParent)
{
	m_pParent = i_pParent;
}

//--------------------------------------------------------------------
//	SetObjectPositionAndOrientation()
//--------------------------------------------------------------------
void dirltDirLightObject::SetObjectPositionAndOrientation()
{
	//	set the position of the 3-D object
	//
	maPoint3d light_pos;
	maRotation light_orientation;
	calc_dirlight_position_orientation( m_Direction, light_pos, light_orientation );

	SetPosition( light_pos );
	SetOrientation( light_orientation );
}

//--------------------------------------------------------------------
//	Register the properties for display
//--------------------------------------------------------------------
void dirltDirLightObject::RegisterProperties()
{
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyVector3dEditUIInfo(&(m_Data.m_Position), "category2", "Position of the object");
	AddProperty( pPUII );
	pPUII  = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_Direction), "category2", "Direction of the object");
	AddProperty( pPUII );
	pPUII = new prtyTextBoxUIInfo(&(m_Data.m_Name), "category1", "Name of the object");
	AddProperty( pPUII );
	pPUII = new prtyTextBoxUIInfo(&(m_Data.m_Filename), "category1", "File Name of the object");
	pPUII->SetReadOnly(true);
	AddProperty( pPUII );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bEditorVisible), "Editor", "Is the object visible in the editor") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_Enabled), "Flags", "Is the object enabled") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_ShadowSource), "Flags", "Is this object a shadow source") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bDiffuseEnabled), "Flags", "Enable the diffuse channel") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bSpecularEnabled), "Flags", "Enable the specular channel") );
	pPUII = new prtyColorRGBEditUIInfo(&(m_Data.m_Color), "category1", "Color of the object");
	AddProperty( pPUII );
}

