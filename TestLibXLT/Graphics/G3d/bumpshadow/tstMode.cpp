/*****************************************************************************
**  tstMode.cpp
**
**      See tstMode.hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "tstMode.hpp"

#include "tstLightMgr.hpp"

#include "appModeMgr.hpp"
#include "appSimTime.hpp"
#include "camCameraMgr.hpp"
#include "dbgLog.hpp"
#include "g2dFontUtil.hpp"
#include "g2dScreenDrawUtil.hpp"
#include "g2dScreen.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dScene.hpp"
#include "inDeviceMgr.hpp"
#include "scBillboard.hpp"
#include "scCamera.hpp"

//====================================================================
//====================================================================
tstMode::tstMode(int i_nNumDebugStrings,
				bool i_bCreateLights, 
				const g2dRGBColor& i_TextColor/*=g2dRGBColor(235,20,80)*/,
				const g2dRGBColor& i_BackColor/*=g2dRGBColor(0,0,0)*/ )
:	m_DebugStrings(i_nNumDebugStrings),
	m_bCreateLights(i_bCreateLights),
	m_Font( g2dFontUtil::LoadFont(itString("Arial"), 14) ),
	m_TextColor( i_TextColor ),
	m_BackColor( i_BackColor )
{
}

//====================================================================
//====================================================================
// virtual
tstMode::~tstMode()
{
	g2dFontUtil::ReleaseFont(m_Font);
}

//====================================================================
//	Initialize will be called before the first call of Think after
//	the object is first created or DeInitialized.  During the
//	lifetime of a mode, Initialize and DeInitialize may be called
//	several times.  Children of extModeWidthTest should remember to call
//	extModeWidthTest::Initialize() at the beginning of their Initialize
//	function.
//====================================================================
void tstMode::Initialize()
{
	appMode::Initialize();

	tstLightMgr::Initialize();

	if (m_bCreateLights)
	{
		//	set lights
		m_Light1 = tstLightMgr::CreateDirectionalLight();
		m_Light2 = tstLightMgr::CreateDirectionalLight();

		m_Light1->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
		m_Light2->SetDirection(maVector3d(-0.5f, -0.8f, -0.0f));

		m_Light1->SetIntensity(maFloatRGBA(0.4f, 0.7f, 0.2f, 1.0f));
		m_Light2->SetIntensity(maFloatRGBA(0.3f, 0.6f, 0.8f, 1.0f));
	}
}

//====================================================================
//	The object should clean up things that are not needed while the
//	mode is not running in the DeInitialize function.
//====================================================================
void tstMode::DeInitialize()
{
	appMode::DeInitialize();

	if (m_bCreateLights)
	{
		tstLightMgr::DestroyLight(m_Light1);
		tstLightMgr::DestroyLight(m_Light2);
	}

	tstLightMgr::DeInitialize();
}

//====================================================================
//	The mode should do it's per frame "work" in the Think function.
//====================================================================
void tstMode::Think()
{
	// base class think first
	appMode::Think();

	// input think
	inDeviceMgr::Think();

	// Get the keyboard pointer.
	inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();

	// Space bar is used to exit modes, by default
	if (pKeyboard->IsReleased(inKeys::e_SPACE))
	{
		SetTerminateCondition(e_TerminateAndRemove);
		return;
	}

	// ESC is used to exit the program immediately, by default
	if ( pKeyboard->IsReleased( inKeys::e_ESC ) ) 
	{
		appModeMgr::Clear();
		SetTerminateCondition(e_TerminateAndRemove);
		return;
	}

	// Calls Set
	camCameraMgr::Think();

	g2dScreenDrawUtil::Clear( m_BackColor );
}

//====================================================================
// Between thinks, debug strings can be added, they are set to
// the display vector until Render is called, at which time they 
// are displayed.
//====================================================================
void tstMode::SetDebugString( int i_nIndex, const itString& i_String )
{

	DBG_ASSERT0(i_nIndex < m_DebugStrings.size(), "Invalid debug string index, allocate more!" );

	m_DebugStrings[i_nIndex] = i_String;
}

//====================================================================
//	Render is a convenience for child classes which does the usual
//	rendering stuff like the scScene cull, g3dRenderer::RenderModels,
//	and so on.  i_Camera can be NULL, in which case no culling is
//	done (for menus, for instance).
//====================================================================
void tstMode::Render()
{
	display_debug_text();

	g3dScene::Render(appSimTime::GetTime());

	//	flip screen from last frame
	g2dScreen::EndScene();
}

//====================================================================
// display_debug_text will draw all of the debug strings on each render
//====================================================================
void tstMode::display_debug_text()
{
	int num_lines = m_DebugStrings.size();

	int i;
	for( i = 0 ; i < num_lines; i++ )
	{
		g2dScreenDrawUtil::DrawText(	10,
										(i * 17) + 10,
										m_Font,
										m_DebugStrings[i],
										m_TextColor);

		// text_color.SetRed(text_color.GetRed() + 10);
	}
}
