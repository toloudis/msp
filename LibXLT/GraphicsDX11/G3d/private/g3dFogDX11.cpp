/****************************************************************************\
**	g3dFogDX11.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "Core/dbg/dbgMsg.hpp"
#include "Core/ma/maFloatRGBA.hpp"
#include "Graphics/g3d/g3dType.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g3d/g3dFogDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"


//	The reason a macro is used here (instead of a function, an inline function, or a template inline function)
//	is that I need the __FILE__ and __LINE__ macros to resolve to useful values
#define CHECK_D3D_ERROR(op_result, error_string) \
			if( op_result != D3D_OK )	\
			{	\
				g2dDX11Global::PrintDXError(op_result);	\
				DBG_ASSERT(false, error_string);	\
			}	\

namespace
{
	bool l_bFogEnabled = true;
	int l_nFogMode = 0;
}


//------------------------------------------------------------------------
//	EnableFog - temporarily turn on/off fog during rendering process
//------------------------------------------------------------------------
void g3dFogDX11::EnableFog( bool i_bEnable )
{
/*
	HRESULT op_result;

	// Enable the fog if it is not already enabled
	if( l_nFogMode != g3dType::e_FogModeNone && !l_bFogEnabled && i_bEnable )
	{
#ifdef ENABLE_LEGACYSTATES
		op_result = g2dDX11Global::g_pDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
		CHECK_D3D_ERROR(op_result, "Couldn't enable/disable fog");
#endif
		l_bFogEnabled = true;
	}
	// Disable the fog if it is not already disabled
	else if( l_bFogEnabled && !i_bEnable )
	{
#ifdef ENABLE_LEGACYSTATES
		op_result = g2dDX11Global::g_pDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
		CHECK_D3D_ERROR(op_result, "Couldn't enable/disable fog");
#endif
		l_bFogEnabled = false;
	}
*/
}

