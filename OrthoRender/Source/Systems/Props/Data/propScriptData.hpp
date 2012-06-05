/********************************************************************************************\
**  propScriptData.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#ifdef PROP_SCRIPTDATA_HPP
#error propScriptData.hpp multiply included
#endif
#define PROP_SCRIPTDATA_HPP

#ifndef PROP_DATA_HPP
#include "Systems/Props/Data/propData.hpp"
#endif

#ifndef DYN_CONTROLDATA_HPP
#include "Support/dyn/dynControlData.hpp"
#endif
#ifndef FGMT_FRAGMENTDATA_HPP
#include "Support/fgmt/fgmtFragmentData.hpp"
#endif
#ifndef FGMT_FRAGMENTSAODATA_HPP
#include "Support/fgmt/fgmtFragmentsAOData.hpp"
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
#ifndef MDL_MATERIALINFO_HPP
#include "Graphics/mdl/mdlMaterialInfo.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
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
class propScriptData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	propScriptData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	propScriptData(const propScriptData& i_Data );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	explicit propScriptData(const propData& i_BaseData );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	propScriptData(const itString& i_Filename );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	propScriptData(	const itString& i_Filename,
					const maPoint3d& i_Position,
					const maRotation& i_Orientation );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~propScriptData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool operator == (const propScriptData& i_Item);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	propScriptData& operator=(const propScriptData& i_Data);

	//------------------------------------------------------------------------
	//	data
	//------------------------------------------------------------------------
	propData		m_BaseData;

	std::vector<tmlnDriverInfo*>	m_Drivers;
	std::vector<dynControlData>		m_Controls;
	std::vector< shared_ptr<mdlMaterialInfo> > m_Materials;
	std::map<std::string, tmlnChannelInfo> m_ChannelInfo;
	std::vector<fgmtFragmentData>	m_Fragments;
	fgmtFragmentsAOData m_AOData;
	
};


//============================================================================
//============================================================================
class propPropsData
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
	std::vector<propScriptData> m_Items;
};

