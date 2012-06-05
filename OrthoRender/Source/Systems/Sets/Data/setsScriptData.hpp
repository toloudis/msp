/********************************************************************************************\
**  setsScriptData.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#ifdef SETS_SCRIPTDATA_HPP
#error setsScriptData.hpp multiply included
#endif
#define SETS_SCRIPTDATA_HPP

#ifndef SETS_DATA_HPP
#include "Systems/Sets/Data/setsData.hpp"
#endif

#ifndef FGMT_FRAGMENTDATA_HPP
#include "Support/fgmt/fgmtFragmentData.hpp"
#endif
#ifndef FGMT_FRAGMENTSAODATA_HPP
#include "Support/fgmt/fgmtFragmentsAOData.hpp"
#endif
#ifndef MDL_MATERIALINFO_HPP
#include "Graphics/mdl/mdlMaterialInfo.hpp"
#endif
#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif

#ifndef ENV_STLHELPERS_HPP
#include "Core/env/envSTLHelpers.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif
#ifndef IT_STRINGUTIL_HPP
#include "Core/it/itStringUtil.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class setsScriptData
{
public:
	setsScriptData() 
	: m_BaseData()
	{
	}

	setsScriptData(const itString &i_Filename)
	: m_BaseData(i_Filename)
	{
		m_BaseData.m_Filename = i_Filename;
		m_BaseData.m_Name.SetValue(itStringUtil::GetStdString(i_Filename));
	}

	bool operator == (const setsScriptData& i_Item) const
	{
		return (this->m_BaseData == i_Item.m_BaseData);
	}

	setsScriptData& operator = (const setsScriptData& i_Item)
	{
		m_BaseData		= i_Item.m_BaseData;

		this->m_Materials = i_Item.m_Materials;
		this->m_Fragments = i_Item.m_Fragments;
		this->m_AOData = i_Item.m_AOData;

		return *this;
	}

	setsScriptData& operator = (const setsData& i_Item)
	{
		m_BaseData		= i_Item;

		return *this;
	}

	//
	//	data
	//
	setsData	m_BaseData;

	std::vector< shared_ptr<mdlMaterialInfo> > m_Materials;
	std::vector<fgmtFragmentData>	m_Fragments;
	fgmtFragmentsAOData m_AOData;
};


//============================================================================
//============================================================================
class setsListData
{
public:
	void AddItem(const itString &i_Filename)
	{
		m_SetItems.push_back(setsScriptData(i_Filename));
	}

	std::vector<setsScriptData> m_SetItems;
};

