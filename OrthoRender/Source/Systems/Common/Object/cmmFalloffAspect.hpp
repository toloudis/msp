/*****************************************************************************
**  cmmFalloffAspect.hpp
**
**      A cmmFalloffAspect is a base class for lights that have
**	falloff properties.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PTLT_FALLOFFASPECT_HPP
#error cmmFalloffAspect.hpp multiply included
#endif
#define PTLT_FALLOFFASPECT_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_ENUM_HPP
#include "Core/prty/prtyEnum.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif



//============================================================================
//	forward references
//============================================================================
class prtyObject;
class prtyPoint3d;
class prtyVector3dEditUpDownUIInfo;
class prtyNumericUpDownUIInfo;


//============================================================================
//============================================================================
class cmmFalloffAspect
{
public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cmmFalloffAspect();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~cmmFalloffAspect();

		//----------------------------------------------------------------------------
		// Register the properties so they can be displayed to the user.
		// Pass in the falloff property of the main class that this
		// aspect should control.
		//----------------------------------------------------------------------------
		void RegisterProperties(prtyObject* i_pObject,
								prtyPoint3d* i_pFalloff);

		//--------------------------------------------------------------------
		// Initialize the FalloffType property based on the D3D Falloff value
		//--------------------------------------------------------------------
		void InitializeFalloffType();

		//--------------------------------------------------------------------
		// Returns true if the falloff range 3D icon should be visible.
		//--------------------------------------------------------------------
		bool ShouldShowFalloffIcon() const;

protected:
		//--------------------------------------------------------------------
		// Override this function to update the icon that displays the 
		//--------------------------------------------------------------------
		virtual void UpdateFalloffIcon() = 0;

		// These properties are not animatable or saved to file:
		prtyBoolean		m_ShowFalloff;
		prtyEnum		m_FalloffType;		// custom, constant, linear, or quadratic falloff
		prtyFloat		m_FalloffPercent;	// percent of intensity left at Falloff Range distance away
		prtyFloat		m_FalloffRange;		// distance away from light when intensity reaches certain percentage

private:
		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void FalloffChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void FalloffTypeChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void FalloffRangeChanged(prtyProperty *i_pProperty, bool i_bDirty);

		prtyPoint3d*	m_pFalloff;			// D3D property from another class that this class tracks

		prtyVector3dEditUpDownUIInfo* m_pFalloffD3DUI;
		prtyNumericUpDownUIInfo* m_pFalloffRangeUI;
};


