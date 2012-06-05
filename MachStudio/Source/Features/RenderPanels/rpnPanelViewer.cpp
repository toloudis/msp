/****************************************************************************\
**	rpnPanelViewer.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Features/RenderPanels/rpnPanelViewer.hpp"

#include "Features/Prefs/PrefsMgr.hpp"
#include "Features/RenderPanels/rpnRenderingPrefs.hpp"
#include "Features/RenderPrefs/rndrPrefsUtil.hpp"
#include "MainApp/mnmApp.hpp"
#include "Support/cams/camsCameraMgr.hpp"
#include "Support/cams/camsDirectorsCut.hpp"
#include "Support/cmps/cmpsCompassMgr.hpp"
#include "Support/cmps/cmpsWorldAxis.hpp"
#include "Support/mnm/mnmCompassUtil.hpp"
#include "Support/pfx/pfxPostEffectMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/scr/scrCreator.hpp"
#include "Graphics/scr/scrText.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/gpx/gpxText.hpp"
#include "Tool/icn/icnIconLayer.hpp"
#include "Tool/icn/icnIconScale.hpp"


//============================================================================
//============================================================================
namespace
{
	enum LayerEnum
	{
		e_CameraLayer = 0,
		e_ScreenLayer,
		e_IconLayer,
		e_ManipulatorLayer
	};

	const float c_fScreenCompassRadian	= 0.02f;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
rpnPanelViewer::rpnPanelViewer()
:	m_pTextLabel(NULL),
	m_pTextLabelProxy(NULL),
	//m_bTextVisible(true),
	m_bAllowShadows(false),
	m_pDirectorsCut(NULL)
{
	// Panel viewers have one screen space layer and two icon layers that are specific to 
	// this viewer (so that they can resize and set visibility separately)
	//	
	// Camera space layer specific to this viewer
	m_LayerRoots.push_back( new g3dSceneNode() );
	this->AppendLayer(new g3dLayer( m_LayerRoots[e_CameraLayer] , g3dLayer::e_ZBuffer,
									g3dLayer::e_Camera ) );
	// Screen space layer specific to this viewer
	m_LayerRoots.push_back( new g3dSceneNode() );
	this->AppendLayer( new g3dLayer( m_LayerRoots[e_ScreenLayer],
						  g3dLayer::e_ZBuffer,g3dLayer::e_Screen, g3dLayer::e_Additive,
						  false, false, true, false) );
	//	World space layer for icons only:
	m_LayerRoots.push_back( new g3dSceneNode() );
	this->AppendLayer( new g3dLayer( m_LayerRoots[e_IconLayer],
					g3dLayer::e_ZBuffer, g3dLayer::e_World, g3dLayer::e_Multiplicative,
					false, false, false, false) );
	//	Create a world space layer for compass manipulators only:
	// (note that this layer clears the z buffer in order to always draw manipulators on top!)
	m_LayerRoots.push_back( new g3dSceneNode() );
	this->AppendLayer( new g3dLayer( m_LayerRoots[e_ManipulatorLayer],
					g3dLayer::e_ZBuffer, g3dLayer::e_World, g3dLayer::e_Multiplicative,
					false, false, false, true) );

	// Register this viewer´s layers with the icon layer management,
	// so that this viewer gets a local copy of each icon.
	m_IconLayerIndex = icnIconLayer::AddIconLayers( m_LayerRoots[e_IconLayer], m_LayerRoots[e_ManipulatorLayer]);

	// Create the world axis for this viewer and place it in the 
	// camera space layer
	m_pWorldAxis = new cmpsWorldAxis;
	m_pWorldAxis->SetScale( maPoint3d( c_fScreenCompassRadian, c_fScreenCompassRadian, c_fScreenCompassRadian ) );
	m_pWorldAxis->SetPosition( maPoint3d(2.1f,-1.6f,2.5f) );
	m_LayerRoots[e_CameraLayer]->AddChild( m_pWorldAxis->GetSceneNode() );
}

//--------------------------------------------------------------------
// Deletes the renderer and viewer owned by this class.
//--------------------------------------------------------------------
rpnPanelViewer::~rpnPanelViewer()
{
	m_LayerRoots[e_CameraLayer]->RemoveChild( m_pWorldAxis->GetSceneNode() );
	delete m_pWorldAxis;

	delete m_pTextLabelProxy;
	if (m_pTextLabel)
	{
		m_LayerRoots[e_ScreenLayer]->RemoveChild( m_pTextLabel->GetSceneNode() );
		delete m_pTextLabel;
	}

	envSTLHelpers::DeleteContainer(m_LayerRoots);
}

//--------------------------------------------------------------------
// Return the layer index for this viewer's icons.
//--------------------------------------------------------------------
int rpnPanelViewer::GetIconLayerIndex() const
{
	return m_IconLayerIndex;
}


//--------------------------------------------------------------------
//	Prepare viewer´s layers for rendering
//--------------------------------------------------------------------
//virtual 
void rpnPanelViewer::Think( float i_fSimTime )
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
	
	// Update scale of icons that stay the same size in every camera pane
	int width = 0, height = 0;
	this->GetWindow()->GetDimensions(width, height);
	icnIconScale::UpdateIconsScale( m_IconLayerIndex, *GetCamera(), width );

	// Update scale of compasses for this view also
	cmpsCompassMgr::ResizeCompasses( *GetCamera(), m_IconLayerIndex );

	bool bShowWorldAxis = PrefsMgr::Data().m_bAxisCompassVisible.GetValue();
	m_pWorldAxis->SetRenderable(bShowWorldAxis);
	if (bShowWorldAxis)
	{
		//	update the world compass axis (it has a built in proxy)
		maMatrix4x4 matrix;
		maRotation rot;
		GetCamera()->GetCameraMatrix(matrix);
		rot.SetValue( matrix );
		m_pWorldAxis->SetOrientation( rot );

		// This axis is in camera space. In order to keep it in the 
		// lower left corner, we need to adjust when the camera's
		// aspect ratio changes.
		float aspect = GetCamera()->GetAspect();
		float yoff = (aspect > 0) ? (-2.0f / aspect) : -1.5f; 
		m_pWorldAxis->SetPosition( maPoint3d(2.1f,yoff,2.5f) );
	}

	// Update position of text label
	if (m_pTextLabelProxy)
	{
		// Try to keep the text in the bottom center of the window
		int vwidth = 0, vheight = 0;
		this->GetWindow()->GetVirtualResolution(vwidth, vheight);
		if (vwidth > 0 && vheight > 0)
		{
			maPoint3d position( vwidth / 2.0f, (float)(vheight - 16), 0.0f );
			if (position != m_pTextLabelProxy->GetPosition())
				m_pTextLabelProxy->SetPosition(position);
		}
	}
}

//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
//virtual 
void rpnPanelViewer::Render( float i_fSimTime, bool i_bClear )
{
	// Turn off shadows if needed
	bool bShadowsWereOn = g3dSingleLightRendering::GetDoSingleLightRendering();
	if (!m_bAllowShadows || rpnRenderingPrefs::GetAllFastRender())
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

	// Update transforms in icon layer since we have been changing the
	// scales of the icons above
	//GetScene()->UpdateLayerData(mnmApp::GetIconsLayerIndex());
	//GetScene()->UpdateLayerData(mnmApp::GetManipulatorsLayerIndex());
	// this transforms the icons that are specific to this viewer
	this->UpdateWorldData();

	// apply post effect setting
	pfxPostEffectMgr::ApplyPostEffect(pfxPostEffectMgr::e_ViewportPfx);

	// Do base class render, passing in our icon layers specific to this render panel
	g3dViewer::Render( i_fSimTime, this->GetLayers(),i_bClear );

	//bga - each viewer has its own text label, so not necessary anymore to turn it off
	// Turn off our text label so it doesn't show up in other views 
	// or in the capture renders
	//if (m_pTextLabel)
	//	m_pTextLabel->SetRenderable(false);

	// Restore the shadow state if we changed it
	if (!m_bAllowShadows || rpnRenderingPrefs::GetAllFastRender())
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
	DBG_ASSERT(m_pTextLabel == NULL, "Text label already created.");
			
	/*std::string filename;
	fsFileUtil::LocatorToANSIFilename(i_FontPath, filename);
	DBG_LOG( "RenderPanel, loading font file " << filename.c_str() );*/

	m_pTextLabel = scrCreator::MakeText( i_FontPath, this->GetWindow(), 320, 256 );
	m_pTextLabel->SetForegroundColor( maFloatRGBA(1,1,1,0) );
	m_pTextLabel->SetBackgroundColor( maFloatRGBA(0,0,0,1) );
	m_pTextLabel->SetPosition( maPoint3d( 10, 100, 0 ) );
	m_pTextLabel->SetCentered( true );
	m_pTextLabel->SetSize( 5, 8 );
	m_pTextLabel->SetRenderable(true);
	m_pTextLabel->SetText( itString("") );

	// Create thread safe proxy
	m_pTextLabelProxy = new gpxText(*m_pTextLabel);
			
	m_LayerRoots[e_ScreenLayer]->AddChild( m_pTextLabel->GetSceneNode() );
}

//------------------------------------------------------------------------
// SetTextVisible - turn display of text on/off
//------------------------------------------------------------------------
void rpnPanelViewer::SetTextVisible(bool i_bShow)
{
	//m_bTextVisible = i_bShow;
	if (m_pTextLabelProxy)
	{
		m_pTextLabelProxy->SetRenderable(i_bShow);
	}
}

//------------------------------------------------------------------------
// SetTextColor - set color of the camera name text label
//------------------------------------------------------------------------
void rpnPanelViewer::SetTextColor(const maFloatRGBA& i_Color)
{
	if (m_pTextLabelProxy)
	{
		m_pTextLabelProxy->SetForegroundColor( i_Color );
	}
}

//------------------------------------------------------------------------
// SetTextString - set string to display
//------------------------------------------------------------------------
void rpnPanelViewer::SetTextString(const std::string& i_String)
{
	if (m_pTextLabelProxy)
	{
		m_pTextLabelProxy->SetText( itString(i_String.c_str()) );
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
