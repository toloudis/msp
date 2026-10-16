/****************************************************************************\
**	tqcPictureBox.hpp
**
**	Panel that draws an image
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_PICTUREBOX_HPP
#error tqcPictureBox.hpp multiply included
#endif
#define TQC_PICTUREBOX_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT

//============================================================================
//============================================================================
class tqcPictureBox : public QWidget
{
	public:
		tqcPictureBox(QWidget *parent, const QSize& size = QSize());

		//--------------------------------------------------------------------
		// Load the image from a file, optionally resizing to fit it
		//--------------------------------------------------------------------
		void LoadImage(const QString& i_file, bool i_changePanelSize = false);
		void SetImage(const QPixmap& i_tex, bool i_changePanelSize = false);

	protected:
		virtual void paintEvent(QPaintEvent* i_pEvent);

	private:
		QPixmap m_texture;
};

#endif // USE_QT
