/********************************************************************************************\
**  trfnScriptData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2010 - All Rights Reserved
\********************************************************************************************/
#ifdef TRFN_SCRIPTDATA_HPP
#error trfnScriptData.hpp multiply included
#endif
#define TRFN_SCRIPTDATA_HPP

#ifndef TRFN_DATA_HPP
#include "Systems/Transforms/Data/trfnData.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif
#ifndef LTST_LIGHTSETSDATA_HPP
#include "Support/ltst/ltstLightSetsData.hpp"
#endif 
#ifndef TMLN_CHANNELINFO_HPP
#include "Support/tmln/tmlnChannelInfo.hpp"
#endif
#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef XTRA_PROPERTYDATA_HPP
#include "Support/xtra/xtraPropertyData.hpp"
#endif 


#include <map>
#include <vector>

//============================================================================
// Connection data is used to track connections during delete/restore
//	and duplication. It is not written to file.
//============================================================================
struct trfnConnectionData
{
	//nameString m_EnvironmentName;
	std::vector<ltstLightSetObjectData> m_LightSets;
	//nameString m_LayerName;
	//std::vector<nameString> m_GroupNames;
	//std::vector<nameString> m_RLayerNames;
};

//============================================================================
//============================================================================
class trfnScriptData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	trfnScriptData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	trfnScriptData(const trfnScriptData& i_Data );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	explicit trfnScriptData(const trfnData& i_BaseData );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~trfnScriptData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool operator == (const trfnScriptData& i_Item);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	trfnScriptData& operator=(const trfnScriptData& i_Data);

	//------------------------------------------------------------------------
	//	data
	//------------------------------------------------------------------------
	trfnData	m_BaseData;
	trfnConnectionData m_ConnectionData;

	std::vector<tmlnDriverInfo*>	m_Drivers;
	std::map<std::string, tmlnChannelInfo> m_ChannelInfo;
	std::vector< shared_ptr<xtraPropertyData> > m_CustomProperties;
};


//============================================================================
//============================================================================
class trfnTransformsData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Clear();

	//------------------------------------------------------------------------
	//	data
	//------------------------------------------------------------------------
	std::vector<trfnScriptData> m_Items;
};

