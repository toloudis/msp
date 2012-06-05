/********************************************************************************************\
**  aoScriptData.hpp
**
**
**  Extra Large Technology
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

#include <vector>
#include <map>


//============================================================================
//============================================================================

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
template<class BASEDATA> class cmmScriptData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cmmScriptData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cmmScriptData(const cmmScriptData& i_Data );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	explicit cmmScriptData(const BASEDATA& i_BaseData );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~cmmScriptData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool operator == (const cmmScriptData& i_Item);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cmmScriptData& operator=(const cmmScriptData& i_Data);

	//------------------------------------------------------------------------
	//	data
	//------------------------------------------------------------------------
	BASEDATA		m_BaseData;

	std::vector<tmlnDriverInfo*>	m_Drivers;
	std::map<std::string, tmlnChannelInfo> m_ChannelInfo;
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template<class BASEDATA> cmmScriptData<BASEDATA>::cmmScriptData()
: m_BaseData()
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
template<class BASEDATA> cmmScriptData<BASEDATA>::cmmScriptData(const cmmScriptData& i_Data )
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template<class BASEDATA> cmmScriptData<BASEDATA>::cmmScriptData(const BASEDATA& i_BaseData)
: m_BaseData(i_BaseData)
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
template<class BASEDATA> cmmScriptData<BASEDATA>::~cmmScriptData()
{
	envSTLHelpers::DeleteContainer(this->m_Drivers);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template<class BASEDATA> bool cmmScriptData<BASEDATA>::operator == (const cmmScriptData<BASEDATA>& i_Item)
{
	return (this->m_BaseData == i_Item.m_BaseData);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template<class BASEDATA> cmmScriptData<BASEDATA>& cmmScriptData<BASEDATA>::operator = (const cmmScriptData<BASEDATA>& i_Data)
{
	if (this == &i_Data) return *this;

	m_BaseData		= i_Data.m_BaseData;

	// drivers
	envSTLHelpers::DeleteContainer(this->m_Drivers);
	int num_drivers = i_Data.m_Drivers.size();
	this->m_Drivers.resize(num_drivers);
	for (int i=0; i<num_drivers; i++)
	{
		this->m_Drivers[i] = i_Data.m_Drivers[i]->Clone();
	}

	this->m_ChannelInfo	= i_Data.m_ChannelInfo;

	return *this;
}


//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
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
};

