/****************************************************************************\
**	pqtCategoryLabel.hpp
**
**	Header of a category of property controls. Clicking it collapses or
**	expands the widget holding the category's controls.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_CATEGORYLABEL_HPP
#error pqtCategoryLabel.hpp multiply included
#endif
#define PQT_CATEGORYLABEL_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT

//============================================================================
//============================================================================
class pqtCategoryLabel : public QWidget
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		pqtCategoryLabel(QWidget* i_pParent,
						 const std::string& i_DialogName,
						 const QString& i_LabelText);

		//--------------------------------------------------------------------
		// The widget holding the category's controls, shown or hidden
		// when the label is clicked
		//--------------------------------------------------------------------
		void SetGroup(QWidget* i_pGroup);

		//----------------------------------------------------------------------------
		// Return true if this category label should be initially collapsed
		// based on what the user has done previously.
		//----------------------------------------------------------------------------
		bool GetInitialCollapsedState() const;

		//----------------------------------------------------------------------------
		// Colors of the label
		//----------------------------------------------------------------------------
		void SetColors(const QColor& i_Text, const QColor& i_Background);

		virtual QSize sizeHint() const;

	protected:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void paintEvent(QPaintEvent* i_pEvent);
		virtual void mousePressEvent(QMouseEvent* i_pEvent);

	private:
		std::string m_DialogName;
		QString m_LabelText;
		QPointer<QWidget> m_pGroup;
		QColor m_TextColor;
		QColor m_BackgroundColor;
};

#endif // USE_QT
