/****************************************************************************\
**	tqcPictureBox.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/private/tqcPictureBox.hpp"

#ifdef USE_QT

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcPictureBox::tqcPictureBox(QWidget *parent, const QSize& size)
:	QWidget(parent)
{
	if (size.isValid())
		this->resize(size);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcPictureBox::LoadImage(const QString& i_file, bool i_changePanelSize)
{
	if (!i_file.isEmpty())
		SetImage(QPixmap(i_file), i_changePanelSize);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcPictureBox::SetImage(const QPixmap& i_tex, bool i_changePanelSize)
{
	m_texture = i_tex;
	if (!m_texture.isNull() && i_changePanelSize)
		this->setFixedSize(m_texture.size());
	this->update();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcPictureBox::paintEvent(QPaintEvent*)
{
	if (!m_texture.isNull())
	{
		QPainter painter(this);
		painter.drawPixmap(0, 0, m_texture);
	}
}

#endif // USE_QT
