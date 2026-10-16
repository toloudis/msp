/****************************************************************************\
**	pqtKeyPropertyButton.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/private/pqtKeyPropertyButton.hpp"

#include "Core/Fs/fsFileUtil.hpp"
#include "Tool/gui/guiMenuMgr.hpp"

#ifdef USE_QT

namespace
{
	//------------------------------------------------------------------------
	// Full path of an icon in the icon directory
	//------------------------------------------------------------------------
	QString icon_path(const char* i_pFilename)
	{
		fsLocator loc(guiMenuMgr::GetIconDirectory());
		loc.Push(i_pFilename);
		itString filename;
		fsFileUtil::LocatorToUnicodeString(loc, filename);
		return QString::fromWCharArray(filename.GetString());
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
pqtKeyPropertyButton::pqtKeyPropertyButton(QWidget* i_pParent,
											 const std::string& i_PropertyName,
											 KeyPropertyFunction i_KeyPropertyFunction)
:	QToolButton( i_pParent ),
	m_PropertyName(i_PropertyName),
	m_KeyPropertyFunction(i_KeyPropertyFunction)
{
	QIcon icon(icon_path("property-key.PNG"));
	icon.addFile(icon_path("property-key_highlight.PNG"), QSize(), QIcon::Active);
	icon.addFile(icon_path("property-key_pressed.PNG"), QSize(), QIcon::Selected);
	this->setIcon(icon);
	this->setAutoRaise(true);
	this->setToolTip("Key this property");

	QObject::connect(this, &QToolButton::clicked, [this]() { DoKeyProperty(); });
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pqtKeyPropertyButton::DoKeyProperty()
{
	// Call user function to key this property
	if (m_KeyPropertyFunction)
	{
		(*m_KeyPropertyFunction)(m_PropertyName);
	}
}

#endif // USE_QT
