/****************************************************************************\
**	tqcDirPicker.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/tqcDirPicker.hpp"

#ifdef USE_QT

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcDirPicker::tqcDirPicker(QWidget* i_pParent)
:	QWidget(i_pParent)
{
	QHBoxLayout* bSizer1 = new QHBoxLayout(this);
	bSizer1->setContentsMargins(0, 0, 0, 0);
	bSizer1->setSpacing(0);

	m_pTextBox = new QLineEdit(this);
	m_pTextBox->setMinimumWidth(150);
	QObject::connect(m_pTextBox, &QLineEdit::editingFinished, [this]()
	{
		if (m_pTextBox->text() != m_LastPath)
		{
			m_LastPath = m_pTextBox->text();
			NotifyValueChanged();
		}
	});
	bSizer1->addWidget( m_pTextBox, 1 );

	m_pBrowseButton = new QPushButton("...", this);
	m_pBrowseButton->setFixedWidth(30);
	QObject::connect(m_pBrowseButton, &QPushButton::clicked, [this]() { OnBrowseClick(); });
	bSizer1->addWidget( m_pBrowseButton, 0 );
}

//--------------------------------------------------------------------
//	Path
//--------------------------------------------------------------------
QString tqcDirPicker::GetPath() const
{
	return m_pTextBox->text();
}
void tqcDirPicker::SetPath(const QString& i_Path)
{
	m_LastPath = i_Path;
	m_pTextBox->setText(i_Path);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcDirPicker::OnBrowseClick()
{
	QString path = QFileDialog::getExistingDirectory(this, "Choose Directory", GetPath());
	if (!path.isEmpty())
	{
		SetPath(QDir::toNativeSeparators(path));
		NotifyValueChanged();
	}
}

#endif // USE_QT
