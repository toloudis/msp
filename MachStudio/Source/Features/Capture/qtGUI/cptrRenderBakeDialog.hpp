/*****************************************************************************
**	cptrRenderBakeDialog.hpp
**
**		implementation of the render baking texture dialog
**
\****************************************************************************/
#ifdef CPTR_RENDERBAKEDIALOG_HPP
#error cptrRenderBakeDialog.hpp multiply included
#endif
#define CPTR_RENDERBAKE_HPP

//	needed before App so USE_QT is set
#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifndef CPTR_RENDERBAKEDATA_HPP
#include "Features/Capture/cptrRenderBakeData.hpp"
#endif

#include <string>


#ifdef USE_QT
//----------------------------------------------------------------------------
// Forward References
//----------------------------------------------------------------------------
class rprfPrefsObject;

//============================================================================
// Class RdrLayersDialog
//============================================================================
class QTabWidget;

class cptrRenderBakeDialog : public QDialog
{
	public:
		static cptrRenderBakeDialog* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cptrRenderBakeDialog(cptrRenderBakeData& i_Data, QWidget* parent);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~cptrRenderBakeDialog();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Update();

	protected:
		void UpdateTabbedPage();
		void UpdateObjectList();
		void RegisterRenderPrefs();

		void closeEvent( QCloseEvent* i_pEvent ) override;

	private:
		void button_bake_OnButtonClick();

		QTabWidget* m_layerTabPages;
		QWidget* m_scrolledWindow_Prefs;
		cptrRenderBakeData m_Data;
		bool m_bakeAborted;
};
#endif
