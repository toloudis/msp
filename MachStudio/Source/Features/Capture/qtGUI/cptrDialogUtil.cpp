/****************************************************************************\
**	cptrDialogUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/qtGUI/cptrDialogUtil.hpp"

#include "Support/capt/captRenderOutputTagsUtil.hpp"

#include "Core/it/itStringUtil.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "ToolUIQt/tqt/tqtSystem.hpp"

#ifdef USE_QT
#include <QDialog>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#endif
#include <list>
#include <sstream>
#include <iomanip>


//--------------------------------------------------------------------
// Show dialog with the valid render directory/file tags
//--------------------------------------------------------------------
void cptrDialogUtil::ShowTagsInfo()
{
	// Set the default text of the control.
	std::ostringstream taginfo;
	taginfo <<  "Render File and Directory Tags" << std::endl << std::endl;

	//	loop through the tags
	for (int i = 0; i < captRenderOutputTagsUtil::GetNumTags(); ++i)
	{
		itString tag = captRenderOutputTagsUtil::GetTag(i);
		taginfo << "<" << itStringUtil::GetStdString(tag).c_str() << ">" << std::endl;
	}

	taginfo << std::endl;
	taginfo << "Note: For counter tags (0 and #): the number of those characters within the tag indicates number of places.  <0000> - means 4 digits long, empty space filled with zeros.";
	taginfo << std::endl;

#ifdef USE_QT
	QDialog renderTagInfoDialog(tqtSystem::g_pMainForm);
	renderTagInfoDialog.setWindowTitle("Render Tag Info");
	renderTagInfoDialog.resize(300, 400);

	QVBoxLayout* bSizer1 = new QVBoxLayout( &renderTagInfoDialog );
	QPlainTextEdit* richText1 = new QPlainTextEdit( tqtQString(taginfo.str()), &renderTagInfoDialog );
	richText1->setReadOnly(true);
	richText1->setFrameShape(QFrame::NoFrame);
	bSizer1->addWidget( richText1, 1 );

	renderTagInfoDialog.exec();
#endif
}
