/*****************************************************************************
**  fogFogObject.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Fog/Object/fogFogObject.hpp"

// library
#include "Tool/api3d/api3dScene.hpp"
#include "Core/prty/prtyColorRGBAEditUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"

namespace
{
}

//----------------------------------------------------------------------------
// This object takes ownership of the arguments passed in
//----------------------------------------------------------------------------
fogFogObject::fogFogObject(	)
{
	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyColorRGBAEditUIInfo(&(m_Data.m_Color), "Parameters", "Color");
	AddProperty( pPUII );
	pPUII = new prtyComboBoxUIInfo(&(m_Data.m_Mode), "Parameters", "Fog Decay Function");
	AddProperty( pPUII );

	prtyRangedFloatUIInfo* pRFUII  ;
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Density), "Parameters", "Fog Density");
	pRFUII->SetMinimum(0);
	pRFUII->SetMaximum(1);
	pRFUII->SetDecimalPlaces(2);
	pRFUII->SetNumTicks(100);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Start), "Parameters", "Fog Start Depth");
	pRFUII->SetMinimum(0);
	pRFUII->SetMaximum(1);
	pRFUII->SetDecimalPlaces(2);
	pRFUII->SetNumTicks(100);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_End), "Parameters", "Fog End Depth");
	pRFUII->SetMinimum(0);
	pRFUII->SetMaximum(1);
	pRFUII->SetDecimalPlaces(2);
	pRFUII->SetNumTicks(100);
	AddProperty( pRFUII );

	// Register callbacks to update particle generator and icons
	// when properties change
	m_Data.m_Color.AddCallback(new prtyCallbackWrapper<fogFogObject>(this, &fogFogObject::FogChanged));
	m_Data.m_Mode.AddCallback(new prtyCallbackWrapper<fogFogObject>(this, &fogFogObject::FogChanged));
	m_Data.m_Density.AddCallback(new prtyCallbackWrapper<fogFogObject>(this, &fogFogObject::FogChanged));
	m_Data.m_Start.AddCallback(new prtyCallbackWrapper<fogFogObject>(this, &fogFogObject::FogChanged));
	m_Data.m_End.AddCallback(new prtyCallbackWrapper<fogFogObject>(this, &fogFogObject::FogChanged));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
fogFogObject::~fogFogObject()
{
}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string fogFogObject::GetPick3dName() const
{
	return "Fog";
}


//============================================================================
//	Data
//============================================================================


//--------------------------------------------------------------------
// Get values as a data structure
//--------------------------------------------------------------------
const fogFogData& fogFogObject::GetData() const
{
	return this->m_Data;
}

//--------------------------------------------------------------------
// Set from data structure
//--------------------------------------------------------------------
void fogFogObject::SetData(const fogFogData &i_Data)
{
	m_Data = i_Data;
}

//--------------------------------------------------------------------
// Mode property access
//--------------------------------------------------------------------
prtyEnum&	fogFogObject::PropertyMode()
{
	return m_Data.m_Mode;
}
const prtyEnum&	fogFogObject::GetPropertyMode() const
{
	return m_Data.m_Mode;
}

//--------------------------------------------------------------------
// Color property access
//--------------------------------------------------------------------
prtyColor&	fogFogObject::PropertyColor()
{
	return m_Data.m_Color;
}
const prtyColor&	fogFogObject::GetPropertyColor() const
{
	return m_Data.m_Color;
}

//--------------------------------------------------------------------
// Density property access
//--------------------------------------------------------------------
prtyFloat& fogFogObject::PropertyDensity()
{
	return m_Data.m_Density;
}
const prtyFloat& fogFogObject::GetPropertyDensity() const
{
	return m_Data.m_Density;
}

//--------------------------------------------------------------------
// Start property access
//--------------------------------------------------------------------
prtyFloat& fogFogObject::PropertyStart()
{
	return m_Data.m_Start;
}
const prtyFloat& fogFogObject::GetPropertyStart() const
{
	return m_Data.m_Start;
}

//--------------------------------------------------------------------
// End property access
//--------------------------------------------------------------------
prtyFloat& fogFogObject::PropertyEnd()
{
	return m_Data.m_End;
}
const prtyFloat& fogFogObject::GetPropertyEnd() const
{
	return m_Data.m_End;
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void fogFogObject::FogChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	api3dScene::SetFog( m_Data.m_Mode.GetValue(), 
						m_Data.m_Color.GetValue(), 
						m_Data.m_Start.GetValue(), 
						m_Data.m_End.GetValue(), 
						m_Data.m_Density.GetValue() );
}

//--------------------------------------------------------------------
// SetName
//--------------------------------------------------------------------
void fogFogObject::SetName(const nameString& i_Name)
{
    // Set name first, which may change the ID number
    nameObject::SetName(i_Name);

    // Make sure that the data matches our true name (including
    // ID number)
    m_Data.m_Name.SetValue(this->GetName());
}
