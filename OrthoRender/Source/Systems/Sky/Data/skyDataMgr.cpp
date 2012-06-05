/*****************************************************************************
**	skyDataMgr.cpp
**
**	Keeps track of the current displayed set items
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#include "skyDataMgr.hpp"

#include "api3dScene.hpp"
#include "daySkyMgr.hpp"
#include "dbgLog.hpp"
#include "envSTLHelpers.hpp"

#include <vector>


namespace skyDataMgr
{
	namespace
	{
		std::vector<DataChangedCallback*> l_CallbackFuncs;

		//--------------------------------------------------------------------
		void set_sky()
		{
			//DBG_LOG3("Sky: %f %f %f", l_Data.m_Start, l_Data.m_End, l_Data.m_Density );
			//agSky::SetSky( l_Data.m_Color, l_Data.m_Mode, l_Data.m_Start,	l_Data.m_Density, l_Data.m_End );
// FIX:			api3dScene::SetSky( l_Data.m_Mode, l_Data.m_Color, l_Data.m_Start, l_Data.m_End, l_Data.m_Density );
		}

		//--------------------------------------------------------------------
		void notify_callbacks()
		{
			static bool notifying = false;
			if (notifying) return;

			notifying = true;
			for (int i=0; i<l_CallbackFuncs.size(); i++)
				l_CallbackFuncs[i]->DataChanged();
			notifying = false;
		}

	}	// end of namespace


	//--------------------------------------------------------------------
	// Clear all items
	//--------------------------------------------------------------------
	void  Clear()
	{
		daySkyMgr::RemoveAll();
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//  Set layer name
	//--------------------------------------------------------------------
	void  SetSkyLayerModelName( int i_LayerIndex, const std::string& i_ModelName )
	{
		daySkyMgr::SetModelName( i_LayerIndex, i_ModelName );
		set_sky();
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//  Set rotation axis
	//--------------------------------------------------------------------
	void  SetRotationAxis( int i_LayerIndex, const maVector3d& i_RotationAxis )
	{
		daySkyMgr::SetRotationAxis( i_LayerIndex, i_RotationAxis );
		set_sky();
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//  Set Rotation Velocity
	//--------------------------------------------------------------------
	void  SetRotationVelocity( int i_LayerIndex, float i_fRotateVelocity )
	{
		daySkyMgr::SetRotationVelocity( i_LayerIndex, i_fRotateVelocity );
		set_sky();
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//  Set Layer Color - Day
	//--------------------------------------------------------------------
	void  SetLayerColorDay( int i_LayerIndex, const maFloatRGBA& i_Color )
	{
		daySkyMgr::SetDayColor( i_LayerIndex, i_Color );
		set_sky();
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//  Set Layer Color - SunRise/Set
	//--------------------------------------------------------------------
	void  SetLayerColorSunRiseSunSet( int i_LayerIndex, const maFloatRGBA& i_Color )
	{
		daySkyMgr::SetSunRiseSetColor( i_LayerIndex, i_Color );
		set_sky();
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//  Set Layer Color - Night
	//--------------------------------------------------------------------
	void  SetLayerColorNight( int i_LayerIndex, const maFloatRGBA& i_Color )
	{
		daySkyMgr::SetNightColor( i_LayerIndex, i_Color );
		set_sky();
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//  Access to whole data as one structure for easy display and
	// parsing
	//--------------------------------------------------------------------
	const daySkyLayerList & GetListData()
	{
		return daySkyMgr::GetListData();
	}

	//--------------------------------------------------------------------
	// Set data as a whole
	//--------------------------------------------------------------------
	void SetListData(const daySkyLayerList &i_Data)
	{
		daySkyMgr::SetListData( i_Data );
		set_sky();
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//	delete a layer
	//--------------------------------------------------------------------
	void DeleteLayer( int i_LayerIndex )
	{
		daySkyMgr::Remove( i_LayerIndex );
		//set_sky();
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//	Access on data member
	//--------------------------------------------------------------------
	const daySkyLayerData & GetData( int i_Index )
	{
		return daySkyMgr::GetData( i_Index );
	}

	void SetData( int i_Index, const daySkyLayerData &i_Data )
	{
		daySkyMgr::SetData( i_Index, i_Data );
	}

	//--------------------------------------------------------------------
	// This callback will be called when the data changes
	//--------------------------------------------------------------------
	void AddDataChangedCallback(DataChangedCallback *i_Func)
	{
		l_CallbackFuncs.push_back(i_Func);

	}
	void RemoveDataChangedCallback(DataChangedCallback *i_Func)
	{
		envSTLHelpers::RemoveOneValue(l_CallbackFuncs, i_Func);
	}
}	// end of namespace

