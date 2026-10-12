/*****************************************************************************
**  fgmtUVEditor.cpp
**
**     see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Support/fgmt/qtGUI/fgmtUVEditor.hpp"

#include "Support/fgmt/GUI/fgmtDialogUtil.hpp"

#include "Graphics/cam/camCamera.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#include "Graphics/g3d/g3dSceneRendererCreate.hpp"
#include "ToolUIQt/tqc/tqcRenderPanel.hpp"
#include "ToolUIQt/tqt/tqtPaneMgr.hpp"

#ifdef USE_QT

#include <QHBoxLayout>

//--------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------
fgmtUVEditor::fgmtUVEditor(QWidget* parent, const std::string& i_Title)
: QWidget(parent),
	m_pRenderPanel(NULL),
	m_pScene(NULL),
	m_pRootNode(NULL),
	m_pRenderer(NULL),
	m_pCamera(NULL)
{
	// Setup panel for the render area
	QHBoxLayout* pSizer = new QHBoxLayout(this);
	pSizer->setContentsMargins(0, 0, 0, 0);
	m_pRenderPanel = new tqcRenderPanel( this, fgmtDialogUtil::GetSystem() );
	pSizer->addWidget( m_pRenderPanel, 1 );

	// i own the scene, but the renderpanel will use it.
	m_pScene = new g3dScene(new g3dLayer());
	m_pRootNode = new g3dSceneNode(NULL);
	m_pScene->GetLayer(0)->SetRootNode(m_pRootNode);
	m_pRenderPanel->SetScene(m_pScene);

	// i own the camera, but the renderpanel will use it.
	// ortho camera where the SetSubViewport will define the coordinate system.
	m_pCamera = new camCamera();
	m_pCamera->SetOrthographic(true);
	m_pCamera->SetClip(0,1);
	m_pCamera->SetAspect(1);
	m_pRenderPanel->SetCamera(m_pCamera);

	// i own the renderer, but the renderpanel will use it.
	m_pRenderer = g3dSceneRendererCreate::CreateUVRenderer();
	m_pRenderPanel->SetRenderer(m_pRenderer);

	// Add this panel to the pane manager
	tqtPaneMgr::AddPane(this, tqtPaneInfo().Name(i_Title).Caption(i_Title).Show().Layer(1).Left().BestSize(400, 400));
}

//--------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------
fgmtUVEditor::~fgmtUVEditor()
{
	// assumes singleton owned by fgmtDialogUtil!!!
	fgmtDialogUtil::NotifyUVEditorDestroyed();

	delete m_pRenderer;
	if (m_pRootNode)
		delete m_pRootNode;
	delete m_pScene;
	delete m_pCamera;
}

//--------------------------------------------------------------------
// Access to the render areas
//--------------------------------------------------------------------
tqcRenderPanel* fgmtUVEditor::GetRenderWindow()
{
	return m_pRenderPanel;
}

//--------------------------------------------------------------------
// Set the fragment to edit.
//--------------------------------------------------------------------
void fgmtUVEditor::SetNode(g3dSceneNode* i_pNode)
{
	// clean up old root
	m_pScene->GetLayer(0)->SetRootNode(NULL);
	delete m_pRootNode;
	m_pRootNode = NULL;

	// set new root
	m_pScene->GetLayer(0)->SetRootNode(i_pNode);

	// redraw!
	m_pRenderPanel->update();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void fgmtUVEditor::resizeEvent(QResizeEvent* i_pEvent)
{
	QWidget::resizeEvent(i_pEvent);

	// keep -1..1 centered in the view.
	if (m_pCamera && this->height() > 0)
	{
		float aspect = (float)this->width()/(float)this->height();
		if (aspect >= 1)
			m_pCamera->SetSubViewport(1, -1, -aspect, aspect);//t,b,l,r
		else
			m_pCamera->SetSubViewport(1/aspect, -1/aspect, -1, 1);// t,b,l,r
		m_pRenderPanel->update();
	}
}

#endif // USE_QT
