/*****************************************************************************
**	fogDataMgr.hpp
**
**	Keeps track of the current displayed set items
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#ifdef FOG_DATAMGR_HPP
#error fogDataMgr.hpp multiply included
#endif
#define FOG_DATAMGR_HPP

#ifndef FOG_FOGDATA_HPP
#include "Systems/Fog/Data/fogFogData.hpp"
#endif

class fsLocator;

namespace fogDataMgr
{
	// EventCallback when any data in this manager changes
	class DataChangedCallback
	{
	public:
		virtual void DataChanged() = 0;
	};

	//--------------------------------------------------------------------
	// Clear all items
	//--------------------------------------------------------------------
	void  Clear();

	//--------------------------------------------------------------------
	//  Set mode of fog, none, linear, exp, exp_sq
	//--------------------------------------------------------------------
	void  SetFogMode(fogFogData::FogMode i_Mode);

	//--------------------------------------------------------------------
	//  Set color of fog
	//--------------------------------------------------------------------
	void  SetFogColor(const maFloatRGBA &i_Color);

	//--------------------------------------------------------------------
	//  Set density of fog, usually between 0-0.25
	//--------------------------------------------------------------------
	void  SetFogDensity(float i_Density);

	//--------------------------------------------------------------------
	//  Set fog starting distance
	//--------------------------------------------------------------------
	void  SetFogStart(float i_Start);

	//--------------------------------------------------------------------
	//  Set fog ending distance
	//--------------------------------------------------------------------
	void  SetFogEnd(float i_End);

	//--------------------------------------------------------------------
	//  Access to whole data as one structure for easy display and
	// parsing
	//--------------------------------------------------------------------
	const fogFogData & GetData();
	void SetData(const fogFogData &i_Data);

	//--------------------------------------------------------------------
	// This callback will be called when the data changes
	//--------------------------------------------------------------------
	void AddDataChangedCallback(DataChangedCallback *i_Func);
	void RemoveDataChangedCallback(DataChangedCallback *i_Func);

}	// end of namespace
