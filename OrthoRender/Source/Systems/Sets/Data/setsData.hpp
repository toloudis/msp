/********************************************************************************************\
**  setsData.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#ifdef SETS_DATA_HPP
#error setsData.hpp multiply included
#endif
#define SETS_DATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif
#ifndef PRTY_NAME_HPP
#include "Core/prty/prtyName.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class setsData
{
public:
	setsData()
	:	m_bEditorVisible("Visible in Editor",true),
		m_Name("Name"),
		m_Filename("File Name")
	{
	}

	setsData(const itString &i_Filename)
	:	m_bEditorVisible("Visible in Editor",true),
		m_Name("Name"),
		m_Filename("File Name", i_Filename)
	{
	}

	bool operator == (const setsData& i_Item) const
	{
		return (m_Filename == i_Item.m_Filename);
	}

	setsData& operator = (const setsData& i_Item)
	{
		m_bEditorVisible	= i_Item.m_bEditorVisible;
		m_Name				= i_Item.m_Name;
		m_Filename			= i_Item.m_Filename;

		return *this;
	}

	//
	//	data
	//
	prtyBoolean		m_bEditorVisible;
	prtyName		m_Name;
	prtyFileName	m_Filename;
};
