/********************************************************************************************\
**  prtclScriptData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#ifdef PRTCL_SCRIPTDATA_HPP
#error prtclScriptData.hpp multiply included
#endif
#define PRTCL_SCRIPTDATA_HPP

#ifndef PRTCL_DATA_HPP
#include "Systems/Particles/Data/prtclData.hpp"
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


//============================================================================
//============================================================================
class prtclScriptData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	prtclScriptData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	prtclScriptData(const prtclScriptData& i_Data );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	explicit prtclScriptData(const prtclData& i_BaseData);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	prtclScriptData(const itString& i_Filename );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	prtclScriptData(const itString& i_Filename,
					const maPoint3d& i_Position,
					const maRotation& i_Orientation );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~prtclScriptData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool operator == (const prtclScriptData& i_Item);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	prtclScriptData& operator=(const prtclScriptData& i_Data);

	//
	//	Data
	//
	prtclData	m_BaseData;

	std::vector<tmlnDriverInfo*>	m_Drivers;
	std::map<std::string, tmlnChannelInfo> m_ChannelInfo;
};


//============================================================================
//============================================================================
class prtclParticlesData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Add(const itString& i_Filename,
			 const maPoint3d& i_Position,
			 const maRotation& i_Orientation );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Clear();

	//------------------------------------------------------------------------
	//	data
	//------------------------------------------------------------------------
	std::vector<prtclScriptData> m_Items;
};

