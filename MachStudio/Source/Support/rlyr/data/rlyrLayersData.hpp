/********************************************************************************************\
** rlyrLayersData.hpp
**
**		Data relating to the data of each individual render layer
**
**  studio|gpu
\********************************************************************************************/
#pragma once

#ifdef RLYR_LAYERSDATA_HPP
#error rlyrLayersData.hpp multiply included
#endif
#define RLYR_LAYERSDATA_HPP

#ifndef CAPT_RENDEROUTPUTDATA_HPP
#include "Support/capt/captRenderOutputData.hpp"
#endif

#ifndef RPRFPREFSDATA_HPP
#include "Support/rprf/rprfPrefsData.hpp"
#endif

#ifndef RLYR_PASSESDATA_HPP
#include "Support/rlyr/data/rlyrPassesData.hpp"
#endif

#ifndef PFX_DATA_HPP
#include "Support/pfx/pfxData.hpp"
#endif

#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif 
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif
#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif

#include <string>
#include <vector>


//============================================================================
//	Data
//============================================================================

//----------------------------------------------------------------------------
// Fragment Nodes
//----------------------------------------------------------------------------
class rlyrNodeDataItem 
{
public:
	rlyrNodeDataItem();

	prtyInt32			m_Index;
	prtyBoolean			m_IsVisible;
};

//----------------------------------------------------------------------------
//	objects
//----------------------------------------------------------------------------
class rlyrObjectDataItem 
{
public:
	rlyrObjectDataItem();

	prtyText			m_Name;
	prtyBoolean			m_IsVisible;
	std::vector<rlyrNodeDataItem> m_Nodes;
};

//----------------------------------------------------------------------------
//	individual render layer
//----------------------------------------------------------------------------
class rlyrLayerDataItem
{
public:
	rlyrLayerDataItem();

	//properties
	prtyText			m_Name;
	prtyText			m_ParentName;
	prtyBoolean			m_IsActive;

	//data items
	std::vector<rlyrObjectDataItem>		m_Objects;
	rprfPrefsData						m_RenderPrefs;
	captRenderOutputData				m_OutputFormat;
	rlyrPassesData						m_RenderPasses;
	pfxData								m_PostEffect;
};

//----------------------------------------------------------------------------
//	all render layer data
//----------------------------------------------------------------------------
struct rlyrLayersData
{
	std::vector<rlyrLayerDataItem>	m_Layers;
};
