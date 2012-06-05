/*****************************************************************************
**	swlScriptObject.hpp
**
**	This class handles altering the software lighting flags within a script object
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef SWL_SCRIPTOBJECT_HPP
#error swlScriptObject.hpp multiply included
#endif
#define SWL_SCRIPTOBJECT_HPP

#ifndef SWL_DATA_HPP
#include "Support/swl/swlData.hpp"
#endif

#include <list>
#include <string>
#include <vector>

//============================================================================
//============================================================================
class swlData;
class swlInterest;
class swlPropertyObject;

//============================================================================
//============================================================================
class swlScriptObject
{
public:
	//--------------------------------------------------------------------
	// Constructor
	//--------------------------------------------------------------------
	swlScriptObject();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~swlScriptObject();

	//--------------------------------------------------------------------
	//	Get software lighting data
	//--------------------------------------------------------------------
	const swlData& GetSwlData() const;

	//--------------------------------------------------------------------
	//	Set software lighting data
	//--------------------------------------------------------------------
	void SetSwlData(const swlData& i_Data);

	//--------------------------------------------------------------------
	//	Return software lighting ui properties
	//--------------------------------------------------------------------
	swlPropertyObject* GetPropertyUI() const;

	//--------------------------------------------------------------------
	//	Register interest
	//--------------------------------------------------------------------
	void RegisterInterest(swlInterest* i_Interest);

private:
	swlData m_Data;
	swlPropertyObject* m_PropertyUI;

};
