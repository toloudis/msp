/*****************************************************************************
**	rdrLayersDialog.hpp
**
**		implementation of the render layer dialog, in Qt
**
\****************************************************************************/
#ifdef RDR_LAYERSDIALOG_HPP
#error rdrLayersDialog.hpp multiply included
#endif
#define RDR_LAYERSDIALOG_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif

#include <string>

#ifdef USE_QT

class rlyrPassesObject;
class prtyObject;
class captRenderOutputObject;
class rprfPrefsObject;
class pfxPostEffectObject;
class QLabel;
class QTabWidget;
class QTreeWidget;
class QTreeWidgetItem;
class QVBoxLayout;

//============================================================================
// Class rdrLayersDialog - tree of render layers with tab pages for the
//	selected layer's objects, render prefs, passes and post effects.
//============================================================================
class rdrLayersDialog : public QWidget
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static rdrLayersDialog* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		rdrLayersDialog( QWidget* parent,
						  const std::string& i_Title = "Render Layers",
						  const std::string& i_Caption = "Render Layers" );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~rdrLayersDialog();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Update();
		virtual void Draw();
		virtual void ResetSelection();
		virtual void UpdateTabPages(rprfPrefsObject* i_RenderPrefs = NULL,
			rlyrPassesObject* i_RenderPasses = NULL,
			pfxPostEffectObject* i_RenderPfx = NULL);
		virtual void UpdatePfxShader();

	protected:
		//--------------------------------------------------------------------
		// Delete key in the tree
		//--------------------------------------------------------------------
		bool eventFilter(QObject* i_pWatched, QEvent* i_pEvent) override;

	private:
		void UpdateLayerTree();
		void UpdateObjectList();
		void UpdateRenderPrefs(rprfPrefsObject* i_RenderPrefs = NULL);
		void UpdatePassesList(rlyrPassesObject* i_RenderPasses = NULL);
		void UpdatePfx(pfxPostEffectObject* i_RenderPfx = NULL);

		QTreeWidgetItem* add_layer_item(const std::string& i_Name, bool i_bActive);
		void update_root_state();
		void deleteSelection();

		//--------------------------------------------------------------------
		// event handling
		//--------------------------------------------------------------------
		void layerTree_ItemChanged( QTreeWidgetItem* i_pItem );
		void layerTree_CurrentChanged( QTreeWidgetItem* i_pItem );
		void AddButtonClick();
		void DuplicateButtonClick();
		void RenameButtonClick();

		QTreeWidget* m_layerTree;
		QTreeWidgetItem* m_pRoot;
		QLabel* m_layerNameText;
		QTabWidget* m_layerTabPages;

		QWidget* m_pTabPage_Pfx;
		QVBoxLayout* m_pTabPage_Pfx_Sizer;
		QWidget* m_pTabPage_Pfx_Shader;

		QTreeWidgetItem* m_curSelection;
		int m_CurPage;
		bool m_bUpdating;
};

#endif
