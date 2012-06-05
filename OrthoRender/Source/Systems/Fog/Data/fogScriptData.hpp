/********************************************************************************************\
**  fogScriptData.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#ifdef FOG_SCRIPTDATA_HPP
#error fogScriptData.hpp multiply included
#endif
#define FOG_SCRIPTDATA_HPP

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef FOG_FOGDATA_HPP
#include "Systems/Fog/Data/fogFogData.hpp"
#endif

#ifndef TMLN_CHANNELINFO_HPP
#include "Support/tmln/tmlnChannelInfo.hpp"
#endif

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif

#include <vector>
#include <map>


//============================================================================
//============================================================================

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
class fogScriptData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	fogScriptData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	fogScriptData(const fogScriptData& i_Data );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	explicit fogScriptData(const fogFogData& i_BaseData );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~fogScriptData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool operator == (const fogScriptData& i_Item);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	fogScriptData& operator=(const fogScriptData& i_Data);

	//------------------------------------------------------------------------
	//	data
	//------------------------------------------------------------------------
	fogFogData		m_BaseData;

	std::vector<tmlnDriverInfo*>	m_Drivers;
	std::map<std::string, tmlnChannelInfo> m_ChannelInfo;
};

