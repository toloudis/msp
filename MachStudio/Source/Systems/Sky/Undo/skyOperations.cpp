/*****************************************************************************
**	skyOperations.cpp
**
**	Utility for operations that are undoable in sky system
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#include "skyOperations.hpp"

#include "skyDataMgr.hpp"
#include "skySetOperation.hpp"

#include "undoUndoMgr.hpp"

#include "daySkyMgr.hpp"


namespace skyOperations
{
	namespace
	{
		undoUndoOperation *l_LastOp = NULL;

	}	// end of namespace

	//--------------------------------------------------------------------
	//	Sky operations are joined together if the same type of operation.
	//	Call this to force a new operation
	//--------------------------------------------------------------------
	void StartNewOp()
	{
		l_LastOp = NULL;
	}

	//--------------------------------------------------------------------
	//  Change layer name
	//--------------------------------------------------------------------
	void  ChangeSkyLayerModelName( int i_LayerIndex, const std::string i_ModelName )
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			l_LastOp = new skySetOperation(daySkyMgr::GetListData());
			undoUndoMgr::AddOperation(l_LastOp);
		}
		daySkyMgr::SetModelName( i_LayerIndex, i_ModelName );
	}

	//--------------------------------------------------------------------
	//  Change rotation axis
	//--------------------------------------------------------------------
	void  ChangeRotationAxis( int i_LayerIndex, const maVector3d& i_RotationAxis )
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			l_LastOp = new skySetOperation(daySkyMgr::GetListData());
			undoUndoMgr::AddOperation(l_LastOp);
		}
		skyDataMgr::SetRotationAxis( i_LayerIndex, i_RotationAxis );
	}

	//--------------------------------------------------------------------
	//  Change Rotation Velocity
	//--------------------------------------------------------------------
	void  ChangeRotationVelocity( int i_LayerIndex, float i_fRotateVelicty )
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			l_LastOp = new skySetOperation(daySkyMgr::GetListData());
			undoUndoMgr::AddOperation(l_LastOp);
		}
		skyDataMgr::SetRotationVelocity( i_LayerIndex, i_fRotateVelicty );
	}

	//--------------------------------------------------------------------
	//  Change Layer Color - Day
	//--------------------------------------------------------------------
	void  ChangeLayerColorDay( int i_LayerIndex, const maFloatRGBA& i_Color )
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			l_LastOp = new skySetOperation(daySkyMgr::GetListData());
			undoUndoMgr::AddOperation(l_LastOp);
		}
		skyDataMgr::SetLayerColorDay( i_LayerIndex, i_Color );
	}

	//--------------------------------------------------------------------
	//  Change Layer Color - SunRise/Set
	//--------------------------------------------------------------------
	void  ChangeLayerColorSunRiseSunSet( int i_LayerIndex, const maFloatRGBA& i_Color )
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			l_LastOp = new skySetOperation(daySkyMgr::GetListData());
			undoUndoMgr::AddOperation(l_LastOp);
		}
		skyDataMgr::SetLayerColorSunRiseSunSet( i_LayerIndex, i_Color );
	}

	//--------------------------------------------------------------------
	//  Change Layer Color - Night
	//--------------------------------------------------------------------
	void  ChangeLayerColorNight( int i_LayerIndex, const maFloatRGBA& i_Color )
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			l_LastOp = new skySetOperation(daySkyMgr::GetListData());
			undoUndoMgr::AddOperation(l_LastOp);
		}
		skyDataMgr::SetLayerColorNight( i_LayerIndex, i_Color );
	}

	//--------------------------------------------------------------------
	//  remove a layer
	//--------------------------------------------------------------------
	void  DeleteLayer( int i_LayerIndex )
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			l_LastOp = new skySetOperation(daySkyMgr::GetListData());
			undoUndoMgr::AddOperation(l_LastOp);
		}

		skyDataMgr::DeleteLayer( i_LayerIndex );
	}
}
