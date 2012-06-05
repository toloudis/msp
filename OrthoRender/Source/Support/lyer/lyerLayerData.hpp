/********************************************************************************************\
**  lyerLayerData.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/

#ifdef LYER_LAYERDATA_HPP
#error lyerLayerData.hpp multiply included
#endif
#define LYER_LAYERDATA_HPP

//#ifndef LYER_LAYERSTYLE_HPP
//#include "Support/lyer/lyerLayerStyle.hpp"
//#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif

#include <vector>

class lyerLayerData
{
public:
	lyerLayerData();

	nameString m_Name;
	std::vector<nameString> m_Objects;

	//lyerLayerStyle m_Style;
	bool m_bVisible;
	bool m_bPickable;
	bool m_bWireframe;
	bool m_bLowRes;
};

class lyerLayersData
{
public:
	std::vector<lyerLayerData> m_Layers;

	//----------------------------------------------------------------------------
	// Add data as new layer, or merge data into 
	// existing layer with the same name.
	//----------------------------------------------------------------------------
	void AddLayerData(const lyerLayerData& i_Data);
};

