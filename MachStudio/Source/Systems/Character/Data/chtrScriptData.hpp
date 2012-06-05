/********************************************************************************************\
**  chtrScriptData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#ifdef CHTR_SCRIPTDATA_HPP
#error chtrScriptData.hpp multiply included
#endif
#define CHTR_SCRIPTDATA_HPP

#ifndef CHTR_DATA_HPP
#include "Systems/Character/Data/chtrData.hpp"
#endif
#ifndef CHTR_EXPRESSIONDATA_HPP
#include "Systems/Character/Data/chtrExpressionData.hpp"
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
#ifndef TMLN_CHANNELINFO_HPP
#include "Support/tmln/tmlnChannelInfo.hpp"
#endif
#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef LTST_LIGHTSETSDATA_HPP
#include "Support/ltst/ltstLightSetsData.hpp"
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
struct chtrConnectionData
{
	nameString m_EnvironmentName;
	std::vector<ltstLightSetObjectData> m_LightSets;
	nameString m_LayerName;
	std::vector<nameString> m_GroupNames;
	std::vector<nameString> m_RLayerNames;
};

//============================================================================
//============================================================================
class chtrScriptData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	chtrScriptData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	chtrScriptData(const chtrScriptData& i_Data );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	explicit chtrScriptData(const chtrData& i_BaseData );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	chtrScriptData(const itString& i_Filename );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	chtrScriptData(	const itString& i_Filename,
					const maPoint3d& i_Position,
					const maRotation& i_Orientation );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~chtrScriptData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool operator == (const chtrScriptData& i_Item);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	chtrScriptData& operator=(const chtrScriptData& i_Data);

	//------------------------------------------------------------------------
	//	data
	//------------------------------------------------------------------------
	chtrData	m_BaseData;
	chtrConnectionData m_ConnectionData;

	std::vector<tmlnDriverInfo*>	m_Drivers;
	std::vector<dynControlData>		m_Controls;
	std::map<std::string, tmlnChannelInfo> m_ChannelInfo;
	std::vector< shared_ptr<xtraPropertyData> > m_CustomProperties;
	std::vector< shared_ptr<mdlMaterialInfo> > m_Materials;
	chtrExpressionsData				m_Expressions;
	std::vector<fgmtFragmentData>	m_Fragments;
	fgmtFragmentsAOData				m_AOData;
};


//============================================================================
//============================================================================
class chtrCharactersData
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
	std::vector<chtrScriptData> m_Items;
};

