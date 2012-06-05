/*****************************************************************************
**	fcmdModeCast.hpp
**
**		Mode class for the the cast mode
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef FCMD_MODECAST_HPP
#error fcmdModeCast.hpp multiply included
#endif
#define FCMD_MODECAST_HPP

#ifndef FCMD_MODETEMPLATE_HPP
#include "FCSupport/fcmd/fcmdModeTemplate.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif


//============================================================================
//============================================================================
class fcmdModeCast : public fcmdModeTemplate
{ 
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	fcmdModeCast();
	fcmdModeCast(const fsLocator& i_Directory);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~fcmdModeCast();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Activate();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void DeActivate();

	///-----------------------------------------------------------------------
	/// Do any mode thinking
	///-----------------------------------------------------------------------
	void Think();

	///-----------------------------------------------------------------------
	/// Perform the mode specific operations when an element needs to 
	/// execute an action.
	///-----------------------------------------------------------------------
	void ProcessSubject(const fsLocator& i_SubjectDirectory);
	
	///-----------------------------------------------------------------------
	/// Perform the mode specific operations when an element needs to 
	/// execute an action.
	///-----------------------------------------------------------------------
	void ProcessCategory(const fsLocator& i_CategoryDirectory);

	///-----------------------------------------------------------------------
	/// Perform the mode specific operations when an element needs to 
	/// execute an action.
	///-----------------------------------------------------------------------
	void ProcessElement(const fsLocator& i_ElementDirectory);

private:
	itString m_CurrentSubject;
	itString m_CurrentCategory;
	itString m_CurrentElement;
};