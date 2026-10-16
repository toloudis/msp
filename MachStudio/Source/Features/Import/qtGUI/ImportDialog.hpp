/*****************************************************************************
**	ImportDialog.hpp
**
**	Dialog for importing parts of another scene, written in Qt
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef IMPORTDIALOG_HPP
#error ImportDialog.hpp multiply included
#endif
#define IMPORTDIALOG_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif
#ifndef IMPORT_DATA_HPP
#include "Features/Import/ImportData.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif

#include <map>

//============================================================================
//============================================================================
class docDocument;
class QCheckBox;
class QTreeWidget;
class tqcFilePicker;

#ifdef USE_QT

//----------------------------------------------------------------------------
// Class ImportDialog
//----------------------------------------------------------------------------
class ImportDialog : public QDialog
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		ImportDialog( QWidget* parent, ImportData& i_Data);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~ImportDialog();

	private:
		//--------------------------------------------------------------------
		// private functions
		//--------------------------------------------------------------------
		void load_document(const fsLocator& i_ImportLoc);
		void fillintree();
		void update_ImportDataList();

		//--------------------------------------------------------------------
		// event handling
		//--------------------------------------------------------------------
		void filePicker_FileChanged();
		void buttonImport_Click();

		tqcFilePicker* m_filePicker1;
		QCheckBox* m_checkBox_AllowDupes;
		QTreeWidget* m_treeCtrl_Import;

		ImportData& m_Data;
		shared_ptr<docDocument> m_pDoc;
};

#endif // USE_QT
