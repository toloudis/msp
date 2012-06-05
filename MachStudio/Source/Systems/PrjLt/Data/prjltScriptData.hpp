/********************************************************************************************\
**  prjltScriptData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#ifdef PRJLT_SCRIPTDATA_HPP
#error prjltScriptData.hpp multiply included
#endif
#define PRJLT_SCRIPTDATA_HPP

#ifndef PRJLT_DATA_HPP
#include "Systems/PrjLt/Data/prjltData.hpp"
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
struct prjltConnectionData
{
	nameString m_LightSetName;
	nameString m_LayerName;
	std::vector<nameString> m_GroupNames;
};

//============================================================================
//============================================================================
class prjltScriptData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	prjltScriptData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	prjltScriptData(const prjltScriptData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	prjltScriptData(const prjltData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~prjltScriptData();

	//-------------------------------------------------
	bool operator == (const prjltScriptData& i_Item);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	prjltScriptData& operator=(const prjltScriptData& i_Data);

public:
	//------------------------------------------------------------------------
	//	data
	//------------------------------------------------------------------------
	prjltData	m_BaseData;
	prjltConnectionData m_ConnectionData;

	std::vector<tmlnDriverInfo*>	m_Drivers;
	std::map<std::string, tmlnChannelInfo> m_ChannelInfo;
	std::vector< shared_ptr<xtraPropertyData> > m_CustomProperties;
};


//============================================================================
//============================================================================
class prjltProjectLightsData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Clear();

	std::vector<prjltScriptData> m_Items;
};

