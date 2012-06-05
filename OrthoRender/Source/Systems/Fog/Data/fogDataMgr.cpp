/*****************************************************************************
**	fogDataMgr.cpp
**
**	Keeps track of the current displayed set items
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Systems/Fog/Data/fogDataMgr.hpp"

#include "Core/Dbg/dbgLog.hpp"
#include "Core/Env/envSTLHelpers.hpp"
#include "Tool/api3d/api3dScene.hpp"

#include <vector>

namespace fogDataMgr
{

	namespace
	{

		fogFogData l_Data;
		std::vector<DataChangedCallback*> l_CallbackFuncs;

		//--------------------------------------------------------------------
		void set_fog()
		{
			//DBG_LOG3("Fog: %f %f %f", l_Data.m_Start, l_Data.m_End, l_Data.m_Density );
			//agFog::SetFog( l_Data.m_Color, l_Data.m_Mode, l_Data.m_Start,	l_Data.m_Density, l_Data.m_End );
			api3dScene::SetFog( l_Data.m_Mode.GetValue(), 
								l_Data.m_Color.GetValue(), 
								l_Data.m_Start.GetValue(), 
								l_Data.m_End.GetValue(), 
								l_Data.m_Density.GetValue() );
		}

		//--------------------------------------------------------------------
		void notify_callbacks()
		{
			static bool notifying = false;
			if (notifying) return;

			notifying = true;
			for (int i=0; i<l_CallbackFuncs.size(); i++)
			{
				l_CallbackFuncs[i]->DataChanged();
			}
			notifying = false;
		}

	}	// end of namespace


	//--------------------------------------------------------------------
	// Clear all items
	//--------------------------------------------------------------------
	void  Clear()
	{
		l_Data = fogFogData();
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//  Set mode of fog, none, linear, exp, exp_sq
	//--------------------------------------------------------------------
	void  SetFogMode(fogFogData::FogMode i_Mode)
	{
		l_Data.m_Mode.SetValue(i_Mode);
		set_fog();
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//  Set color of fog
	//--------------------------------------------------------------------
	void  SetFogColor(const maFloatRGBA &i_Color)
	{
		l_Data.m_Color.SetValue(i_Color);
		set_fog();
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//  Set density of fog, usually between 0-0.25
	//--------------------------------------------------------------------
	void  SetFogDensity(float i_Density)
	{
		l_Data.m_Density.SetValue(i_Density);
		set_fog();
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//  Set fog starting distance
	//--------------------------------------------------------------------
	void  SetFogStart(float i_Start)
	{
		l_Data.m_Start.SetValue(i_Start);
		set_fog();
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//  Set fog ending distance
	//--------------------------------------------------------------------
	void  SetFogEnd(float i_End)
	{
		l_Data.m_End.SetValue(i_End);
		set_fog();
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//  Access to whole data as one structure for easy display and
	// parsing
	//--------------------------------------------------------------------
	const fogFogData & GetData()
	{
		return l_Data;
	}

	//--------------------------------------------------------------------
	// Set data as a whole
	//--------------------------------------------------------------------
	void SetData(const fogFogData &i_Data)
	{
		l_Data = i_Data;
		set_fog();
		notify_callbacks();
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
