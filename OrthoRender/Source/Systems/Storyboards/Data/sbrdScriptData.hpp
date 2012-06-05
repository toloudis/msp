/********************************************************************************************\
**  sbrdScriptData.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#ifdef SBRD_SCRIPTDATA_HPP
#error sbrdScriptData.hpp multiply included
#endif
#define SBRD_SCRIPTDATA_HPP

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef SBRD_OBJECTDATA_HPP
#include "Systems/Storyboards/Data/sbrdObjectData.hpp"
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
namespace sbrdObjectDataTypes
{
	typedef int BillboardID;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
class sbrdScriptData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	sbrdScriptData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	sbrdScriptData(const sbrdScriptData& i_Data );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	explicit sbrdScriptData(const sbrdObjectData& i_BaseData );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	sbrdScriptData(const itString& i_Filename );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	sbrdScriptData(	const itString& i_Filename,
					const maPoint3d& i_Position,
					float i_Scale );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~sbrdScriptData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool operator == (const sbrdScriptData& i_Item);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	sbrdScriptData& operator=(const sbrdScriptData& i_Data);

	//------------------------------------------------------------------------
	//	data
	//------------------------------------------------------------------------
	sbrdObjectData		m_BaseData;

	std::vector<tmlnDriverInfo*>	m_Drivers;
	std::map<std::string, tmlnChannelInfo> m_ChannelInfo;
};


//
//
class sbrdObjectsData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Add(const itString& i_Filename,
			 const maPoint3d& i_Position,
			 float i_Scale);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Clear();

	//------------------------------------------------------------------------
	//	data
	//------------------------------------------------------------------------
	std::vector<sbrdScriptData> m_Items;
};

