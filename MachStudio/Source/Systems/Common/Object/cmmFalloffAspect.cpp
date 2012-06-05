/*****************************************************************************
**  cmmFalloffAspect.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/Object/cmmFalloffAspect.hpp"

#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyObject.hpp"
#include "Core/prty/prtyPoint3d.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"

namespace
{
	//============================================================================
	//	A point light is attenuated according to this formula:
	//
	//		A =				1
	//			-------------------------
	//			a0 + a1 * D + a2 * D^2,
	//
	//	where D is the distance from the light to the surface it is
	//	illuminating and a0, a1, and a2 are the falloff coefficients.
	//	Some (but not all) of the coefficients may be zero.
	//
	// Solving for distance:
	//		(A*a2) * D^2 + (A*a1) * D + (A*a0 - 1) = 0
	// Then use quadratic equation:
	// x = (-b +/- sqrt(b^2 - 4*a*c)) / 2*a
	//============================================================================

	enum FalloffTypes
	{
		e_Custom = 0,
		e_Constant = 1,
		e_Linear = 2,
		e_Quadratic = 3
	};

	maVector3d compute_falloff( FalloffTypes i_Type, 
								float i_Percent, 
								float i_Range)
	{
		DBG_ASSERT((i_Percent>0 && i_Percent<1), "Unallowed falloff percent: " << i_Percent);

		switch (i_Type)
		{
		default:
		case e_Custom:
			DBG_ASSERT(false, "Unexpected falloff type");
			return maVector3d(1,0,0);
		case e_Constant:
			return maVector3d(1,0,0);
		case e_Linear:
			{
				float den = i_Percent * i_Range;
				return maVector3d(1, ((den > 0) ? (1 - i_Percent) / den : 1), 0);
			}
		case e_Quadratic:
			{
				float den = i_Percent * i_Range * i_Range;
				return maVector3d(1, 0, ((den > 0) ? (1 - i_Percent) / den : 1));
			}
		}
	}

	bool compute_range(const maVector3d &i_Falloff, 
					    float i_Percent, 
						float &o_Range)
	{
		if (i_Percent <= 0) return false;

		float A = (i_Percent * i_Falloff[2]);
		float B = (i_Percent * i_Falloff[1]);
		float C = (i_Percent * i_Falloff[0] - 1);
		if (A==0)
		{
			if (B == 0) return false; // constant

			// linear equation
			float range = -C / B;
			if (range > 0)
			{
				o_Range = range;
				return true;
			}
		}
		else
		{
			// quadratic equation
			float discr = B*B - 4*A*C;
			if (discr < 0) return false;

			float sol = (-1*B + sqrtf(discr)) / (2*A);
			if (sol > 0)
			{
				o_Range = sol;
				return true;
			}
		}
		return false;
	}
		

} // end of namespace

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmmFalloffAspect::cmmFalloffAspect()
:	m_ShowFalloff("Show Falloff", false),
	m_FalloffType("Falloff Type", e_Constant),
	m_FalloffPercent("Falloff Percent", 0.5f),
	m_FalloffRange("Falloff Range", 100.0f),
	m_pFalloff(NULL)
{
	// Set up property ranges
	//m_FalloffPercent.SetMaximum(0.9f);
	//m_FalloffPercent.SetMinimum(0.1f);
	//m_FalloffRange.SetMinimum(1.0f);

	// Set up enumeration for manip mode
	m_FalloffType.SetEnumTag(e_Custom, "Custom");
	m_FalloffType.SetEnumTag(e_Constant, "Constant");
	m_FalloffType.SetEnumTag(e_Linear, "Linear");
	m_FalloffType.SetEnumTag(e_Quadratic, "Quadratic");
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmmFalloffAspect::~cmmFalloffAspect()
{
}

//----------------------------------------------------------------------------
// Register the properties so they can be displayed to the user.
// Pass in the falloff property of the main class that this
// aspect should control.
//----------------------------------------------------------------------------
void cmmFalloffAspect::RegisterProperties(prtyObject* i_pObject,
										   prtyPoint3d*	i_pFalloff)
{
	m_pFalloff = i_pFalloff;

	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;

	// Falloff group
	i_pObject->AddProperty( new prtyCheckBoxUIInfo(&(m_ShowFalloff), "Falloff", "Display Falloff Range Sphere") );
	pPUII = new prtyComboBoxUIInfo(&(m_FalloffType), "Falloff", "Type of falloff");
	i_pObject->AddProperty( pPUII );
	m_pFalloffD3DUI  = new prtyVector3dEditUpDownUIInfo(m_pFalloff, "Falloff", "D3D Falloff of the object");
	m_pFalloffD3DUI->SetIncrement( 0.1f, 0.01f, 0.001f );
	m_pFalloffD3DUI->SetDecimalPlaces(5);
	m_pFalloffD3DUI->SetReadOnly(true);
	i_pObject->AddProperty( m_pFalloffD3DUI );
	m_pFalloffRangeUI = new prtyNumericUpDownUIInfo(&(m_FalloffRange), "Falloff", "Range when light hits percentage");
	//m_pFalloffRangeUI->SetMinimum( 1.0f );
	m_pFalloffRangeUI->SetIncrement(1.0f);
	m_pFalloffRangeUI->SetReadOnly(true);
	i_pObject->AddProperty( m_pFalloffRangeUI );
	prtyRangedFloatUIInfo* pRFUII  = new prtyRangedFloatUIInfo(&(m_FalloffPercent), "Falloff", "Percentage of intensity used by falloff range");
	pRFUII->SetMinimum(0.1f);
	pRFUII->SetMaximum(0.9f);
	pRFUII->SetNumTicks(80);
	i_pObject->AddProperty( pRFUII );

	// Register callback on the persistent property
	m_pFalloff->AddCallback(new prtyCallbackWrapper<cmmFalloffAspect>(this, &cmmFalloffAspect::FalloffChanged));

	// Callbacks for the intermediate properties which are just for the UI
	m_FalloffType.AddCallback(new prtyCallbackWrapper<cmmFalloffAspect>(this, &cmmFalloffAspect::FalloffTypeChanged));
	m_FalloffPercent.AddCallback(new prtyCallbackWrapper<cmmFalloffAspect>(this, &cmmFalloffAspect::FalloffChanged));
	m_FalloffRange.AddCallback(new prtyCallbackWrapper<cmmFalloffAspect>(this, &cmmFalloffAspect::FalloffRangeChanged));
	m_ShowFalloff.AddCallback(new prtyCallbackWrapper<cmmFalloffAspect>(this, &cmmFalloffAspect::FalloffRangeChanged));
}

//--------------------------------------------------------------------
// Initialize the FalloffType property based on the D3D Falloff value
//--------------------------------------------------------------------
void cmmFalloffAspect::InitializeFalloffType()
{
	if (m_pFalloff)
	{
		// Try to detect the type of falloff
		maVector3d falloff = m_pFalloff->GetValue();
		bool bLinearComp = (falloff[1] != 0.0f);
		bool bQuadComp = (falloff[2] != 0.0f);

		if (bQuadComp && bLinearComp)
			m_FalloffType.SetValue(e_Custom);
		else if (bQuadComp)
			m_FalloffType.SetValue(e_Quadratic);
		else if (bLinearComp)
			m_FalloffType.SetValue(e_Linear);
		else 
			m_FalloffType.SetValue(e_Constant);
	}
	else
		m_FalloffType.SetValue(e_Constant);
}

//--------------------------------------------------------------------
// Returns true if the falloff range 3D icon should be visible.
//--------------------------------------------------------------------
bool cmmFalloffAspect::ShouldShowFalloffIcon() const
{
	FalloffTypes type = (FalloffTypes) m_FalloffType.GetValue();
	return (m_ShowFalloff.GetValue() && (type != e_Constant));
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void cmmFalloffAspect::FalloffChanged(prtyProperty *i_pProperty, bool i_bDirty)
{		
	// This callback is triggered when the 3-value D3D falloff property is changed.
	// Note: It is also called when the falloff percentage changes because this
	// causes a change to the falloff range computation.

	// Compute the new values for the more user-friendly falloff properties.
	float range = 0.0;
	if (compute_range(m_pFalloff->GetValue(), m_FalloffPercent.GetValue(), range))
	{
		m_FalloffRange.SetValue( range );
	}

	UpdateFalloffIcon();
	
}
void cmmFalloffAspect::FalloffTypeChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// This callback is triggered when the type of the light's falloff
	// is changed. This causes other properties to be enabled or disabled.

	switch (m_FalloffType.GetValue())
	{
		case e_Custom:
		{
			m_pFalloffD3DUI->SetReadOnly(false);
			m_pFalloffRangeUI->SetReadOnly(true);
		}
		break;
		case e_Constant:
		{
			m_pFalloffD3DUI->SetReadOnly(true);
			m_pFalloffRangeUI->SetReadOnly(true);
		}
		break;
		case e_Linear:
		case e_Quadratic:
		{
			m_pFalloffD3DUI->SetReadOnly(true);
			m_pFalloffRangeUI->SetReadOnly(false);
		}
		break;
	}
	m_pFalloffD3DUI->UpdateControl();
	m_pFalloffRangeUI->UpdateControl();

	if (i_bDirty)
	{
		FalloffTypes type = (FalloffTypes) m_FalloffType.GetValue();
		if (type != e_Custom)
		{
			maVector3d falloff = compute_falloff(type, m_FalloffPercent.GetValue(), m_FalloffRange.GetValue());
			m_pFalloff->SetValue(falloff);
		}
	}

	UpdateFalloffIcon();
}
void cmmFalloffAspect::FalloffRangeChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// This callback is triggered when attributes of the light's falloff
	// are changed. The changes are converted into a new vector3d of D3D falloff values
	//
	
	// We only want to do something if the property changed because of
	//	the user interface sliders. 
	if (i_bDirty)
	{
		FalloffTypes type = (FalloffTypes) m_FalloffType.GetValue();
		if (type != e_Custom)
		{
			maVector3d falloff = compute_falloff(type, m_FalloffPercent.GetValue(), m_FalloffRange.GetValue());
			m_pFalloff->SetValue(falloff);
		}
	}

	UpdateFalloffIcon();
}
