/*****************************************************************************
**  tqcRenderPanel.cpp
**
**     see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/tqcRenderPanel.hpp"

#include "Core/app/appTime.hpp"
#include "Core/dbg/dbgMsg.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g2d/g2dSystem.hpp"
#include "Graphics/g3d/g3dViewer.hpp"

#ifdef USE_QT

//--------------------------------------------------------------------
// canvas constructor
//--------------------------------------------------------------------
tqcRenderPanel::tqcRenderPanel(QWidget* parent, g2dSystem* i_pSystem)
	: tqcNativeWindow(parent),
	m_pSystem(i_pSystem),
	m_pViewer(NULL)
{
	// don't take focus (for now?)
	this->setFocusPolicy(Qt::NoFocus);

	m_pWindow = i_pSystem->CreateSubWindow(this->GetHandle());
	m_pViewer = new g3dViewer(m_pWindow, NULL);
	m_pViewer->SetBackgroundColor(g2dRGBColor(0,0,0));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tqcRenderPanel::~tqcRenderPanel()
{
	delete m_pViewer;
	// hopefully g2dSystem is still alive? Is there a better way to access this globally?
	m_pSystem->DestroyWindow(m_pWindow);
}

//--------------------------------------------------------------------
// The renderer is not owned by the viewer, just pointed to
//--------------------------------------------------------------------
g3dSceneRenderer* tqcRenderPanel::GetRenderer() const
{
	return m_pViewer->GetRenderer();
}
void tqcRenderPanel::SetRenderer(g3dSceneRenderer* i_pRenderer)
{
	m_pViewer->SetRenderer(i_pRenderer);
}

//--------------------------------------------------------------------
// The scene is not owned by the viewer, just pointed to
//--------------------------------------------------------------------
g3dScene* tqcRenderPanel::GetScene() const
{
	return m_pViewer->GetScene();
}
void tqcRenderPanel::SetScene(g3dScene* i_pScene)
{
	m_pViewer->SetScene(i_pScene);
}

//--------------------------------------------------------------------
// The camera is not owned by the viewer, just pointed to
//--------------------------------------------------------------------
camCamera* tqcRenderPanel::GetCamera() const
{
	return m_pViewer->GetCamera();
}
void tqcRenderPanel::SetCamera(camCamera* i_pCamera)
{
	m_pViewer->SetCamera(i_pCamera);
}

//--------------------------------------------------------------------
// Set background color
//--------------------------------------------------------------------
void tqcRenderPanel::SetBackgroundColor(const g2dRGBColor& i_Color)
{
	m_pViewer->SetBackgroundColor(i_Color);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tqcRenderPanel::do_resize()
{
	if (this->m_pViewer != NULL)
	{
		if (this->width() > 0 && this->height() > 0)
		{
			// Resize the underlying window
			m_pViewer->GetWindow()->ResizeWindow(PhysicalWidth(), PhysicalHeight());

			// Setting virtual resolution will keep text the same size
			// when the window resizes and will avoid stretching when
			// aspect ratio changes
			m_pViewer->GetWindow()->SetVirtualResolution(this->width(), this->height());
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tqcRenderPanel::resizeEvent(QResizeEvent* i_pEvent)
{
	tqcNativeWindow::resizeEvent(i_pEvent);
	this->do_resize();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tqcRenderPanel::paintEvent(QPaintEvent*)
{
	if (m_pViewer->GetRenderer() != NULL)
	{
		m_pViewer->Render(appTime::GetTime());
		m_pViewer->Present();
	}
}

#endif // USE_QT
