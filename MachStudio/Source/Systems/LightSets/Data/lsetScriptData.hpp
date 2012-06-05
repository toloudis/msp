/********************************************************************************************\
**  lsetScriptData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#ifdef LSET_SCRIPTDATA_HPP
#error lsetScriptData.hpp multiply included
#endif
#define LSET_SCRIPTDATA_HPP

#ifndef LSET_DATA_HPP
#include "Systems/LightSets/Data/lsetData.hpp"
#endif
#ifndef TMLN_CHANNELINFO_HPP
#include "Support/tmln/tmlnChannelInfo.hpp"
#endif
#ifndef XTRA_PROPERTYDATA_HPP
#include "Support/xtra/xtraPropertyData.hpp"
#endif 

#include <vector>
#include <map>


//============================================================================
//	Forward References
//============================================================================
class tmlnDriverInfo;


//============================================================================
//============================================================================
class lsetScriptData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	lsetScriptData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	lsetScriptData(const lsetScriptData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	explicit lsetScriptData(const lsetData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~lsetScriptData();

	//-------------------------------------------------
	bool operator == (const lsetScriptData& i_Item);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	lsetScriptData& operator=(const lsetScriptData& i_Data);

public:
	lsetData		m_BaseData;

	std::vector<tmlnDriverInfo*>	m_Drivers;
	std::map<std::string, tmlnChannelInfo> m_ChannelInfo;
	std::vector< shared_ptr<xtraPropertyData> > m_CustomProperties;
};


//============================================================================
//============================================================================
class lsetLightSetsData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Clear();

	std::vector<lsetScriptData> m_Items;
};

