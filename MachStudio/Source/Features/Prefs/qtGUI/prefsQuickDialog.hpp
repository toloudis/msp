/*****************************************************************************
**  prefsQuickDialog.hpp
**
**     Popup grid of buttons for the quick commands
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifndef __prefsQuickDialog__
#define __prefsQuickDialog__

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifndef PREFS_QUICKDATA_HPP
#include "Features/Prefs/prefsQuickData.hpp"
#endif

#ifdef USE_QT

class QGridLayout;

///////////////////////////////////////////////////////////////////////////////
/// Class prefsQuickDialog
///////////////////////////////////////////////////////////////////////////////
class prefsQuickDialog : public QDialog
{
	protected:
		void mousePressEvent( QMouseEvent* event ) override;

	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prefsQuickDialog( QWidget* parent );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~prefsQuickDialog();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void BuildButtons(const prefsQuickData& i_Data, const int i_ScreenX, const int i_ScreenY);

	private:
		void execute_command(const std::string& i_Tag);

		QGridLayout* m_Sizer;
};

#endif // USE_QT
#endif //__prefsQuickDialog__
