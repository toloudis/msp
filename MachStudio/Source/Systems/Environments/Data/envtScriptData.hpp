/********************************************************************************************\
**  envtScriptData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#ifdef ENVT_SCRIPTDATA_HPP
#error envtScriptData.hpp multiply included
#endif
#define ENVT_SCRIPTDATA_HPP

#ifndef ENVT_DATA_HPP
#include "Systems/Environments/Data/envtData.hpp"
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
class envtScriptData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	envtScriptData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	envtScriptData(const envtScriptData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	explicit envtScriptData(const envtData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~envtScriptData();

	//-------------------------------------------------
	bool operator == (const envtScriptData& i_Item);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	envtScriptData& operator=(const envtScriptData& i_Data);

public:
	envtData		m_BaseData;

	std::vector<tmlnDriverInfo*>	m_Drivers;
	std::map<std::string, tmlnChannelInfo> m_ChannelInfo;
	std::vector< shared_ptr<xtraPropertyData> > m_CustomProperties;
};


//============================================================================
//============================================================================
class envtEnvironmentsData
{
public:
	//------------------------------------------------------------------------
	// always init the list with one default environment.
	//------------------------------------------------------------------------
	envtEnvironmentsData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Clear();

	envtScriptData m_DefaultEnv;
	envtScriptData m_SwlEnv;
	std::vector<envtScriptData> m_Items;
};

