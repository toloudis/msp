/****************************************************************************\
**	rpnPanelViewer.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Features/RenderPanels/rpnPanelViewer.hpp"

#include "Features/RenderPanels/rpnRenderingPrefs.hpp"
#include "Features/RenderPrefs/rndrPrefsUtil.hpp"

#include "Support/cams/camsCameraMgr.hpp"
#include "Support/cams/camsDirectorsCut.hpp"
#include "Support/cmps/cmpsCompassMgr.hpp"
#include "Support/mnm/mnmCompassUtil.hpp"

#include "Tool/api3d/api3dScene.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/scr/scrCreator.hpp"
#include "Graphics/scr/scrText.hpp"

namespace
{
	const int c_ScreenSpaceLayer = 2; // FIX - hard coded number
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
rpnPanelViewer::rpnPanelViewer()
:	m_pTextLabel(NULL),
	m_bTextVisible(true),
	m_bAllowShadows(false),
	m_pDirectorsCut(NULL)
{
}

//--------------------------------------------------------------------
// Deletes the renderer and viewer owned by this class.
//--------------------------------------------------------------------
rpnPanelViewer::~rpnPanelViewer()
{
	if (m_pTextLabel)
	{
		api3dScene::GetRoot( c_ScreenSpaceLayer )->RemoveChild( m_pTextLabel->GetSceneNode() );
		delete m_pTextLabel;
	}

}

//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
//virtual 
void rpnPanelViewer::Render( float i_fSimTime )
{
	// Set up the camera if we have a directors cut
	if (m_pDirectorsCut)
	{
		int dcut_index = m_pDirectorsCut->GetCameraIndex();
		if (dcut_index >= 0 && dcut_index < camsCameraMgr::GetNumCameras())
		{
			this->SetCamera( camsCameraMgr::GetCamera(dcut_index) );
		}
	}

	//	update the world compass axis
	mnmCompassUtil::UpdateCompass( cmpsCompassMgr::e_World, *this->GetCamera() );

	// We only want to display the text label during our render
	if (m_pTextLabel)
	{
		// Try to keep the text in the bottom center of the window
		int width = 0, height = 0;
		this->GetWindow()->GetVirtualResolution(width, height);
		if (width > 0 && height > 0)
			m_pTextLabel->SetPosition(maPoint3d( width / 2.0f, (float)(height - 16), 0.0f ));

		m_pTextLabel->SetRenderable( m_bTextVisible );
	}

	// Turn off shadows if needed
	bool bShadowsWereOn = g3dSingleLightRendering::GetDoSingleLightRendering();
	if (!m_bAllowShadows)
	{
		rndrPrefsUtil::SetMultipassRendering( false );
	}

	// Low resolution is turned on/off as a whole, not per individual panel
	// right now. But this might change.
	bool bWasLowRes = g3dPrefs::CurrentPrefs().m_bLowResolution;
	if (rpnRenderingPrefs::GetAllLowResolution())
	{
		// Should use CurrentPrefs or ViewportPrefs?
		// Maybe it should be that this panel viewer has its
		// own render prefs structure and it would set flags
		// on that structure and use it when rendering?
		g3dPrefs::CurrentPrefs().m_bLowResolution = true;
	}

	// Do base class render
	g3dViewer::Render( i_fSimTime );

	// Turn off our text label so it doesn't show up in other views 
	// or in the capture renders
	if (m_pTextLabel)
		m_pTextLabel->SetRenderable(false);
	
	// Restore the shadow state if we changed it
	if (!m_bAllowShadows)
	{
		rndrPrefsUtil::SetMultipassRendering( bShadowsWereOn );
	}
	
	// Restore low-res state
	if (rpnRenderingPrefs::GetAllLowResolution())
	{
		g3dPrefs::CurrentPrefs().m_bLowResolution = bWasLowRes;
	}
}


//------------------------------------------------------------------------
// CreateTextLabel - give font to use 
//------------------------------------------------------------------------
void rpnPanelViewer::CreateTextLabel(const fsLocator &i_FontPath)
{
	DBG_ASSERT0(m_pTextLabel == NULL, "Text label already created.");
			
	/*std::string filename;
	fsFileUtil::LocatorToANSIFilename(i_FontPath, filename);
	DBG_LOG1( "RenderPanel, loading font file %s", filename.c_str() );*/

	m_pTextLabel = scrCreator::MakeText( i_FontPath, this->GetWindow(), 320, 256 );
	m_pTextLabel->SetForegroundColor( maFloatRGBA(1,1,1,0) );
	m_pTextLabel->SetBackgroundColor( maFloatRGBA(0,0,0,1) );
	m_pTextLabel->SetPosition( maPoint3d( 10, 100, 0 ) );
	m_pTextLabel->SetCentered( true );
	m_pTextLabel->SetSize( 5, 8 );
	m_pTextLabel->SetRenderable(false);
	m_pTextLabel->SetText( itString("") );
			
	api3dScene::GetRoot( c_ScreenSpaceLayer )->AddChild( m_pTextLabel->GetSceneNode() );
}

//------------------------------------------------------------------------
// SetTextVisible - turn display of text on/off
//------------------------------------------------------------------------
void rpnPanelViewer::SetTextVisible(bool i_bShow)
{
	m_bTextVisible = i_bShow;
}

//------------------------------------------------------------------------
// SetTextString - set string to display
//------------------------------------------------------------------------
void rpnPanelViewer::SetTextString(const std::string& i_String)
{
	if (m_pTextLabel)
	{
		m_pTextLabel->SetText( itString(i_String.c_str()) );
	}
	m_TextString = i_String;
}
const std::string& rpnPanelViewer::GetTextString() const
{
	return m_TextString;
}

//------------------------------------------------------------------------
// SetAllowShadows - allow shadows to be turned on for this viewer
//------------------------------------------------------------------------
void rpnPanelViewer::SetAllowShadows(bool i_bAllow)
{
	m_bAllowShadows = i_bAllow;
}
bool rpnPanelViewer::GetAllowShadows() const
{
	return m_bAllowShadows;
}

//----------------------------------------------------------------------------
// Set directors cut, this causes the camera to switch based 
//	on the simulation time
//----------------------------------------------------------------------------
void rpnPanelViewer::SetDirectorsCut(camsDirectorsCut *i_pDirectorsCut)
{
	m_pDirectorsCut = i_pDirectorsCut;
}
camsDirectorsCut* rpnPanelViewer::GetDirectorsCut()
{
	return m_pDirectorsCut;
}

//----------------------------------------------------------------------------
// Returns true if the panel has a directors cut
//----------------------------------------------------------------------------
bool rpnPanelViewer::HasDirectorsCut() const
{
	return (m_pDirectorsCut != NULL);
}
