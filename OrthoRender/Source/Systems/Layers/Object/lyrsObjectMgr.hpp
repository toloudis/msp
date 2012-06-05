/*****************************************************************************
**	lyrsObjectMgr.hpp
**
**	Manages the 3d representation of the lights in the editor system.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef LYRS_OBJECTMGR_HPP
#error lyrsObjectMgr.hpp multiply included
#endif
#define LYRS_OBJECTMGR_HPP

#ifndef CMM_OBJECTMGRTEMPLATE_HPP
#include "Systems/Common/Templates/cmmSimpleObjectMgrTemplate.hpp"
#endif

#ifndef LYRS_DATA_HPP
#include "Systems/Layers/Data/lyrsData.hpp"
#endif
#ifndef LYRS_LAYEROBJECT_HPP
#include "Systems/Layers/Object/lyrsLayerObject.hpp"
#endif


//============================================================================
//============================================================================
class lyrsObjectCreator 
{
public:
	//--------------------------------------------------------------------
	// Create lyrsLayerObject from lyrsData
	//--------------------------------------------------------------------
	static lyrsLayerObject* Create(const lyrsData &i_Data);
};


//============================================================================
//============================================================================
class lyrsObjectMgr
	: public cmmSimpleObjectMgrTemplate<  class lyrsLayerObject, 
									class lyrsLayersData, 
									class lyrsData,
									class lyrsObjectCreator>
{
public:

};	// end of static class

