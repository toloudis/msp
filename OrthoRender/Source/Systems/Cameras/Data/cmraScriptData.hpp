/********************************************************************************************\
**  cmraScriptData.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#ifdef CMRA_SCRIPTDATA_HPP
#error cmraScriptData.hpp multiply included
#endif
#define CMRA_SCRIPTDATA_HPP

#ifndef CMRA_DATA_HPP
#include "Systems/Cameras/Data/cmraData.hpp"
#endif

#ifndef AN_KEYDATA_HPP
#include "Graphics/an/anKeyData.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_POINT3D_HPP
#include "Core/prty/prtyPoint3d.hpp"
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
class cmraScriptData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cmraScriptData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	explicit cmraScriptData(const cmraCameraData& i_Data);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cmraScriptData(const cmraScriptData& i_Data);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~cmraScriptData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool operator == (const cmraScriptData& i_Item);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cmraScriptData& operator=(const cmraScriptData& i_Data);

public:
	//------------------------------------------------------------------------
	//	data
	//------------------------------------------------------------------------
	cmraCameraData	m_BaseData;

	std::vector<tmlnDriverInfo*>	m_Drivers;
	std::map<std::string, tmlnChannelInfo> m_ChannelInfo;
};


//============================================================================
//============================================================================
//class cmraCueFormData
//{
//public:
//	cmraCueFormData();
//
//	// Names of cameras
//	std::vector<nameString> m_CameraNames;
//
//	// Camera index displayed on viewers in form
//	int m_Index[4];
//	anKeyData<int> m_Cues;
//};


//============================================================================
//============================================================================
class cmraCamerasData
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	cmraCamerasData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Clear();

	std::vector<cmraScriptData> m_Items;

	// Initial camera info - changed to contain full camera data structure
	// so that we can alter and store the HDR information for the editor camera.
	cmraCameraData m_EditorCamera;
	//prtyPoint3d m_Target;
	//prtyFloat	m_Pitch;
	//prtyFloat	m_Yaw;
	//prtyFloat	m_Radius;

	// Camera cue info
	//cmraCueFormData m_CueForm;
};

