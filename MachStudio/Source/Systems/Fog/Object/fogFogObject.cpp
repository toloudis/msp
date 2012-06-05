/*****************************************************************************
**	fogFogObject.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Fog/Object/fogFogObject.hpp"

// library
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyColorRGBAEditUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/gpx/gpxFog.hpp"


//----------------------------------------------------------------------------
// This object takes ownership of the arguments passed in
//----------------------------------------------------------------------------
fogFogObject::fogFogObject(	)
{
	// Create thread-safe proxy
	m_pFogProxy = new gpxFog();

	// Register the properties so they can be displayed to the user
	//

	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bEnable), "Parameters", "Enable Fog") );
	
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bWorldOrientation), "Parameters", "World Orientation or Camera Orientation") );

	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyColorRGBAEditUIInfo(&(m_Data.m_Color), "Parameters", "Color");
	AddProperty( pPUII );
	pPUII = new prtyComboBoxUIInfo(&(m_Data.m_Mode), "Parameters", "Fog Decay Function");
	AddProperty( pPUII );

	prtyRangedFloatUIInfo* pRFUII  ;
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Density), "Parameters", "Fog Density");
	pRFUII->SetMinimum(0);
	pRFUII->SetMaximum(10);
	pRFUII->SetDecimalPlaces(2);
	pRFUII->SetNumTicks(100);
	m_MaxDensity = pRFUII->GetMaximum();
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Start), "Parameters", "Fog Start Depth");
	pRFUII->SetMinimum(0);
	pRFUII->SetMaximum(100);
	pRFUII->SetDecimalPlaces(2);
	pRFUII->SetNumTicks(100);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_End), "Parameters", "Fog End Depth");
	pRFUII->SetMinimum(0);
	pRFUII->SetMaximum(100);
	pRFUII->SetDecimalPlaces(2);
	pRFUII->SetNumTicks(100);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_AltitudeStart), "Parameters", "Fog Start Height");
	pRFUII->SetMinimum(0);
	pRFUII->SetMaximum(100);
	pRFUII->SetDecimalPlaces(2);
	pRFUII->SetNumTicks(100);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_AltitudeEnd), "Parameters", "Fog End Height");
	pRFUII->SetMinimum(0);
	pRFUII->SetMaximum(100);
	pRFUII->SetDecimalPlaces(2);
	pRFUII->SetNumTicks(100);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_AltitudeDensity), "Parameters", "Fog Height Density");
	pRFUII->SetMinimum(0);
	pRFUII->SetMaximum(10);
	pRFUII->SetDecimalPlaces(2);
	pRFUII->SetNumTicks(100);
	m_MaxHeightDensity = pRFUII->GetMaximum();
	AddProperty( pRFUII );


	// Register callbacks to update particle generator and icons
	// when properties change
	m_Data.m_bEnable.AddCallback(new prtyCallbackWrapper<fogFogObject>(this, &fogFogObject::FogChanged));
	m_Data.m_bWorldOrientation.AddCallback(new prtyCallbackWrapper<fogFogObject>(this, &fogFogObject::FogChanged));
	m_Data.m_Color.AddCallback(new prtyCallbackWrapper<fogFogObject>(this, &fogFogObject::FogChanged));
	m_Data.m_Mode.AddCallback(new prtyCallbackWrapper<fogFogObject>(this, &fogFogObject::FogChanged));
	m_Data.m_Density.AddCallback(new prtyCallbackWrapper<fogFogObject>(this, &fogFogObject::FogChanged));
	m_Data.m_Start.AddCallback(new prtyCallbackWrapper<fogFogObject>(this, &fogFogObject::FogChanged));
	m_Data.m_End.AddCallback(new prtyCallbackWrapper<fogFogObject>(this, &fogFogObject::FogChanged));
	m_Data.m_AltitudeStart.AddCallback(new prtyCallbackWrapper<fogFogObject>(this, &fogFogObject::FogChanged));
	m_Data.m_AltitudeEnd.AddCallback(new prtyCallbackWrapper<fogFogObject>(this, &fogFogObject::FogChanged));
	m_Data.m_AltitudeDensity.AddCallback(new prtyCallbackWrapper<fogFogObject>(this, &fogFogObject::FogChanged));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
fogFogObject::~fogFogObject()
{
	// delete the proxy
	delete m_pFogProxy;
}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string fogFogObject::GetDisplayName() const
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
// AltitudeStart property access
//--------------------------------------------------------------------
prtyFloat& fogFogObject::PropertyAltitudeStart()
{
	return m_Data.m_AltitudeStart;
}
const prtyFloat& fogFogObject::GetPropertyAltitudeStart() const
{
	return m_Data.m_AltitudeStart;
}

//--------------------------------------------------------------------
// AltitudeEnd property access
//--------------------------------------------------------------------
prtyFloat& fogFogObject::PropertyAltitudeEnd()
{
	return m_Data.m_AltitudeEnd;
}
const prtyFloat& fogFogObject::GetPropertyAltitudeEnd() const
{
	return m_Data.m_AltitudeEnd;
}

//--------------------------------------------------------------------
// m_AltitudeDensity property access
//--------------------------------------------------------------------
prtyFloat& fogFogObject::PropertyAltitudeDensity()
{
	return m_Data.m_AltitudeDensity;
}
const prtyFloat& fogFogObject::GetPropertyAltitudeDensity() const
{
	return m_Data.m_AltitudeDensity;
}

//--------------------------------------------------------------------
// m_bEnable property access
//--------------------------------------------------------------------
prtyBoolean& fogFogObject::PropertybEnable()
{
	return m_Data.m_bEnable;
}
const prtyBoolean& fogFogObject::GetPropertybEnable() const
{
	return m_Data.m_bEnable;
}

//--------------------------------------------------------------------
// m_bWorldOrientation property access
//--------------------------------------------------------------------
prtyBoolean& fogFogObject::PropertybWorldOrientation()
{
	return m_Data.m_bWorldOrientation;
}
const prtyBoolean& fogFogObject::GetPropertybWorldOrientation() const
{
	return m_Data.m_bWorldOrientation;
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void fogFogObject::FogChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	fogParams fp;

	if(m_Data.m_bEnable.GetValue())
	{ 
		
		switch (m_Data.m_Mode.GetValue())
		{
			case fogFogData::e_Linear:		fp.m_nMode = g3dType::e_FogModeLinear; break;
			case fogFogData::e_Exponential: fp.m_nMode = g3dType::e_FogModeExp; break;
			case fogFogData::e_ExponentialSq: fp.m_nMode = g3dType::e_FogModeExp2; break;
			default:		fp.m_nMode = g3dType::e_FogModeNone; break;
		}
	}
	else
	{
		fp.m_nMode = g3dType::e_FogModeNone;
	}

	fp.m_bUseWorld		= m_Data.m_bWorldOrientation.GetValue();
	fp.m_Color			= m_Data.m_Color.GetValue();
	fp.m_fStart			= m_Data.m_Start.GetValue();
	fp.m_fEnd			= m_Data.m_End.GetValue();

	//attempting to make the density slider more intuitive when moving the
	//slider in the positive direction to make the density get larger.
	fp.m_fDensity		= m_MaxDensity - m_Data.m_Density.GetValue();
	fp.m_fAltitudeStart = m_Data.m_AltitudeStart.GetValue();
	fp.m_fAltitudeEnd	= m_Data.m_AltitudeEnd.GetValue();
	fp.m_fAltitudeDensity = m_MaxHeightDensity - m_Data.m_AltitudeDensity.GetValue();
	//fp.m_fbEnable		= m_Data.m_bEnable.GetValue();

	// Use proxy instead
	m_pFogProxy->SetFog( fp );
	//api3dScene::SetFog( fp );
	
}

//--------------------------------------------------------------------
// SetName
//--------------------------------------------------------------------
//void fogFogObject::SetName(const nameString& i_Name)
//{
//    // Set name first, which may change the ID number
//    nameObject::SetName(i_Name);
//
//    // Make sure that the data matches our true name (including
//    // ID number)
//    m_Data.m_Name.SetValue(this->GetName());
//}
