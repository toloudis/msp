/********************************************************************************************\
**  rlyrLayersData.cpp
**
**		See .hpp for details
**
**  studio|gpu
\********************************************************************************************/
#include "Support/rlyr/data/rlyrLayersData.hpp"

//Nodes
rlyrNodeDataItem::rlyrNodeDataItem()
	: m_Index("Index of Node"),
	  m_IsVisible("Node is visible in this layer")
{}

//Objects
rlyrObjectDataItem::rlyrObjectDataItem()
	: m_Name(""),
	  m_IsVisible("Object is visible in this layer")
{}

//Render Layers
rlyrLayerDataItem::rlyrLayerDataItem()
	: m_Name(""),
	  m_ParentName(""),
	  m_IsActive("Layer is active")
{}