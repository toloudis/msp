/*****************************************************************************
**	fcmdModeLocation.hpp
**
**		Mode class for the the location mode
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef FCMD_MODELOCATION_HPP
#error fcmdModeLocation.hpp multiply included
#endif
#define FCMD_MODELOCATION_HPP

#ifndef FCMD_MODETEMPLATE_HPP
#include "FCSupport/fcmd/fcmdModeTemplate.hpp"
#endif

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif


//============================================================================
//============================================================================
class fcmdModeLocation : public fcmdModeTemplate
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	fcmdModeLocation();
	fcmdModeLocation(const fsLocator& i_Directory);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~fcmdModeLocation();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Activate();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void DeActivate();

	///-----------------------------------------------------------------------
	/// Do any mode thinking
	///-----------------------------------------------------------------------
	virtual void Think();

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

	///---------------------------------------------------------------------------
	/// Perform the mode specific operations when an element needs to 
	/// execute an action.
	///---------------------------------------------------------------------------
	void ProcessCustomIcon(const fsLocator& i_ElementDirectory);

	///-----------------------------------------------------------------------
	/// Perform the mode specific operations when dealing with props
	///-----------------------------------------------------------------------
	void ProcessProps(const fsLocator& i_Element, const fsLocator& i_ElementDirectory);

private:
	///-----------------------------------------------------------------------
	/// ProcessFiles
	///-----------------------------------------------------------------------
	void ProcessFiles(const fsLocator& i_ElementDirectory, const std::vector<fsLocator>& i_Files);

	///-----------------------------------------------------------------------
	/// ProcessFile
	/// Returns true if the file was processed.
	///-----------------------------------------------------------------------
	bool ProcessFile(const fsLocator& i_ElementDirectory, const fsLocator& i_File);

private:
	fsLocator m_ModeDirectory;
	itString m_CurrentSubject;
	itString m_CurrentCategory;
	itString m_CurrentElement;
};