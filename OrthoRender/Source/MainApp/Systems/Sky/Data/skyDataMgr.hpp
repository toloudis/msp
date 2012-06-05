/*****************************************************************************
**	skyDataMgr.hpp
**
**	Keeps track of the current displayed set items
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#ifdef SKY_DATAMGR_HPP
#error skyDataMgr.hpp multiply included
#endif
#define SKY_DATAMGR_HPP

#ifndef DAY_SKYDATA_HPP
#include "daySkyData.hpp"
#endif

class fsLocator;

namespace skyDataMgr
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
	//  Set layer name
	//--------------------------------------------------------------------
	void  SetSkyLayerModelName( int i_LayerIndex, const std::string& i_ModelName );

	//--------------------------------------------------------------------
	//  Set rotation axis
	//--------------------------------------------------------------------
	void  SetRotationAxis( int i_LayerIndex, const maVector3d& i_RotationAxis );

	//--------------------------------------------------------------------
	//  Set Rotation Velocity
	//--------------------------------------------------------------------
	void  SetRotationVelocity( int i_LayerIndex, float i_fRotateVelocity );

	//--------------------------------------------------------------------
	//  Set Layer Color - Day
	//--------------------------------------------------------------------
	void  SetLayerColorDay( int i_LayerIndex, const maFloatRGBA& i_Color );

	//--------------------------------------------------------------------
	//  Set Layer Color - SunRise/Set
	//--------------------------------------------------------------------
	void  SetLayerColorSunRiseSunSet( int i_LayerIndex, const maFloatRGBA& i_Color );

	//--------------------------------------------------------------------
	//  Set Layer Color - Night
	//--------------------------------------------------------------------
	void  SetLayerColorNight( int i_LayerIndex, const maFloatRGBA& i_Color );

	//--------------------------------------------------------------------
	//	delete a layer
	//--------------------------------------------------------------------
	void DeleteLayer( int i_LayerIndex );

	//--------------------------------------------------------------------
	//  Access to whole data as one structure for easy display and
	// parsing
	//--------------------------------------------------------------------
	const daySkyLayerList & GetListData();
	void SetListData(const daySkyLayerList &i_Data);

	//--------------------------------------------------------------------
	//	Access on data member
	//--------------------------------------------------------------------
	const daySkyLayerData & GetData( int i_Index );
	void SetData( int i_Index, const daySkyLayerData &i_Data );

	//--------------------------------------------------------------------
	// This callback will be called when the data changes
	//--------------------------------------------------------------------
	void AddDataChangedCallback(DataChangedCallback *i_Func);
	void RemoveDataChangedCallback(DataChangedCallback *i_Func);

}	// end of namespace
