/********************************************************************************************\
**  ptltScriptData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#ifdef PTLT_SCRIPTDATA_HPP
#error ptltScriptData.hpp multiply included
#endif
#define PTLT_SCRIPTDATA_HPP

#ifndef PTLT_DATA_HPP
#include "Systems/PtLt/Data/ptltData.hpp"
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
// Connection data is used to track connections during delete/restore
//	and duplication. It is not written to file.
//============================================================================
struct ptltConnectionData
{
	nameString m_LightSetName;
	nameString m_LayerName;
	std::vector<nameString> m_GroupNames;
};

//============================================================================
//============================================================================
class ptltScriptData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	ptltScriptData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	ptltScriptData(const ptltScriptData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	explicit ptltScriptData(const ptltData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~ptltScriptData();

	//-------------------------------------------------
	bool operator == (const ptltScriptData& i_Item);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	ptltScriptData& operator=(const ptltScriptData& i_Data);

public:
	ptltData		m_BaseData;
	ptltConnectionData m_ConnectionData;

	std::vector<tmlnDriverInfo*>	m_Drivers;
	std::map<std::string, tmlnChannelInfo> m_ChannelInfo;
	std::vector< shared_ptr<xtraPropertyData> > m_CustomProperties;
};


//============================================================================
//============================================================================
class ptltPointLightsData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Clear();

	std::vector<ptltScriptData> m_Items;
};

