/********************************************************************************************\
**  aoScriptData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#ifdef AO_SCRIPTDATA_HPP
#error aoScriptData.hpp multiply included
#endif
#define AO_SCRIPTDATA_HPP

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef AO_AODATA_HPP
#include "Systems/AmbientOcclusion/Data/aoAOData.hpp"
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
class aoScriptData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	aoScriptData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	aoScriptData(const aoScriptData& i_Data );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	explicit aoScriptData(const aoAOData& i_BaseData );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~aoScriptData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool operator == (const aoScriptData& i_Item);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	aoScriptData& operator=(const aoScriptData& i_Data);

	//------------------------------------------------------------------------
	//	data
	//------------------------------------------------------------------------
	aoAOData		m_BaseData;

	std::vector<tmlnDriverInfo*>	m_Drivers;
	std::map<std::string, tmlnChannelInfo> m_ChannelInfo;
	std::vector< shared_ptr<xtraPropertyData> > m_CustomProperties;
};

