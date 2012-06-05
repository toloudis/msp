/********************************************************************************************\
**  giScriptData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2010 - All Rights Reserved
\********************************************************************************************/
#ifdef GI_SCRIPTDATA_HPP
#error giScriptData.hpp multiply included
#endif
#define GI_SCRIPTDATA_HPP

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef GI_GIDATA_HPP
#include "Systems/GlobalIllumination/Data/giGIData.hpp"
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

#include <vector>
#include <map>


//============================================================================
//============================================================================
class giScriptData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	giScriptData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	giScriptData(const giScriptData& i_Data );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	explicit giScriptData(const giGIData& i_BaseData );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~giScriptData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool operator == (const giScriptData& i_Item);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	giScriptData& operator=(const giScriptData& i_Data);

	//------------------------------------------------------------------------
	//	data
	//------------------------------------------------------------------------
	giGIData		m_BaseData;

	std::vector<tmlnDriverInfo*>	m_Drivers;
	std::map<std::string, tmlnChannelInfo> m_ChannelInfo;
	std::vector< shared_ptr<xtraPropertyData> > m_CustomProperties;
};

