/*****************************************************************************
**	skyOperations.hpp
**
**	Utility for operations that are undoable in sky system
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#ifdef SKY_OPERATIONS_HPP
#error skyOperations.hpp multiply included
#endif
#define SKY_OPERATIONS_HPP


#ifndef DAY_SKYDATA_HPP
#include "daySkyData.hpp"
#endif


namespace skyOperations
{
	//--------------------------------------------------------------------
	//	Sky operations are joined together if the same type of operation.
	//	Call this to force a new operation
	//--------------------------------------------------------------------
	void StartNewOp();

	//--------------------------------------------------------------------
	//  Change layer name
	//--------------------------------------------------------------------
	void  ChangeSkyLayerModelName( int i_LayerIndex, const std::string& i_ModelName );

	//--------------------------------------------------------------------
	//  Change rotation axis
	//--------------------------------------------------------------------
	void  ChangeRotationAxis( int i_LayerIndex, const maVector3d& i_RotationAxis );

	//--------------------------------------------------------------------
	//  Change Rotation Velocity
	//--------------------------------------------------------------------
	void  ChangeRotationVelocity( int i_LayerIndex, float i_fRotateVelicty );

	//--------------------------------------------------------------------
	//  Change Layer Color - Day
	//--------------------------------------------------------------------
	void  ChangeLayerColorDay( int i_LayerIndex, const maFloatRGBA& i_Color );

	//--------------------------------------------------------------------
	//  Change Layer Color - SunRise/Set
	//--------------------------------------------------------------------
	void  ChangeLayerColorSunRiseSunSet( int i_LayerIndex, const maFloatRGBA& i_Color );

	//--------------------------------------------------------------------
	//  Change Layer Color - Night
	//--------------------------------------------------------------------
	void  ChangeLayerColorNight( int i_LayerIndex, const maFloatRGBA& i_Color );

	//--------------------------------------------------------------------
	//  remove a layer
	//--------------------------------------------------------------------
	void  DeleteLayer( int i_LayerIndex );
}
