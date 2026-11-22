/*****************************************************************************
**  fgmtUVEditor.hpp
**
**     Pane showing the UVs of a fragment, when using Qt
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef FGMT_UVEDITOR_HPP
#error fgmtUVEditor.hpp multiply included
#endif
#define FGMT_UVEDITOR_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT

class camCamera;
class tqcRenderPanel;
class g3dFragment;
class g3dScene;
class g3dSceneNode;
class g3dSceneRenderer;

//============================================================================
//============================================================================
class fgmtUVEditor : public QWidget
{
public:
    // ctor(s)
    fgmtUVEditor(QWidget* parent, const std::string& i_Title);

    // dtor
    ~fgmtUVEditor();

	// Access to the render area
	tqcRenderPanel* GetRenderWindow();

	// Set the node with fragment to edit.
	void SetNode(g3dSceneNode* i_pNode);

protected:
	void resizeEvent(QResizeEvent* i_pEvent) override;

private:
    tqcRenderPanel* m_pRenderPanel;

	// we have a scene with a single node to draw, in a single layer
	g3dScene* m_pScene;
	g3dSceneNode* m_pRootNode;
	g3dSceneRenderer* m_pRenderer;
	camCamera* m_pCamera;
};

#endif // USE_QT
