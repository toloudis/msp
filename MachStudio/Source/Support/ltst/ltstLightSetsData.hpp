/********************************************************************************************\
**  ltstLightSetsData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#ifdef LTST_LIGHTSETSDATA_HPP
#error ltstLightSetsData.hpp multiply included
#endif
#define LTST_LIGHTSETSDATA_HPP

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif

#include <vector>

class ltstLightSetObjectData
{
public:
	nameString m_Name;
	std::vector<int> m_LitFragmentIndices;
};

class ltstLightSetData
{
public:
	nameString m_Name;
	std::vector<nameString> m_Lights;
	std::vector<ltstLightSetObjectData> m_Objects;
};

class ltstLightSetsData
{
public:
	std::vector<ltstLightSetData> m_LightSets;

	// Add data as new light set, or merge data into 
	// existing light set.
	void AddLightSetData(const ltstLightSetData& i_Data);
};

