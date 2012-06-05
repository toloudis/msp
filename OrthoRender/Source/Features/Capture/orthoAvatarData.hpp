/********************************************************************************************\
**  orthoAvatarData.hpp
**
**		Avatar data
**
**  Extra Large Technology
**  Copyright(C) 2008 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifdef ORTHO_AVATARDATA_HPP
#error orthoAvatarData.hpp multiply included
#endif
#define ORTHO_AVATARDATA_HPP

#ifndef PRTY_COLOR_HPP
#include "Core/prty/prtyColor.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif
#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class orthoAvatarItemData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	orthoAvatarItemData()
	:	m_PartName("Name of the Part"),
		m_Category("Category of the Part"),
		m_JointName("Name of the joint"),
		m_Color("Material Color"),
		m_bVisible(false)
	{
	};

public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	prtyText	m_PartName;		//	not used
	prtyText	m_Category;		//	not used
	prtyText	m_JointName;
	prtyColor	m_Color;
	bool		m_bVisible;		//	not stored/read
};

//============================================================================
//============================================================================
class orthoAvatarListData
{
public:
	orthoAvatarListData()
	:	m_Name("Name of Object"),
		m_FileName("Filename of Object")
	{
		m_Parts.resize(20);
	};

public:
	//---------------------------------------------------------------------------
	//	data
	//---------------------------------------------------------------------------

	prtyText	m_Name;
	prtyText	m_FileName;

	std::vector<orthoAvatarItemData>	m_Parts;
};