//------------------------------------------------------------------------
//	SetFog sets all fog parameters
//------------------------------------------------------------------------
void g3dFogDX11::SetFog(	int i_nMode,
						const maFloatRGBA& i_Color,
						float i_fStart,
						float i_fEnd,
						float i_fDensity )
{
/*
	HRESULT op_result;

	l_nFogMode = i_nMode; // store this for enabling

	if( i_nMode == g3dType::e_FogModeNone )
	{
#ifdef ENABLE_LEGACYSTATES
		op_result = g2dDX11Global::g_pDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

		// make sure both menthods(pexel and vertex) are disabled.
		g2dDX11Global::g_pDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );
		g2dDX11Global::g_pDevice->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_NONE );
#endif
		l_bFogEnabled = false;
		return;
	}

	// enable fog
#ifdef ENABLE_LEGACYSTATES
	op_result = g2dDX11Global::g_pDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
	CHECK_D3D_ERROR(op_result, "Couldn't enable fog");
#endif
	l_bFogEnabled = true;

	// Get the device caps
//	g2dD3D9CAPS caps;
//	::memset(&caps, 0, sizeof(caps));
//	g2dDX11Global::g_pDevice->GetDeviceCaps(&caps);

	bool use_normalized_value;
	bool can_do_table_fog;
	bool can_do_vertex_fog;

	if (	( g2dDX11Global::g_Caps.RasterCaps & D3DPRASTERCAPS_FOGTABLE )
		 && ( (g2dDX11Global::g_Caps.RasterCaps & D3DPRASTERCAPS_ZFOG) || (g2dDX11Global::g_Caps.RasterCaps & D3DPRASTERCAPS_WFOG) ) )
	{
		can_do_table_fog = true;
	}
	else
	{
		can_do_table_fog = false;
	}

	can_do_vertex_fog = ( g2dDX11Global::g_Caps.RasterCaps & D3DPRASTERCAPS_FOGVERTEX ) ? true : false;

	// normalize fog start and end values for table (pixel) fog mode on devices that
	// do not use WFOG. These devices expect fog between 0.0 and 1.0.
	if( can_do_table_fog && ( (g2dDX11Global::g_Caps.RasterCaps & D3DPRASTERCAPS_WFOG) == 0 ) )
	{
		use_normalized_value = true;
	}
	else
	{
		use_normalized_value = false;
	}

	// pick table fog first
	if ( can_do_table_fog )
	{
#ifdef ENABLE_LEGACYSTATES
		int d3d_fog_mode;
		switch ( i_nMode )
		{
			case g3dType::e_FogModeNone:
				d3d_fog_mode = D3DFOG_NONE;
				break;
			case g3dType::e_FogModeLinear:
				d3d_fog_mode = D3DFOG_LINEAR;
				break;
			case g3dType::e_FogModeExp:
				d3d_fog_mode = D3DFOG_EXP;
				break;
			case g3dType::e_FogModeExp2:
				d3d_fog_mode = D3DFOG_EXP2;
				break;
		}
		g2dDX11Global::g_pDevice->SetRenderState( D3DRS_FOGTABLEMODE, d3d_fog_mode );
		// need to disable vertex mode if table mode is selected
		g2dDX11Global::g_pDevice->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_NONE );
#endif
	}
	else if ( can_do_vertex_fog )
	{
		// only linear mode is supported for vertex fog
		if ( i_nMode == g3dType::e_FogModeLinear )
		{
#ifdef ENABLE_LEGACYSTATES
			g2dDX11Global::g_pDevice->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
			// need to disable table mode if vertex mode is selected
			g2dDX11Global::g_pDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );
#endif
		}
		else
		{
			DBG_WARNING( "Couldn't enable the specified fog. The 3D card only supports linear vertex fog." );
#ifdef ENABLE_LEGACYSTATES
			g2dDX11Global::g_pDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
			g2dDX11Global::g_pDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );
			g2dDX11Global::g_pDevice->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_NONE );
#endif
		}
	}
	else
	{
		DBG_WARNING( "Neither pixel nor vertex fog is supported by the 3D card." );
#ifdef ENABLE_LEGACYSTATES
		g2dDX11Global::g_pDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
		g2dDX11Global::g_pDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );
		g2dDX11Global::g_pDevice->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_NONE );
#endif
	}

	if ( i_nMode == g3dType::e_FogModeLinear )
	{
		float start, end;
		if ( use_normalized_value )
		{
			float near_clip, far_clip;
			near_clip = g3dSceneGlobal::GetProjectionTransform()(3, 2) / g3dSceneGlobal::GetProjectionTransform()(2, 2);
			far_clip = g3dSceneGlobal::GetProjectionTransform()(2, 2) * near_clip / ( g3dSceneGlobal::GetProjectionTransform()(2, 2) - 1.0f );
			start = ( i_fStart - near_clip ) / ( far_clip - near_clip );
			end = ( i_fEnd - near_clip ) / ( far_clip - near_clip );
		}
		else
		{
			start = i_fStart;
			end = i_fEnd;
		}
#ifdef ENABLE_LEGACYSTATES
		op_result = g2dDX11Global::g_pDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&start));
		op_result = g2dDX11Global::g_pDevice->SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&end));
#endif
	}
	else
	{
#ifdef ENABLE_LEGACYSTATES
		op_result = g2dDX11Global::g_pDevice->SetRenderState(D3DRS_FOGDENSITY, *((DWORD*)&i_fDensity));
#endif
	}

	D3DCOLOR fog_color;
	fog_color = D3DCOLOR_COLORVALUE(i_Color.GetRed(), i_Color.GetGreen(), i_Color.GetBlue(), i_Color.GetAlpha());
#ifdef ENABLE_LEGACYSTATES
	g2dDX11Global::g_pDevice->SetRenderState(D3DRS_FOGCOLOR, fog_color);
#endif
	CHECK_D3D_ERROR(op_result, "Couldn't set D3D texture transform");
	DBG_ASSERT(op_result == D3D_OK, "Couldn't set fog color");
*/
}




