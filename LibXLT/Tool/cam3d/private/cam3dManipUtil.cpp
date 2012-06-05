/*****************************************************************************
**	cam3dManipUtil.hpp
**
**		Based on the user's current preferences for mouse configuration, 
**	cam3dManipUtil, will tell the camera which actions to perform. 
**
**	StudioGPU
**	Copyright(C) 2009
\****************************************************************************/
#include "Tool/cam3d/cam3dManipUtil.hpp"

#include "Input/in/inDeviceMgr.hpp"


//============================================================================
//============================================================================
namespace cam3dManipUtil
{
	//------------------------------------------------------------------------
	//  Function ptr
	//------------------------------------------------------------------------
	void (*m_ControlFunctionPtr)( inMouse* i_pMouse, inKeyboard* i_pKeyboard, CameraControl& o_CamManip );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetCameraControlMode( int i_CameraControlID )
	{
		switch( i_CameraControlID )
		{
		case e_MSPRO:
			m_ControlFunctionPtr = &DoMSProControls;
			break;
		case e_MAYA:
			m_ControlFunctionPtr = &DoMayaControls;
			break;
		case e_MAX:
			m_ControlFunctionPtr = &DoMaxControls;
			break;
		default:
			m_ControlFunctionPtr = NULL;
			break;
		};
	}
	
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------	
	void GetCameraControls( inMouse* i_pMouse, inKeyboard* i_pKeyboard, CameraControl& o_CamManip )
	{
		//if for some reason the function ptr is null, do the MSPro mouse control by default
		if(!m_ControlFunctionPtr)
			DoMSProControls(i_pMouse, i_pKeyboard, o_CamManip);
		else		
			m_ControlFunctionPtr(i_pMouse, i_pKeyboard, o_CamManip);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------	
	void DoMSProControls( inMouse* i_pMouse, inKeyboard* i_pKeyboard, CameraControl& o_CamManip )
	{
		o_CamManip.m_bValidControls = false;
		o_CamManip.m_bScrollWheel = false;
		o_CamManip.m_bXYZoom = false;
		o_CamManip.m_bInverseZoom = false;
		o_CamManip.m_CamManipMode = e_NONE;

		if ( i_pMouse && i_pKeyboard )
		{
			//validate the controls
			o_CamManip.m_bValidControls = true;

			//check first for alt keys
			if (   i_pKeyboard->IsDown(inKeys::e_LALT) 
				|| i_pKeyboard->IsDown(inKeys::e_RALT))
			{
				if (   ( i_pMouse->IsHeld( 1 ) )
					&& ( i_pMouse->IsHeld( 0 ) ) )
					o_CamManip.m_CamManipMode = e_ZOOM;
			
				else if( i_pMouse->IsHeld( 0 ) )
					o_CamManip.m_CamManipMode = e_ROTATE;

				else if( i_pMouse->IsHeld( 1 ) )
					o_CamManip.m_CamManipMode = e_PAN;
			}
			//then check for ctrl keys
			else if (   i_pKeyboard->IsDown(inKeys::e_LCTRL) 
					 || i_pKeyboard->IsDown(inKeys::e_RCTRL))
			{
				if( i_pMouse->IsHeld( 0) )
					o_CamManip.m_CamManipMode = e_PIVOT;
			}
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------	
	void DoMayaControls( inMouse* i_pMouse, inKeyboard* i_pKeyboard, CameraControl& o_CamManip )
	{
		o_CamManip.m_bValidControls = false;
		o_CamManip.m_bScrollWheel = false;
		o_CamManip.m_bXYZoom = true;
		o_CamManip.m_bInverseZoom = true;
		o_CamManip.m_CamManipMode = e_NONE;

		if ( i_pMouse && i_pKeyboard )
		{
			//validate the controls
			o_CamManip.m_bValidControls = true;

			//check first for alt keys
			if (   i_pKeyboard->IsDown(inKeys::e_LALT) 
				|| i_pKeyboard->IsDown(inKeys::e_RALT))
			{
				if (  i_pMouse->IsHeld( 1 ) )
					o_CamManip.m_CamManipMode = e_ZOOM;
			
				else if( i_pMouse->IsHeld( 0 ) )
					o_CamManip.m_CamManipMode = e_ROTATE;

				else if( i_pMouse->IsHeld( 2 ) )
					o_CamManip.m_CamManipMode = e_PAN;
				else
				{
					int dX, dY, dZ;
					i_pMouse->GetAnalogDir(0, dX, dY, dZ);
					if(dZ != 0)
					{
						o_CamManip.m_bScrollWheel = true;
						o_CamManip.m_CamManipMode = e_ZOOM;
					}
				}
			}
			//then check for ctrl keys
			else if (   i_pKeyboard->IsDown(inKeys::e_LCTRL) 
					 || i_pKeyboard->IsDown(inKeys::e_RCTRL))
			{
				if( i_pMouse->IsHeld( 0) )
					o_CamManip.m_CamManipMode = e_PIVOT;
			}
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------	
	void DoMaxControls( inMouse* i_pMouse, inKeyboard* i_pKeyboard, CameraControl& o_CamManip )
	{
		o_CamManip.m_bValidControls = false;
		o_CamManip.m_bScrollWheel = false;
		o_CamManip.m_bXYZoom = false;
		o_CamManip.m_bInverseZoom = false;
		o_CamManip.m_CamManipMode = e_NONE;
		
		if ( i_pMouse && i_pKeyboard )
		{
			//validate the controls
			o_CamManip.m_bValidControls = true;

			//check first for alt keys
			if (   i_pKeyboard->IsDown(inKeys::e_LALT) 
				|| i_pKeyboard->IsDown(inKeys::e_RALT))
			{
				if (  i_pMouse->IsHeld( 2 ) )
					o_CamManip.m_CamManipMode = e_ROTATE;
			}
			//check for middle mouse only
			else if( i_pMouse->IsHeld( 2 ) )
				o_CamManip.m_CamManipMode = e_PAN;

			//then check for ctrl keys
			else if (   i_pKeyboard->IsDown(inKeys::e_LCTRL) 
					 || i_pKeyboard->IsDown(inKeys::e_RCTRL))
			{
				if( i_pMouse->IsHeld( 0) )
					o_CamManip.m_CamManipMode = e_PIVOT;
			}
			//check for middle scroll only
			else 
			{
				int dX, dY, dZ;
				i_pMouse->GetAnalogDir(0, dX, dY, dZ);
				if(dZ != 0)
				{
					o_CamManip.m_bScrollWheel = true;
					o_CamManip.m_CamManipMode = e_ZOOM;
				}
			}
		}
	}


} // end namespace cam3dManipUtil